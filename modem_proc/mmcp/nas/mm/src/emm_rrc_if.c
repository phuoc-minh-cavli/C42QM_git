
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

$Header: //components/rel/mmcp.mpss/6.1.10/nas/mm/src/emm_rrc_if.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/10/14   pahire  Added changes for UE Specific DRX Cycle for QChat/LTE
06/16/11   PA      Added changes for Rel 9 Emergency Bearer services 
05/27/11   PA      Added changes for Security Key generation during W2L PS HO
07/01/10   zren    Updated EMM to get eplmn list from REG upon loading sim data
06/07/10   RI      Lint fixes.
05/11/10   RI      Mainlined spec changes upto Dec 09.
05/11/10   zren    Updated to use trans ID rcved in PLMN SEARCH REQ
05/07/10   zren    Fixed and consolidated equivalent PLMN list handling
05/04/10   zren    Added support for MME routing info setup in RRC CONN EST REQ
                   for different TIN types
04/06/10   zren    Enabled using rat_pri_list_info in REG PLMN SEARCH REQ
03/24/10   zren    Updated to send RRC DEACTIVATE REQ with cause ALT SCAN w/o
                   aborting connetion firstly
03/03/10   MNK     Added LTE_RRC_ACTIVATION_RSP 
02/24/10   zren    Added support for network selection modes of LIMITED MANUAL
                   ACQUISITION and LIMITED ROUTINE ACQUISITIONQUISITION
02/24/10   MNK     Copy RAT pri to the MM global variable 
02/16/10   MNK     Add ignore_GW_activation_ind
02/12/10   zren    Updated to always abort the existing connection before 
                   sending RRC DEACT REQ
02/05/10   zren    Updated processing of connection abort in waiting for RRC 
                   confirmation state
01/22/10   RI      Added support for handling multiple EPS contexts.
01/21/10   MNK     Set the active rat to LTE when MMR_REG_REQ received
01/07/10   zren    Updated release client and pended message processing
12/10/09   zren    Updated to make routing information inclusion in 
                   rrc conn req spec compliant
10/21/09   RI      Deleted saving of security context for each UL Data request.
10/21/09   vdr     rrc_service_req.rat_pri_list.num_items = 0
09/21/09   MNK     Changes due to IRAT design
09/17/09   RI      Security context info to be sent to RRC if first attach request
                   is sent integrity protected
09/16/09   zren    Added support for PDN_CONNECTIVITY_REQ msg req/rsp feature
07/21/09   zren    Added support to set high priority flag in RRC_CONN_EST_REQ
07/15/09   zren    Modified message list handling functions to use message 
                   pointer as parameter
06/23/09   zren    Added support of processing EPLMN list, forbidden PLMN list,
                   forbidden TAI list for roaming and forbidden TAI list for
                   regional provision of service 
06/18/09   zren    Modified power off detach success CB function to abort 
                   signaling connection
06/13/09   RI      Updated the NAS security processing.
05/29/09   zren    Moved reg container setup from emm_send_rrc_service_req to
                   emm_process_reg_req
05/17/09   hnam    Added the code to support security
05/15/09   hnam    Added hplmn value assignment in emm_send_service_request
05/07/09   zren    Added support to set reg container instead of sending out 
                   RRC_SERVICE_REQUEST if EMM is in EMM_CONNECTED_STATE or 
                   EMM_WAITING_FOR_RRC_CONFIRMATION_STATE
04/17/09   zren    Modified to support saving NAS messages in pended message 
                   list in the format of lte_nas_outgoing_msg_type
                   Added one parameter to function emm_send_rrc_connection_establishment_request() 
                   Sepearted RRC_DATA_CNF and RRC_EST_CNF for RRC_EST_REQ
                   Modified EMM connection state machines
04/14/09   RI      Added cell barring flag to emm_rrc_abort_connection()
04/09/09   vdr     Fixed conversion of the NAS PLMN ID into RRC PLMN ID upon sending RRC EST REQ
03/10/09   hnam    Initilize rrc_service_req & rrc_system_update_req to zero (Fix for CR 174915)
02/26/09   vdr     Fixed Lint errors and warnings
01/29/09   vdr     Added two new functions emm_send_sim_update() and emm_send_rrc_system_update
02/20/09   vdr     Now evaluating return value of the emm_msgr_send()
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
#include "emm_rrc_if.h"
#include "msgr.h"
#include "msgr_lte.h"
#include "lte_nas_emm_message_description.h"
#include "emm_utility.h"
#include "lte_nas_security_info.h"
#include "emm_security.h"
#include "err.h"
#include "msgr_nas.h"
#include "emm_connection_handler.h"
#include "mm_multimode_common.h"
#include "emm_csfb_handler.h"
#include <dsm_item.h>
#include <dsm_init.h>
#include "emm_mem.h"
#include "stringl.h"
#include "gmm_int_v.h"
#include "emm_cnm_handler.h"
#include "reg_mode.h"
#include "msg_lib_encode_esm.h"
#include "reg_state.h"
#include "lte_rrc_ext_api.h"
#include "dog_hb_rex.h"
#include "ULogFront.h"

extern void emm_set_mm_msg_info_for_debug_buffer(byte msg_info);

extern ULogHandle mm_get_ulog_handle();
boolean is_last_stop_reason_diff = FALSE;

/*===========================================================================

FUNCTION EMM_SET_RRC_FORBIDDEN_TA_LIST

DESCRIPTION
  This function populates the RRC forbidden TAI list for roaming and RRC 
  forbidden TAI list for manual selection
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_set_rrc_forbidden_ta_list
(
  lte_rrc_tracking_area_list_s  *rrc_fta_list_ptr,
  lte_rrc_tracking_area_list_s  *rrc_manual_fta_list_ptr, 
  emm_ctrl_data_type            *emm_ctrl_data_ptr
)
{
  boolean  plmn_id_is_undefined;
  boolean  mnc_includes_pcs_digit;
  uint32   mcc;
  uint32   mnc;
  byte     index = 0;

  /* FTAI list for roaming */

  rrc_fta_list_ptr->num_entries = 
                          emm_ctrl_data_ptr->rrc_forbidden_list_ptr->tai_lst_length;

  MSG_HIGH_DS_1(MM_SUB, "=EMM= Populating RRC F TA list for roaming - length %d",
                rrc_fta_list_ptr->num_entries);
  
  for(index=0; index < rrc_fta_list_ptr->num_entries && index < LTE_RRC_MAX_NUM_TRACKING_AREAS; index++)
  {
    rrc_fta_list_ptr->tracking_area[index].plmn = 
               emm_convert_nas_plmn_id_to_rrc_plmn_id
                 (emm_ctrl_data_ptr->rrc_forbidden_list_ptr->diff_plmn[index].plmn);

    rrc_fta_list_ptr->tracking_area[index].tac = 
                    emm_ctrl_data_ptr->rrc_forbidden_list_ptr->diff_plmn[index].tac;

    sys_plmn_get_mcc_mnc(emm_ctrl_data_ptr->rrc_forbidden_list_ptr->diff_plmn[index].plmn,
                         &plmn_id_is_undefined,
                         &mnc_includes_pcs_digit,
                         &mcc,
                         &mnc);

    MSG_HIGH_DS_3(MM_SUB, "=EMM= RRC F TA (Roaming) - PLMN %d-%d, TAC %d",
                  mcc,
                  mnc,
                  emm_ctrl_data_ptr->rrc_forbidden_list_ptr->diff_plmn[index].tac);
  }

  /* FTAI list for manual selection */
  (void)pthread_mutex_lock(emm_ctrl_data_ptr->manual_ftai_list_mutex_ptr);

  rrc_manual_fta_list_ptr->num_entries = 
                            emm_ctrl_data_ptr->manual_ftai_list_ptr->tai_lst_length;
  
  MSG_HIGH_DS_1(MM_SUB, "=EMM= Populating RRC F TA list for manual selection - length %d",
                rrc_manual_fta_list_ptr->num_entries);

  for(index=0; index < rrc_manual_fta_list_ptr->num_entries && index < LTE_RRC_MAX_NUM_TRACKING_AREAS; index++)
  {
    rrc_manual_fta_list_ptr->tracking_area[index].plmn = 
               emm_convert_nas_plmn_id_to_rrc_plmn_id
                   (emm_ctrl_data_ptr->manual_ftai_list_ptr->diff_plmn[index].plmn);

    rrc_manual_fta_list_ptr->tracking_area[index].tac = 
                      emm_ctrl_data_ptr->manual_ftai_list_ptr->diff_plmn[index].tac;

    sys_plmn_get_mcc_mnc(emm_ctrl_data_ptr->manual_ftai_list_ptr->diff_plmn[index].plmn,
                         &plmn_id_is_undefined,
                         &mnc_includes_pcs_digit,
                         &mcc,
                         &mnc);

    MSG_HIGH_DS_3(MM_SUB, "=EMM= RRC F TA (Manual) - PLMN %d-%d, TAC %d",
                  mcc,
                  mnc,
                  emm_ctrl_data_ptr->manual_ftai_list_ptr->diff_plmn[index].tac);
  }

  (void)pthread_mutex_unlock(emm_ctrl_data_ptr->manual_ftai_list_mutex_ptr);

}/* emm_set_rrc_forbidden_ta_list */


/*===========================================================================

FUNCTION EMM_SEND_RRC_SERVICE_REQ

DESCRIPTION
  This function sends RRC SERVICE REQ to RRC

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_send_rrc_service_req
(
  //lte_rrc_svc_req_type_e svc_req_type, /*!< Type of request */

  /* network select mode */
  sys_network_selection_mode_e_type network_select_mode,

  boolean req_plmn_info_is_valid, /*!< Is requested PLMN valid */

  /* valid for svc_req_type of LTE_RRC_SVC_REQ_NORMAL only */
  sys_plmn_id_s_type *req_plmn, /*!< Requested PLMN */

  boolean hplmn_info_is_valid,  

  boolean ehplmn_info_is_valid,  

  boolean eplmn_info_is_valid,  

  boolean scan_is_new, /*!< Is service request new or not */

  boolean use_timer, /*!< True = use timer (OOOS), False = don't use timer 
                          (Non-UOOS) */

  /* valid only if use_timer is TRUE */
  dword lte_scan_time, /*!< UOOS search timer for LTE in ms */

  /* service search type TBD */
  emm_ctrl_data_type *emm_ctrl_data_ptr,

  sys_rat_pri_list_info_s_type *rat_pri_list,

  /* request type from reg */
  mmr_reg_req_e_type reg_req_type

#ifdef FEATURE_LTE_REL9
  , 
  uint32  additional_info
#endif                      
#if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW)
  ,sys_csg_id_type  csg_id
#endif 
  ,sys_scan_scope_e_type scan_scope  
  ,uint32    trm_timeout
#ifdef FEATURE_CIOT
    ,boolean new_scan /*!< Is service request new or not */

  ,boolean apply_timer /*!< True = use timer (OOS), False = don't use timer 
                          (OOS) */

  /* valid only if use_timer is TRUE */
  ,uint32 search_time /*!< OOS search timer for LTE in ms */ //FRACQ

 ,boolean nb1_acq_tier3_timer_status

 ,boolean nb1_fullband_tier3_timer_status
#endif

)
{
  lte_rrc_service_req_s *rrc_service_req_ptr = NULL;
  byte                  index = 0 ;
  emm_failure_type      emm_failure_cause;
#ifdef FEATURE_CIOT
  sys_radio_access_tech_e_type current_loaded_rat = emm_ctrl_data_ptr->loaded_lte_rat;
#endif
  /*allocate memory on heap*/
  rrc_service_req_ptr = (lte_rrc_service_req_s*)emm_modem_mem_alloc(
                            sizeof(lte_rrc_service_req_s),
                             emm_ctrl_data_ptr);

  mm_check_for_null_ptr((void*)rrc_service_req_ptr);


  msgr_init_hdr((msgr_hdr_s *)&rrc_service_req_ptr->msg_hdr, MSGR_NAS_EMM, LTE_RRC_SERVICE_REQ);

  rrc_service_req_ptr->trans_id = emm_get_reg_init_rrc_trans_id() ;
  
  rrc_service_req_ptr->ue_is_poweron = reg_state_get_power_up_system_selection();
  memset((void *)&emm_failure_cause,0,sizeof(emm_failure_type));
#ifdef FEATURE_FMD_SPEED_INFO
  rrc_service_req_ptr->pseudo_online_mode = FALSE;
#endif
  switch(network_select_mode)
  {
    case SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION:
      /* RRC does not support LIMITED_MANUAL_ACQUISITION yet. Map it to LIMITED */
      rrc_service_req_ptr->network_select_mode = SYS_NETWORK_SELECTION_MODE_LIMITED;
      break;

#ifdef FEATURE_1XSRLTE
    case SYS_NETWORK_SELECTION_MODE_AUTOMATIC_CAMP_ONLY:
      rrc_service_req_ptr->network_select_mode = SYS_NETWORK_SELECTION_MODE_AUTOMATIC;
      break;

    case SYS_NETWORK_SELECTION_MODE_MANUAL_CAMP_ONLY:
      rrc_service_req_ptr->network_select_mode = SYS_NETWORK_SELECTION_MODE_MANUAL;
      break;
#endif 
    default:
      rrc_service_req_ptr->network_select_mode = network_select_mode;
      break;
  }

  rrc_service_req_ptr->req_plmn_info_is_valid = req_plmn_info_is_valid ;
  if((req_plmn_info_is_valid == TRUE) && (req_plmn != NULL))
  {
    rrc_service_req_ptr->req_plmn = emm_convert_nas_plmn_id_to_rrc_plmn_id(*req_plmn) ;
  }
 
  if(rat_pri_list != NULL)
  {
    rrc_service_req_ptr->rat_pri_list = *rat_pri_list;

    /* Remove the GSM rat, if present, from the priority list to be sent to RRC */    
    rrc_service_req_ptr->rat_pri_list.num_items = 1;

    /*MNK: Copy the RAT PRI LIST to the MM global variable*/
    NAS_ENTER_CRIT_SECT(mm_rat_prioritylist_crit_sec);
    mm_rat_pri_list_info = *rat_pri_list;

    NAS_EXIT_CRIT_SECT(mm_rat_prioritylist_crit_sec);
  }
  
  /* RPLMN */
  emm_set_rrc_rplmn(&rrc_service_req_ptr->rplmn_info_is_valid,
                    &rrc_service_req_ptr->rplmn,
                    emm_ctrl_data_ptr);

/* Populate EHPLMN list or HPLMN */
  rrc_service_req_ptr->hplmn_info_is_valid = FALSE;
  rrc_service_req_ptr->ehplmn_list.num_plmns = 0;

  if((emm_ctrl_data_ptr->plmn_service_state->sim_state == SYS_SIM_STATE_AVAILABLE) || 
     (emm_ctrl_data_ptr->plmn_service_state->sim_state == SYS_SIM_STATE_CS_INVALID))
  {
#ifdef FEATURE_EQUIVALENT_HPLMN
  /* EHPLMN */
  if(ehplmn_info_is_valid == TRUE)
  {
    reg_sim_plmn_list_s_type *ehplmn_list_ptr;
    ehplmn_list_ptr = (reg_sim_plmn_list_s_type *)emm_modem_mem_alloc(
                                              sizeof(reg_sim_plmn_list_s_type),
                                              emm_ctrl_data_ptr);
    mm_check_for_null_ptr((void *)ehplmn_list_ptr);
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
    reg_sim_per_nas_stacks_ehplmn_list_get_ptr(ehplmn_list_ptr,  (sys_modem_as_id_e_type)mm_as_id);
#else
    reg_sim_ehplmn_list_get_ptr(ehplmn_list_ptr);
#endif
    if(ehplmn_list_ptr->length > SYS_EHPLMN_LIST_MAX_LENGTH)
    {
      MSG_HIGH_DS_1(MM_SUB, "=EMM= EHPLMN list too long %d - RRC SRV REQ",
                    ehplmn_list_ptr->length);
      rrc_service_req_ptr->ehplmn_list.num_plmns = SYS_EHPLMN_LIST_MAX_LENGTH;
    }
    else
    {
      rrc_service_req_ptr->ehplmn_list.num_plmns = 
                                     ehplmn_list_ptr->length;
    }

    for(index=0; index<rrc_service_req_ptr->ehplmn_list.num_plmns; index++)
    {
      rrc_service_req_ptr->ehplmn_list.plmn[index] = 
        emm_convert_nas_plmn_id_to_rrc_plmn_id(
                      ehplmn_list_ptr->plmn[index]);
    } 
    emm_modem_mem_free(ehplmn_list_ptr, emm_ctrl_data_ptr);
    ehplmn_list_ptr = NULL;
  }
#else
    /* HPLMN */
    rrc_service_req_ptr->hplmn_info_is_valid = hplmn_info_is_valid ;
    if(hplmn_info_is_valid == TRUE)
    {
      
#if defined FEATURE_DUAL_SIM  || defined FEATURE_SGLTE
      emm_ctrl_data_ptr->emm_home_plmn = reg_sim_per_subs_hplmn_get(mm_as_id);
#else
      emm_ctrl_data_ptr->emm_home_plmn = reg_sim_hplmn_get(); 
#endif
      rrc_service_req_ptr->hplmn = 
      emm_convert_nas_plmn_id_to_rrc_plmn_id(emm_ctrl_data_ptr->emm_home_plmn);   
    }
#endif
  }
  /* Forbidden TAI lists for roaming and manual selection */
  emm_set_rrc_forbidden_ta_list(&rrc_service_req_ptr->forbidden_ta_list,
                                &rrc_service_req_ptr->forbidden_manual_ta_list,
                                emm_ctrl_data_ptr);

  /* Report eplmn list to RRC */
  emm_set_rrc_eplmn_list(&rrc_service_req_ptr->eplmn_list, emm_ctrl_data_ptr);

  /* Tempo solution */
//  rrc_service_req.rat_pri_list.num_items = 0 ;
#ifdef FEATURE_CIOT

  rrc_service_req_ptr->scan_is_new = scan_is_new ;

  rrc_service_req_ptr->use_timer = use_timer ;

  /* valid only if use_timer is TRUE */
  if (rrc_service_req_ptr->use_timer)
  {
    rrc_service_req_ptr->lte_scan_time = lte_scan_time ;
  }

  rrc_service_req_ptr->new_scan = new_scan ;

  rrc_service_req_ptr->use_search_timer = apply_timer ;

  /* valid only if use_timer is TRUE */
  if (rrc_service_req_ptr->use_search_timer)
  {
    rrc_service_req_ptr->lte_search_timer = search_time ;
  } //FRACQ

  if(nb1_acq_tier3_timer_status == TRUE)
  {
    rrc_service_req_ptr->snr2_acq_db_timer_expired = FALSE;
  }
  else
  {
    rrc_service_req_ptr->snr2_acq_db_timer_expired = TRUE;
  }
  if(nb1_fullband_tier3_timer_status == TRUE)
  {
    rrc_service_req_ptr->snr2_band_scan_timer_expired = FALSE;
  }
  else
  {
    rrc_service_req_ptr->snr2_band_scan_timer_expired = TRUE;
  }
#endif
#if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW)
  rrc_service_req_ptr->csg_id = csg_id;
#endif 
#if defined FEATURE_DUAL_SIM || defined FEATURE_1XSRLTE
  rrc_service_req_ptr->t_timeout = trm_timeout;
#endif
  /* Since we started service_req, mark current rat to be LTE. This is needed to handle e.g STOP_MODE_REQ
     if it comes before service indication is rceived from RRC*/
#ifndef FEATURE_CIOT
  emm_ctrl_data_ptr->plmn_service_state->active_rat = SYS_RAT_LTE_RADIO_ACCESS;
#endif
#ifdef FEATURE_NAS_GW
  emm_ctrl_data_ptr->ignore_GW_activation_ind    = TRUE;
#endif
  /* send the translated reg req type to rrc*/
#ifdef FEATURE_LTE_TO_1X
#ifdef FEATURE_LTE_REL9
  if(emm_bit_mask_check(additional_info,(byte)SYS_ADDTNL_INFO_1XCSFB_CALL_END) == TRUE)
  {
    MSG_MED_DS_0(MM_SUB, "=EMM= populating LTE_RRC_SERVICE_REQ_TYPE_1xCSFB_END");
    rrc_service_req_ptr->req_type = LTE_RRC_SERVICE_REQ_TYPE_1xCSFB_END;
  }
  else
#endif
  if(reg_req_type == MMR_REG_REQ_BETTER_SYSTEM_RESELECTION)
  {
    MSG_MED_DS_0(MM_SUB, "=EMM= populating LTE_SERV_REQ_TYPE_BSR");
    rrc_service_req_ptr->req_type = LTE_RRC_SERVICE_REQ_TYPE_BSR;
  }
  else
#endif 
  {
    if (emm_ctrl_data_ptr->psm_restore_done == TRUE)
    {

      emm_ctrl_data_ptr->reg_lte_cs_domain_param = 
               emm_ctrl_data_ptr->reg_req_pending_ptr->lte_cs_domain_param;

      /* Populate service request type as PSM for all PSM exit with context restoration
         psm_restore_done flag will be reset upon getting rrc service indication
         as NAS needs to send drx/edrx params to RRC after receiving service indication */

      MSG_HIGH_DS_0(MM_SUB, "=EMM= populating LTE_SERV_REQ_TYPE_PSM");
      rrc_service_req_ptr->req_type = LTE_RRC_SERVICE_REQ_TYPE_PSM;
    }
#ifdef FEATURE_CIOT
    else if ((rrc_service_req_ptr->network_select_mode == SYS_NETWORK_SELECTION_MODE_LIMITED) &&
        (reg_req_type == MMR_REG_REQ_FULL_RAT_MODE) &&
        (rat_pri_list != NULL)&&(rat_pri_list->priority_list_info[0].acq_sys_mode == SYS_SYS_MODE_LTE_NB1)
        )
    {
        MSG_HIGH_DS_0(MM_SUB, "=EMM= populating LTE_SERV_REQ_TYPE_PLMN");
        rrc_service_req_ptr->req_type = LTE_RRC_SERVICE_REQ_TYPE_PLMN;
        rrc_service_req_ptr->network_select_mode = SYS_NETWORK_SELECTION_MODE_AUTOMATIC;

        /*fake auto params */
        {
        sys_plmn_id_s_type null_plmn = {{0xFF, 0xFF, 0xFF}};

        /* Requested PLMN */
        rrc_service_req_ptr->req_plmn_info_is_valid = TRUE;
        rrc_service_req_ptr->req_plmn = emm_convert_nas_plmn_id_to_rrc_plmn_id(null_plmn) ;
        }

    }
#endif
#ifdef FEATURE_FMD_SPEED_INFO
	else if((rrc_service_req_ptr->network_select_mode == SYS_NETWORK_SELECTION_MODE_LIMITED) &&
        (reg_req_type == MMR_REG_REQ_NORMAL) &&
		emm_bit_mask_check(emm_ctrl_data_ptr->reg_req_pending_ptr->additional_info,(byte)SYS_ADDTNL_INFO_PSEUDO_ONLINE_MODE) == TRUE
  && (rat_pri_list != NULL)&&(rat_pri_list->priority_list_info[0].acq_sys_mode == SYS_SYS_MODE_LTE_NB1)) 
	{
		rrc_service_req_ptr->pseudo_online_mode = TRUE;
		MSG_MED_DS_0(MM_SUB, "=EMM= populating  LTE_SERV_REQ_TYPE_NOR : Pseudo Online ");
        rrc_service_req_ptr->req_type = LTE_RRC_SERVICE_REQ_TYPE_NORMAL;
		rrc_service_req_ptr->network_select_mode = SYS_NETWORK_SELECTION_MODE_AUTOMATIC;

		 /*fake auto params */
        {
        sys_plmn_id_s_type null_plmn = {{0xFF, 0xFF, 0xFF}};

        /* Requested PLMN */
        rrc_service_req_ptr->req_plmn_info_is_valid = TRUE;
        rrc_service_req_ptr->req_plmn = emm_convert_nas_plmn_id_to_rrc_plmn_id(null_plmn) ;
        }
	}
#endif
    else
    {
      MSG_MED_DS_0(MM_SUB, "=EMM= populating  LTE_SERV_REQ_TYPE_NOR");
      rrc_service_req_ptr->req_type = LTE_RRC_SERVICE_REQ_TYPE_NORMAL;
    }
  }
       
  rrc_service_req_ptr->scan_scope = scan_scope;

#if defined(FEATURE_LTE_REL9) && !defined(FEATURE_NAS_IMS_VOICE_DISABLED)
#ifdef FEATURE_LTE_REL9
  if(emm_bit_mask_check(additional_info,(byte)SYS_ADDTNL_INFO_EMERG_CALL) == TRUE)
  {
    rrc_service_req_ptr->emc_srv_pending = TRUE;
  }
  else 
#endif
  {
    rrc_service_req_ptr->emc_srv_pending = FALSE;
  }
#endif
  if(mm_get_deep_sleep_alt_scan_status() == TRUE) 
  {
    emm_resume_paused_mm_timers();
    mm_set_deep_sleep_alt_scan_status(FALSE);
  }
  emm_set_tau_trm_priority(FALSE);

  if ((network_select_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC) &&
      (req_plmn != NULL) &&
      (!sys_plmn_id_is_undefined(*req_plmn)))
  {
    rrc_service_req_ptr->ehplmn_camping_allowed = reg_mode_get_hplmn_to_selected (
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
                                                      (sys_modem_as_id_e_type)mm_as_id
#endif
                                                    );

  }  
#ifdef FEATURE_CIOT
  /* Keep track of the currently loaded image in LTE RRC*/
  emm_ctrl_data_ptr->loaded_lte_rat = mm_convert_acq_mode_to_sys_acq(
      rrc_service_req_ptr->rat_pri_list.priority_list_info[0].acq_sys_mode);
  /*release the pdn if cached*/
  if(current_loaded_rat != SYS_RAT_NONE &&
  	 current_loaded_rat != emm_ctrl_data_ptr->loaded_lte_rat)
  {
      /* Report to ESM and invalidate pdn conn req*/
      emm_failure_cause.cause_type = LTE_NAS_IRAT_INVALID_STATE;
      emm_release_attach_pdn_conn_req(emm_ctrl_data_ptr, emm_failure_cause);
  }
#endif
  emm_msgr_send((msgr_hdr_s *)rrc_service_req_ptr, sizeof(lte_rrc_service_req_s));
  MSG_HIGH_DS_1(MM_SUB, "EMM: Sent LTE_RRC_SERVICE_REQ scan_scope %d", scan_scope);

  if (rrc_service_req_ptr->req_type == LTE_RRC_SERVICE_REQ_TYPE_PSM)
  {
    emm_send_rrc_edrx_update(emm_ctrl_data_ptr, emm_ctrl_data_ptr->edrx_params.enabled);
	if(mm_get_ulog_handle() != NULL)
    {
        ULOG_RT_PRINTF_2(mm_get_ulog_handle(),  "=EMM= Sent LTE_RRC_EDRX_CFG_REQ. edrx_params_valid = %d, edrx_params = 0x%x",
										  emm_ctrl_data_ptr->edrx_params.enabled,
										  (byte)((emm_ctrl_data_ptr->edrx_params.ptw << 4) | 
													  emm_ctrl_data_ptr->edrx_params.cycle_length));
    }

  }

  /* Free memory */
  emm_modem_mem_free(rrc_service_req_ptr, emm_ctrl_data_ptr);
  mm_read_camp_nv();
  mm_read_registration_nv();
} /* end of emm_send_rrc_service_req() */

/*===========================================================================

FUNCTION EMM_SEND_RRC_PLMN_SEARCH_REQ

DESCRIPTION
  This function builds and sends LTE_RRC_PLMN_SEARCH_REQ to RRC

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_send_rrc_plmn_search_req
(
  mmr_plmn_search_req_s_type  *plmn_search_req_ptr,
  emm_ctrl_data_type          *emm_ctrl_data_ptr,
  boolean                      hplmn_info_is_valid
)
{
  lte_rrc_plmn_search_req_s  *rrc_plmn_search_req_ptr;
  uint32                     index = 0;


  /* Allocate memory and initialize it */
  rrc_plmn_search_req_ptr = emm_modem_mem_alloc(sizeof(lte_rrc_plmn_search_req_s),
                                                emm_ctrl_data_ptr);
  mm_check_for_null_ptr((void*)rrc_plmn_search_req_ptr);

  /* Message router heade */
  msgr_init_hdr((msgr_hdr_s *)&rrc_plmn_search_req_ptr->msg_hdr, 
                MSGR_NAS_EMM, 
                LTE_RRC_PLMN_SEARCH_REQ);

  /* Transaction ID */
  rrc_plmn_search_req_ptr->trans_id = (uint32)plmn_search_req_ptr->transaction_id;

  /* NW select mode */
  rrc_plmn_search_req_ptr->network_select_mode = 
                                   plmn_search_req_ptr->network_selection_mode;

  /* Service search */
  rrc_plmn_search_req_ptr->search_type = plmn_search_req_ptr->service_search;

  /* RAT priority list */
  rrc_plmn_search_req_ptr->rat_pri_list = plmn_search_req_ptr->rat_pri_list_info;

  /* No. of cells to search in cell search request */
  rrc_plmn_search_req_ptr->cell_search_num_cells = plmn_search_req_ptr->cell_search_num_cells;
  
   /*mcc based filter*/ 
  if((plmn_search_req_ptr->network_selection_mode == SYS_NETWORK_SELECTION_MODE_MANUAL) &&
	 (plmn_search_req_ptr->service_search == SYS_SERVICE_SEARCH_NONE))
  {
	MSG_HIGH_DS_3(MM_SUB, "=MM= Manual PLMN Timer value FIRST %u  MORE %u  MAX %u",
                      mm_emm_manual_search_timer_value.FirstPLMNTimer,
                      mm_emm_manual_search_timer_value.MorePLMNTimer,
                      mm_emm_manual_search_timer_value.MaxManualPLMNTimer);
	rrc_plmn_search_req_ptr->search_timer_value = mm_emm_manual_search_timer_value;	
	MSG_HIGH_DS_1(MM_SUB, "=MM= BST changes length of mcc mode filter =%d",
                      plmn_search_req_ptr->mcc_mode_band_filter.num_items);
	rrc_plmn_search_req_ptr->mcc_mode_band_filter = plmn_search_req_ptr->mcc_mode_band_filter;	
  }


  /*scan scope*/

  rrc_plmn_search_req_ptr->scan_scope = plmn_search_req_ptr->scan_scope;    

  /* PLMN RAT List */
  rrc_plmn_search_req_ptr->plmn_rat_list = 
                                plmn_search_req_ptr->higher_priority_plmn_list;
#ifdef FEATURE_GPS_WWAN_PRIORITY_SEARCH									
 if(plmn_search_req_ptr->service_search == SYS_SERVICE_SEARCH_GPS_ADVANCE_SEARCH)
 {
  rrc_plmn_search_req_ptr->wwan_adv_search_req_param = plmn_search_req_ptr->gps_wwan_search_info;
 }
#endif 
  emm_set_rrc_rplmn(&rrc_plmn_search_req_ptr->rplmn_info_is_valid,
                    &rrc_plmn_search_req_ptr->rplmn,
                    emm_ctrl_data_ptr);

/* Populate EHPLMN list or HPLMN */
  rrc_plmn_search_req_ptr->hplmn_info_is_valid = FALSE;
  rrc_plmn_search_req_ptr->ehplmn_list.num_plmns = 0;

  if((emm_ctrl_data_ptr->plmn_service_state->sim_state == SYS_SIM_STATE_AVAILABLE) || 
     (emm_ctrl_data_ptr->plmn_service_state->sim_state == SYS_SIM_STATE_CS_INVALID))
  {
#ifdef FEATURE_EQUIVALENT_HPLMN
    reg_sim_plmn_list_s_type *ehplmn_list_ptr;
    ehplmn_list_ptr = (reg_sim_plmn_list_s_type *) emm_modem_mem_alloc(
                                              sizeof(reg_sim_plmn_list_s_type),
                                              emm_ctrl_data_ptr);
    mm_check_for_null_ptr((void *)ehplmn_list_ptr);
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
    reg_sim_per_nas_stacks_ehplmn_list_get_ptr(ehplmn_list_ptr, (sys_modem_as_id_e_type)mm_as_id);
#else
    reg_sim_ehplmn_list_get_ptr(ehplmn_list_ptr);
#endif
    if(ehplmn_list_ptr->length > SYS_EHPLMN_LIST_MAX_LENGTH)
    {
      MSG_HIGH_DS_1(MM_SUB, "=EMM= EHPLMN list too long %d - RRC PLMN SEARCH REQ",
                    ehplmn_list_ptr->length);
      rrc_plmn_search_req_ptr->ehplmn_list.num_plmns = SYS_EHPLMN_LIST_MAX_LENGTH;
    }
    else
    {
      rrc_plmn_search_req_ptr->ehplmn_list.num_plmns = 
                                       ehplmn_list_ptr->length;
    }
    for(index=0; index<rrc_plmn_search_req_ptr->ehplmn_list.num_plmns; index++)
    {
      rrc_plmn_search_req_ptr->ehplmn_list.plmn[index] = 
        emm_convert_nas_plmn_id_to_rrc_plmn_id(ehplmn_list_ptr->plmn[index]);
    } 
    emm_modem_mem_free(ehplmn_list_ptr, emm_ctrl_data_ptr);
    ehplmn_list_ptr = NULL;
#else
    /* HPLMN */
    rrc_plmn_search_req_ptr->hplmn_info_is_valid = hplmn_info_is_valid ;
    if(hplmn_info_is_valid == TRUE)
    {
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
       emm_ctrl_data_ptr->emm_home_plmn = reg_sim_per_subs_hplmn_get(mm_as_id);
#else
       emm_ctrl_data_ptr->emm_home_plmn = reg_sim_hplmn_get(); 
#endif

      rrc_plmn_search_req_ptr->hplmn = 
      emm_convert_nas_plmn_id_to_rrc_plmn_id(emm_ctrl_data_ptr->emm_home_plmn);   
    }
#endif
  }

  /* EPLMN list */
  emm_set_rrc_eplmn_list(&rrc_plmn_search_req_ptr->eplmn_list, emm_ctrl_data_ptr);

  /* Forbidden TA list */
  rrc_plmn_search_req_ptr->forbidden_ta_list.num_entries = 
                          emm_ctrl_data_ptr->rrc_forbidden_list_ptr->tai_lst_length;
  for(index=0;index<rrc_plmn_search_req_ptr->forbidden_ta_list.num_entries;index++)
  {
    rrc_plmn_search_req_ptr->forbidden_ta_list.tracking_area[index].plmn = 
                emm_convert_nas_plmn_id_to_rrc_plmn_id(
                  emm_ctrl_data_ptr->rrc_forbidden_list_ptr->diff_plmn[index].plmn);
    rrc_plmn_search_req_ptr->forbidden_ta_list.tracking_area[index].tac = 
                    emm_ctrl_data_ptr->rrc_forbidden_list_ptr->diff_plmn[index].tac;
  }
#ifdef FEATURE_NAS_GW
  if(plmn_search_req_ptr->network_selection_mode == SYS_NETWORK_SELECTION_MODE_MANUAL)
  {
    /*Block ACTIVATION_INDICATION from RRC*/
    emm_ctrl_data_ptr->ignore_GW_activation_ind = TRUE;
  }
#endif
  emm_msgr_send((msgr_hdr_s *)rrc_plmn_search_req_ptr, 
               sizeof(lte_rrc_plmn_search_req_s));
  if(plmn_search_req_ptr->network_selection_mode == SYS_NETWORK_SELECTION_MODE_MANUAL)
  {
    emm_ctrl_data_ptr->plmn_search_in_progress = TRUE;
  }
  mm_set_plmn_search_in_progress_flag(TRUE);

#ifdef FEATURE_CIOT
   /* Keep track of the currently loaded image in LTE RRC*/
   emm_ctrl_data_ptr->loaded_lte_rat = plmn_search_req_ptr->rat;
#endif

  MSG_HIGH_DS_1(MM_SUB, "=EMM= Sent LTE_RRC_PLMN_SEARCH_REQ scan scope %d",
                plmn_search_req_ptr->scan_scope);

  /* Free the memory block */
  emm_modem_mem_free(rrc_plmn_search_req_ptr, emm_ctrl_data_ptr);

} /* end of emm_send_rrc_plmn_search_req() */

/*===========================================================================

FUNCTION EMM_SEND_RRC_PLMN_SEARCH_ABORT_REQ

DESCRIPTION
  This function builds and sends LTE_RRC_PLMN_SEARCH_ABORT_REQ to RRC

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_send_rrc_plmn_search_abort_req
(
  void
)
{
  lte_rrc_plmn_search_abort_req_s  rrc_plmn_search_abort_req;

  /* Init */
  memset(&rrc_plmn_search_abort_req, 0x0, 
         sizeof(lte_rrc_plmn_search_abort_req_s));
  mm_set_plmn_search_in_progress_flag(FALSE);
  /* Message router heade */
  msgr_init_hdr((msgr_hdr_s *)&rrc_plmn_search_abort_req.msg_hdr, 
                MSGR_NAS_EMM, 
                LTE_RRC_PLMN_SEARCH_ABORT_REQ);

  emm_msgr_send((msgr_hdr_s *)&rrc_plmn_search_abort_req, 
               sizeof(lte_rrc_plmn_search_abort_req_s));
  MSG_HIGH_DS_0(MM_SUB, "=EMM= Sent LTE_RRC_PLMN_SEARCH_ABORT_REQ");
} /* end of emm_send_rrc_plmn_search_abort_req() */

/*===========================================================================
FUNCTION    EMM_CHECK_HIGH_PRIORITY

DESCRIPTION
  This function determinates whether a call is high priority access or not 
  according to Annex D of NAS Spec 

DEPENDENCIES
  None

RETURN VALUE
  TRUE: High priority
  FALSE: Not high priority

SIDE EFFECTS
  None
===========================================================================*/
boolean    emm_check_high_priority
(
  emm_ctrl_data_type               *emm_ctrl_data_ptr
)
{
  sys_plmn_id_s_type    selected_plmn;
  uint32                home_mcc = 0, selected_mcc = 0;

  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ; 
  if(emm_ctrl_data_ptr->last_rrc_service_ind_ptr == NULL)
  {
    MSG_ERROR_DS_0(MM_SUB, "=EMM= Wrong sub this should not happen, return from function emm_check_high_priority");
    return FALSE;
  }

  MSG_HIGH_DS_1(MM_SUB, "=EMM= Checking for high priority - AC 0x%x",
                emm_ctrl_data_ptr->access_class);

#if defined FEATURE_DUAL_SIM|| defined FEATURE_SGLTE
  emm_ctrl_data_ptr->emm_home_plmn = reg_sim_per_subs_hplmn_get((sys_modem_as_id_e_type)mm_as_id);
#else
  emm_ctrl_data_ptr->emm_home_plmn = reg_sim_hplmn_get(); 
#endif

  selected_plmn = emm_convert_rrc_plmn_id_to_nas_plmn_id(
        emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.selected_plmn);

  /* Access class 12, 13 or 14 */
  if(((emm_ctrl_data_ptr->access_class & EMM_ACCESS_CLASS_12) != 0) ||
     ((emm_ctrl_data_ptr->access_class & EMM_ACCESS_CLASS_13) != 0) ||
     ((emm_ctrl_data_ptr->access_class & EMM_ACCESS_CLASS_14) != 0))
  {
    sys_plmn_get_mcc(emm_ctrl_data_ptr->emm_home_plmn, &home_mcc);
    sys_plmn_get_mcc(selected_plmn, &selected_mcc);

    /* Check if UE is in home country */
    if(sys_mcc_same_country(home_mcc, selected_mcc))
    {
      MSG_HIGH_DS_0(MM_SUB, "=EMM= AC 12/13/14 Home country - Set priority high");
      return TRUE;
    }
  }

  /* Access class 11 or 15 */
  if(((emm_ctrl_data_ptr->access_class & EMM_ACCESS_CLASS_11) != 0) ||
     ((emm_ctrl_data_ptr->access_class & EMM_ACCESS_CLASS_15) != 0))
  {
#ifdef FEATURE_EQUIVALENT_HPLMN
    /* Check if serving PLMN is EHPLMN */
    if(reg_sim_is_ehplmn(selected_plmn) == TRUE)
    {  
      MSG_HIGH_DS_0(MM_SUB, "=EMM= AC 11/15 EHPLMN - Set priority high");
      return TRUE;
    }
#endif

    /* Check if serving PLMN is HPLMN */
    if(sys_plmn_match(emm_ctrl_data_ptr->emm_home_plmn, selected_plmn) == TRUE)
    {
      MSG_HIGH_DS_0(MM_SUB, "=EMM= AC 11/15 HPLMN - Set priority high");
      return TRUE;
    }
  }

  return FALSE;

} /* end of emm_check_high_priority() */

/*=============================================================================

FUNCTION  EMM_SET_RRC_CONN_EST_GUMMEI

DESCRIPTION
  This function sets GUMMEI info in RRC CONN EST using info from given GUTI

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void  emm_set_rrc_conn_est_gummei
(
  lte_rrc_conn_est_req_s  *rrc_conn_est_req_ptr,
  lte_nas_emm_guti_type   *guti_ptr,
  lte_rrc_gummei_type_s    gummei_type 
)
{
  rrc_conn_est_req_ptr->registered_mme_is_present = TRUE;

  /* GUMMEI = MCC + MNC + MMEGI + MMEC */
  rrc_conn_est_req_ptr->registered_mme.plmn_identity_is_present = TRUE ;
  rrc_conn_est_req_ptr->registered_mme.plmn_identity = 
                        emm_convert_nas_plmn_id_to_rrc_plmn_id(guti_ptr->plmn);

  memscpy((void*)&rrc_conn_est_req_ptr->registered_mme.mmegi,
          sizeof(rrc_conn_est_req_ptr->registered_mme.mmegi),
         (void *)guti_ptr->mme_group_id,
          sizeof(guti_ptr->mme_group_id));

  rrc_conn_est_req_ptr->registered_mme.mmec = (uint8)guti_ptr->mme_code;

  rrc_conn_est_req_ptr->registered_mme.gummei_type = gummei_type;

} /* emm_set_rrc_conn_est_gummei */

/*===========================================================================
FUNCTION    EMM_SEND_RRC_CONNECTION_ESTABLISHMENT_REQUEST

DESCRIPTION
  This function sends RRC_CONN_EST_REQ to RRC to setup the connection

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_send_rrc_connection_establishment_request
(
  lte_rrc_est_cause_e              lte_rrc_est_cause,
  lte_nas_outgoing_msg_type       *emm_message_ptr,
  dword                            sequence_number,
  lte_nas_conn_req_routing_info    routing_info,
  sys_lte_reestab_calltype_e       actual_calltype,
  emm_ctrl_data_type              *emm_ctrl_data_ptr
)
{
  lte_rrc_conn_est_req_s         rrc_conn_est_req ;
  lte_nas_ul_security_info_type  ul_nas_security_info;
  size_t                         msg_len = EMM_MAX_MESSAGE_LENGTH;
  dsm_item_type                 *dsm_ptr   = NULL;
  msgr_attach_struct_type       *att_ptr;   /* Message attachment pointer */

  boolean                               ac_barring_info_valid = FALSE; 
  lte_rrc_ac_barring_info_for_special_s ac_barring_info;

  uint8 access_barring_prob[] = {40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55};
  emm_security_context_info  *context_ptr = NULL;
#ifdef FEATURE_NAS_GW
  boolean                      gsm_spt  = FALSE;
  boolean                      umts_spt = FALSE;
  boolean                      tds_spt  = FALSE;
#endif
  uint16 dsm_packet_length = 0;
  uint16 len=0;
  mm_check_for_null_ptr((void *)emm_ctrl_data_ptr->emm_ota_message_ptr);

  MSG_HIGH_DS_0(MM_SUB, "=EMM= Establishing RRC connection, set nas_secure_exchange to FALSE");
  emm_ctrl_data_ptr->nas_secure_exchange = FALSE;
  
  /* Init structures */
  memset((void *)&rrc_conn_est_req, 0x0, sizeof(lte_rrc_conn_est_req_s));
  memset((void *)&ul_nas_security_info, 0x0, 
         sizeof(lte_nas_ul_security_info_type));
  memset((void *)&ac_barring_info, 0x0, sizeof(lte_rrc_ac_barring_info_for_special_s));

  msgr_init_hdr_attach((msgr_hdr_s *)&rrc_conn_est_req.msg_hdr, 
                        MSGR_NAS_EMM, LTE_RRC_CONN_EST_REQ, 0, 1) ;

  rrc_conn_est_req.est_cause = lte_rrc_est_cause ;
  rrc_conn_est_req.transaction_id = sequence_number ;
  rrc_conn_est_req.is_high_priority = emm_check_high_priority(emm_ctrl_data_ptr);

#ifdef FEATURE_NAS_EAB
  if(emm_ctrl_data_ptr->eab_barring_flag == TRUE )
  {
    rrc_conn_est_req.eab_cat_c = emm_eab_cat_c_chk(emm_ctrl_data_ptr);
  }
#endif

  /*Initialize the Access Barring Prob list */
  memscpy((void*)&ac_barring_info.access_barring_prob[0], 
          sizeof(ac_barring_info.access_barring_prob),
          (void*)&access_barring_prob[0], 
          sizeof(access_barring_prob));

  ac_barring_info.access_barring_time = 20; /* Timer value = 20 msec */

  rrc_conn_est_req.ac_barring_info_for_special_valid = ac_barring_info_valid;
  rrc_conn_est_req.ac_barring_info_for_special = ac_barring_info;
  rrc_conn_est_req.est_calltype = emm_ctrl_data_ptr->call_type;
  rrc_conn_est_req.actual_calltype = actual_calltype;
  rrc_conn_est_req.barring_restr_for_special_ac_enabled = FALSE;
  
 #ifdef FEATURE_LTE_REL10
  rrc_conn_est_req.mo_data_enabled_dt = emm_ctrl_data_ptr->mo_data_enabled_dt;
 #endif

#ifdef FEATURE_NAS_GW
  gsm_spt = mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, 
                                                  SYS_RAT_GSM_RADIO_ACCESS);
  umts_spt = mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, 
                                                  SYS_RAT_UMTS_RADIO_ACCESS);
  tds_spt = mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, 
                                                   SYS_RAT_TDS_RADIO_ACCESS);
#endif
  switch(routing_info)
  {
    case ROUTING_INFO_STMSI:
      rrc_conn_est_req.include_s_tmsi = TRUE;
      break;
    case ROUTING_INFO_GUMMEI:
#ifdef FEATURE_NAS_GW
      MSG_MED_DS_1(MM_SUB, "=EMM= Setting GUMMEI in RRC CONN EST REQ w/ TIN type - %u",
                   emm_ctrl_data_ptr->TIN);
      if(emm_ctrl_data_ptr->TIN == NAS_MM_TIN_P_TMSI &&
                                  ((gsm_spt == TRUE) || (umts_spt == TRUE) ||(tds_spt == TRUE)))
      {
        if((mm_is_ptmsi_valid() == TRUE) && (mm_is_rai_valid() == TRUE))
        {
          lte_nas_emm_guti_type  mapped_guti;
          memset((void *)&mapped_guti, 0x0, sizeof(lte_nas_emm_guti_type));

          /* Map P-TMSI and RAI to GUMMEI info in GUTI */
          mm_map_ptmsi_rai_to_gumme_info(&mapped_guti);

          /* Set GUMMEI info in rrc conn est req */
          emm_set_rrc_conn_est_gummei(&rrc_conn_est_req, 
                                      &mapped_guti, 
                                      LTE_RRC_GUMMEI_TYPE_MAPPED);
        }
        else
        {
          MSG_HIGH_DS_0(MM_SUB, "=EMM= GUMMI is not available due to no valid PTMSI or RAI");
        }
      }
      /* GUTI or RAT related TMSI or None */
      else
#endif
      {
        if(emm_ctrl_data_ptr->emm_guti_valid == TRUE)
        {
          /* Set GUMMEI info in rrc conn est req */
          emm_set_rrc_conn_est_gummei(&rrc_conn_est_req, 
                                      &emm_ctrl_data_ptr->emm_guti, 
                                      LTE_RRC_GUMMEI_TYPE_NATIVE);
        }
        else
        {
          MSG_HIGH_DS_0(MM_SUB, "=EMM= GUMMI is not available due to no valid GUTI");
        }
      }
      break;
    default:
      /* Use the initialized values. No need to do any thing here. */
      break;
  }

#ifdef FEATURE_CIOT
  if(emm_bit_mask_check(emm_ctrl_data_ptr->nas_ciot_capability_config, EMM_ER_WITHOUT_PDN))
  {
      lte_nas_esm_outgoing_msg_type *esm_outgoing_msg = {NULL};  
      esm_outgoing_msg = get_esm_container(emm_message_ptr);     
      if (esm_outgoing_msg != NULL && 
      	  esm_outgoing_msg->nas_hdr.pd == EPS_SESSION_MANAGEMENT_MESSAGES && 
	  	  esm_outgoing_msg->nas_hdr.msg_id == PDN_CONNECTIVITY_REQUEST &&
	  	  esm_outgoing_msg->pdn_connectivity_req.esm_dummy_flag == TRUE)
      {
        rrc_conn_est_req.ue_ciot_opts.attach_without_pdn = TRUE;
      }
	  else if((emm_ctrl_data_ptr->attach_without_pdn == TRUE) &&
	  	      (emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.attachWithoutPDNConnectivity == TRUE))
	  {
	    rrc_conn_est_req.ue_ciot_opts.attach_without_pdn = TRUE;
      }
      else
      {
	    rrc_conn_est_req.ue_ciot_opts.attach_without_pdn = FALSE;
      }
  }
  else
  {
    rrc_conn_est_req.ue_ciot_opts.attach_without_pdn = FALSE;
  }
  if(emm_ctrl_data_ptr->plmn_service_state->active_rat == SYS_RAT_LTE_NB1_RADIO_ACCESS)
  {
     rrc_conn_est_req.ue_ciot_opts.cp_ciot_eps_optimization = TRUE;
  }
  else
  {
    rrc_conn_est_req.ue_ciot_opts.cp_ciot_eps_optimization = emm_bit_mask_check(emm_ctrl_data_ptr->nas_ciot_capability_config, EMM_CP_CIOT);
  }
  rrc_conn_est_req.ue_ciot_opts.s1u_supported = emm_bit_mask_check(emm_ctrl_data_ptr->nas_ciot_capability_config, EMM_S1_U);
  rrc_conn_est_req.ue_ciot_opts.up_ciot_eps_optimization = emm_bit_mask_check(emm_ctrl_data_ptr->nas_ciot_capability_config, EMM_UP_CIOT)
  	                                                       &emm_ctrl_data_ptr->plmn_service_state->eps_nw_feature_support.up_ciot_supported;
#endif

#if defined  FEATURE_DUAL_SIM || defined FEATURE_1XSRLTE
  rrc_conn_est_req.trm_priority = emm_get_lte_trm_priority(emm_ctrl_data_ptr);
  emm_ctrl_data_ptr->lte_trm_priority = rrc_conn_est_req.trm_priority;
#endif 

#ifdef FEATURE_CIOT
  if ( emm_message_ptr->nas_hdr.msg_id == CP_SERVICE_REQUEST)
  {
    /*check if memory for ota_msg_ptr needs to be increased */
    msg_len = emm_resize_mem_for_ota_msg_ptr(emm_ctrl_data_ptr,emm_message_ptr->nas_hdr.msg_id);
  }
  if (emm_up_opt_active(emm_ctrl_data_ptr) == TRUE && emm_ctrl_data_ptr->is_conn_suspended == TRUE)
  {
	rrc_conn_est_req.resume_req = TRUE;
	rrc_conn_est_req.send_on_resume = emm_ctrl_data_ptr->send_on_resume;
  }
  rrc_conn_est_req.include_dcn_id = FALSE; 
  if ( emm_message_ptr->nas_hdr.msg_id == ATTACH_REQUEST || 
  	   (emm_message_ptr->nas_hdr.msg_id == TRACKING_AREA_UPADTE_REQUEST &&
  	   (emm_ctrl_data_ptr->tau_cause == TAU_CAUSE_TA_CHANGE)))
  {
    int8 i = -1, index = -1;
    i = emm_search_dcn_list(emm_ctrl_data_ptr,emm_ctrl_data_ptr->plmn_service_state->plmn,&index);
    if(i >= 0 && i < LTE_NAS_MAX_DCN_IDS)
    {
      rrc_conn_est_req.include_dcn_id = TRUE;
	  rrc_conn_est_req.dcn_id = emm_ctrl_data_ptr->dcn_list.dcn_list[i].dcn_id;
    }
	else if(reg_sim_check_dcn_cfg() == TRUE)
	{
	  rrc_conn_est_req.include_dcn_id = TRUE;
	  rrc_conn_est_req.dcn_id = reg_sim_get_dcn_id();
	}
	MSG_HIGH_DS_5(MM_SUB, "=EMM= DCN ID  i = %d valid %d DCN id %d reg_sim_check_dcn_cfg() = %d reg_sim_get_dcn_id() = %d",i, rrc_conn_est_req.include_dcn_id,rrc_conn_est_req.dcn_id,reg_sim_check_dcn_cfg(),reg_sim_get_dcn_id());
  }
#endif

  /*reset the memory*/
  memset((void *)emm_ctrl_data_ptr->emm_ota_message_ptr,0x0,
                                 sizeof(byte)*msg_len); 

  emm_ctrl_data_ptr->ota_msg_size = encode_nas_msg_fp(emm_message_ptr, sizeof(lte_nas_outgoing_msg_type),
                                                      emm_ctrl_data_ptr->emm_ota_message_ptr,
                                                      msg_len,
                                                      emm_get_ul_security_info(emm_ctrl_data_ptr,
                                                      &ul_nas_security_info));

  if(emm_ctrl_data_ptr->ota_msg_size == 0)
  {
    MSG_FATAL_DS(MM_SUB,"=EMM= Failed to encode EMM OTA message",0,0,0) ;
  }

  MSG_HIGH_DS_1(MM_SUB, "=EMM= DSM conn_est_req item len %d", emm_ctrl_data_ptr->ota_msg_size);

  /*if we do not have enough small items use large item pool*/
  if(((uint16)emm_ctrl_data_ptr->ota_msg_size <= DSM_DS_SMALL_ITEM_SIZ) && 
     ((uint16)emm_ctrl_data_ptr->ota_msg_size <= NAS_REM_SMALL_ITEM_POOL_SIZE))
  {
    MSG_HIGH_DS_1(MM_SUB, "=EMM= dsm push len %d small item pool", (uint16)emm_ctrl_data_ptr->ota_msg_size);
    /* Pushdown DSM */
    dsm_packet_length = dsm_pushdown(&dsm_ptr, 
                                     emm_ctrl_data_ptr->emm_ota_message_ptr,
                                     (uint16)emm_ctrl_data_ptr->ota_msg_size, 
                                     DSM_DS_SMALL_ITEM_POOL);

    if((uint16)emm_ctrl_data_ptr->ota_msg_size != dsm_packet_length)
    {
      /*free any partial memory allocated */
      dsm_free_packet(&dsm_ptr);

      MSG_HIGH_DS_1(MM_SUB, "=EMM= dsm push len %d large item pool", dsm_packet_length);
      /* If Small item pool is running out always try Large item pool */
      len = dsm_pushdown(&dsm_ptr,emm_ctrl_data_ptr->emm_ota_message_ptr,(uint16)emm_ctrl_data_ptr->ota_msg_size, DSM_DS_LARGE_ITEM_POOL);
      if(len != (uint16)emm_ctrl_data_ptr->ota_msg_size) 
      {
        MSG_FATAL_DS(MM_SUB,"=EMM= Failed to push len %d large item pool",dsm_packet_length,0,0);
      }
    }
  }
  else
  {
    MSG_HIGH_DS_1(MM_SUB, "=EMM= dsm push len %d large item pool", dsm_packet_length);

    /* If Small item pool is running out always use Large item pool */
    dsm_packet_length = dsm_pushdown(&dsm_ptr, 
                                     emm_ctrl_data_ptr->emm_ota_message_ptr,
                                     (uint16)emm_ctrl_data_ptr->ota_msg_size, 
                                     DSM_DS_LARGE_ITEM_POOL);

    if((uint16)emm_ctrl_data_ptr->ota_msg_size != dsm_packet_length)
    {
      /*free any partial memory allocated */
      dsm_free_packet(&dsm_ptr);

      MSG_HIGH_DS_1(MM_SUB, "=EMM= dsm push len %d small item pool", (uint16)emm_ctrl_data_ptr->ota_msg_size);

      /* If Small item pool is running out always try Large item pool */
      len = dsm_pushdown(&dsm_ptr,emm_ctrl_data_ptr->emm_ota_message_ptr,(uint16)emm_ctrl_data_ptr->ota_msg_size, DSM_DS_SMALL_ITEM_POOL);
      if(len != (uint16)emm_ctrl_data_ptr->ota_msg_size) 
      {
        MSG_FATAL_DS(MM_SUB,"=EMM= Failed to push len %d small item pool",(uint16)emm_ctrl_data_ptr->ota_msg_size,0,0);
      }
    }
  }		    
  
  /* Set DSM attach */
  att_ptr = msgr_get_attach(&rrc_conn_est_req.msg_hdr, 0);
  msgr_set_dsm_attach(att_ptr, dsm_ptr);

  emm_set_mm_msg_info_for_debug_buffer((byte)emm_message_ptr->nas_hdr.msg_id);


  emm_msgr_send((msgr_hdr_s *)&rrc_conn_est_req, sizeof(lte_rrc_conn_est_req_s));
  MSG_HIGH_DS_6(MM_SUB, "=EMM= Sent LTE_RRC_CONN_EST_REQ w/ cause %d ,EAB CATC val = %d RESUME(0/1) = %d SEND on RESUME(0/1) = %d, DCN VALID =  %d DCN ID = %d",
					rrc_conn_est_req.est_cause,rrc_conn_est_req.eab_cat_c,rrc_conn_est_req.resume_req,rrc_conn_est_req.send_on_resume,rrc_conn_est_req.include_dcn_id,rrc_conn_est_req.dcn_id);

  
  context_ptr = emm_get_context_by_state(emm_ctrl_data_ptr,
                                             CONTEXT_STATE_CURRENT);
  
  if( context_ptr != NULL )
  { 
    if(emm_message_ptr->emm_outgoing_msg.service_req.security_hdr == SERV_REQ)
    {
      MSG_HIGH_DS_0(MM_SUB, "=EMM= Send SIM_UPDATE_REQ to RRC ");
      emm_send_sim_update(emm_ctrl_data_ptr,TRUE);
    } /* SERV_REQ */
    else
    {  
      switch (emm_message_ptr->nas_hdr.msg_id)
      {
        case ATTACH_REQUEST:
        case EXT_SERVICE_REQUEST:
        case TRACKING_AREA_UPADTE_REQUEST:
		case CP_SERVICE_REQUEST:
          MSG_HIGH_DS_0(MM_SUB, "=EMM= Send SIM_UPDATE_REQ to RRC ");
          emm_send_sim_update(emm_ctrl_data_ptr,TRUE);
        break;
        default:
          MSG_MED_DS_1(MM_SUB, "=EMM= Default, Do not SIM_UPDATE_REQ to RRC for msg_id = %d ",
                       emm_message_ptr->nas_hdr.msg_id);
        break;
      }/* end switch */
    } /* else */
  }
  else  /* No Current context */
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= Security not valid, Do NOT Send SIM_UPDATE_REQ to RRC ");
  }

  /*if message was re-allocated */
  if(emm_ctrl_data_ptr->emm_ota_message_ptr_flipped == TRUE)
  {
    emm_realloc_mem_for_ota_msg_ptr(emm_ctrl_data_ptr);
    emm_ctrl_data_ptr->emm_ota_message_ptr_flipped = FALSE;
  }

  emm_ctrl_data_ptr->psm_active = FALSE;
} /* end of emm_send_rrc_connection_establishment_request() */

void emm_send_rrc_data_request
(
  lte_nas_outgoing_msg_type   *emm_message_ptr,
  dword                        sequence_number,
  emm_ctrl_data_type          *emm_ctrl_data_ptr
)
{
  lte_rrc_ul_data_req_s         rrc_ul_data_req ;
  lte_nas_ul_security_info_type ul_security_info;
  dsm_item_type                *dsm_ptr   = NULL;
  msgr_attach_struct_type      *att_ptr;   /* Message attachment pointer */
  emm_security_context_info    *current_ctxt = NULL;
  size_t                        msg_len = EMM_MAX_MESSAGE_LENGTH;
#if defined(FEATURE_LTE_REL9) && !defined(FEATURE_NAS_IMS_VOICE_DISABLED)
#ifdef FEATURE_NAS_E911_ENABLE
  byte                          nas_invalid_kasme[32];
#endif
#endif
  uint16 dsm_packet_length = 0;
  uint16 len=0;

  mm_check_for_null_ptr((void *)emm_ctrl_data_ptr->emm_ota_message_ptr); 

  msgr_init_hdr_attach(&rrc_ul_data_req.msg_hdr, MSGR_NAS_EMM, 
                                          LTE_RRC_UL_DATA_REQ, 0, 1);

  memset(&ul_security_info,0,sizeof(lte_nas_ul_security_info_type));

  memset((void *)emm_ctrl_data_ptr->emm_ota_message_ptr, 0, EMM_MAX_MESSAGE_LENGTH);
 
  rrc_ul_data_req.trans_id = sequence_number ;
  rrc_ul_data_req.priority = LTE_RRC_UL_NAS_MSG_PRIORITY_HIGH ;
  rrc_ul_data_req.cnf_is_reqd = TRUE ;

  switch(emm_message_ptr->nas_hdr.msg_id)
  {
    case EXT_SERVICE_REQUEST:
      rrc_ul_data_req.cnf_is_reqd_during_conn_rel = TRUE;
    break;

    default:
      rrc_ul_data_req.cnf_is_reqd_during_conn_rel = FALSE;
    break;      
  }

  switch(emm_message_ptr->nas_hdr.msg_id)
  {
    case ESM_DATA_TRANSPORT:
      rrc_ul_data_req.esm_data_transport = TRUE;
    break;

    default:
      rrc_ul_data_req.esm_data_transport = FALSE;
    break;      
  }

  if( emm_message_ptr->nas_hdr.msg_id == UL_GENERIC_NAS_TRANSPORT )
  {
    /*check if memory for ota_msg_ptr needs to be increased */
    msg_len = emm_resize_mem_for_ota_msg_ptr(emm_ctrl_data_ptr,emm_message_ptr->nas_hdr.msg_id);
  }
  else 
  {
    /*check if memory for ota_msg_ptr needs to be increased */
    msg_len = emm_resize_mem_for_ota_msg_ptr(emm_ctrl_data_ptr,emm_message_ptr->esm_outgoing_msg.nas_hdr.msg_id);
  }

  /*reset the memory*/
  memset((void *)emm_ctrl_data_ptr->emm_ota_message_ptr,0x0,
                                 sizeof(byte)*msg_len); 

  emm_ctrl_data_ptr->ota_msg_size = 
                               encode_nas_msg_fp(emm_message_ptr, sizeof(lte_nas_outgoing_msg_type), 
                                                 emm_ctrl_data_ptr->emm_ota_message_ptr,
                                                 msg_len,
                                                 emm_get_ul_security_info(emm_ctrl_data_ptr, &ul_security_info));
  if (emm_ctrl_data_ptr->ota_msg_size == 0)
  {
    MSG_FATAL_DS(MM_SUB,"=EMM= Failed to encode EMM OTA message",0,0,0) ;
  }

                            
   /*push down into appropriate pool*/                          
   /*if we do not have enough small items use large item pool*/
   if(((uint16)emm_ctrl_data_ptr->ota_msg_size <= DSM_DS_SMALL_ITEM_SIZ) && 
     ((uint16)emm_ctrl_data_ptr->ota_msg_size <= NAS_REM_SMALL_ITEM_POOL_SIZE))
  {
    MSG_HIGH_DS_1(MM_SUB, "=EMM= dsm push len %d small item pool", (uint16)emm_ctrl_data_ptr->ota_msg_size);
    /* Pushdown DSM */
    dsm_packet_length = dsm_pushdown(&dsm_ptr, 
                                     emm_ctrl_data_ptr->emm_ota_message_ptr,
                                     (uint16)emm_ctrl_data_ptr->ota_msg_size, 
                                     DSM_DS_SMALL_ITEM_POOL);

    if((uint16)emm_ctrl_data_ptr->ota_msg_size != dsm_packet_length)
    {
      /*free any partial memory allocated */
      dsm_free_packet(&dsm_ptr);

      MSG_HIGH_DS_1(MM_SUB, "=EMM= dsm push len %d large item pool", dsm_packet_length);
      /* If Small item pool is running out always try Large item pool */
      len = dsm_pushdown(&dsm_ptr,emm_ctrl_data_ptr->emm_ota_message_ptr,(uint16)emm_ctrl_data_ptr->ota_msg_size, DSM_DS_LARGE_ITEM_POOL);
      if(len != (uint16)emm_ctrl_data_ptr->ota_msg_size) 
      {
        MSG_FATAL_DS(MM_SUB,"=EMM= Failed to push len %d large item pool",dsm_packet_length,0,0);
      }
    }
  }
  else
  {
    MSG_HIGH_DS_1(MM_SUB, "=EMM= dsm push len %d large item pool", dsm_packet_length);

    /* If Small item pool is running out always use Large item pool */
    dsm_packet_length = dsm_pushdown(&dsm_ptr, 
                                     emm_ctrl_data_ptr->emm_ota_message_ptr,
                                     (uint16)emm_ctrl_data_ptr->ota_msg_size, 
                                     DSM_DS_LARGE_ITEM_POOL);

    if((uint16)emm_ctrl_data_ptr->ota_msg_size != dsm_packet_length)
    {
      /*free any partial memory allocated */
      dsm_free_packet(&dsm_ptr);

      MSG_HIGH_DS_1(MM_SUB, "=EMM= dsm push len %d small item pool", (uint16)emm_ctrl_data_ptr->ota_msg_size);

      /* If Small item pool is running out always try Large item pool */
      len = dsm_pushdown(&dsm_ptr,emm_ctrl_data_ptr->emm_ota_message_ptr,(uint16)emm_ctrl_data_ptr->ota_msg_size, DSM_DS_SMALL_ITEM_POOL);
      if(len != (uint16)emm_ctrl_data_ptr->ota_msg_size) 
      {
        MSG_FATAL_DS(MM_SUB,"=EMM= Failed to push len %d small item pool",(uint16)emm_ctrl_data_ptr->ota_msg_size,0,0);
      }
    }
  }
   
	
   /* Set DSM attach */
   att_ptr = msgr_get_attach(&rrc_ul_data_req.msg_hdr, 0);
   mm_check_for_null_ptr((void *)att_ptr);
   msgr_set_dsm_attach(att_ptr, dsm_ptr);
   mm_check_for_null_ptr((void *)dsm_ptr);


   /*if message was re-allocated */
  if(emm_ctrl_data_ptr->emm_ota_message_ptr_flipped == TRUE)
  {
    emm_realloc_mem_for_ota_msg_ptr(emm_ctrl_data_ptr);
    emm_ctrl_data_ptr->emm_ota_message_ptr_flipped = FALSE;
  }


#if defined FEATURE_DUAL_SIM || defined FEATURE_1XSRLTE 
  emm_update_lte_trm_priorty(emm_ctrl_data_ptr);
#endif 
  emm_set_mm_msg_info_for_debug_buffer((byte)emm_message_ptr->nas_hdr.msg_id);

  emm_msgr_send((msgr_hdr_s *)&rrc_ul_data_req, sizeof(lte_rrc_ul_data_req_s));
  MSG_HIGH_DS_1(MM_SUB, "=EMM= Sent LTE_RRC_UL_DATA_REQ rrc_ul_data_req.esm_data_transport %d",rrc_ul_data_req.esm_data_transport);

  current_ctxt = emm_get_context_by_state(emm_ctrl_data_ptr,
                                             CONTEXT_STATE_CURRENT);

  if( current_ctxt != NULL )
  {
    switch (emm_message_ptr->nas_hdr.msg_id)
    {
      case ATTACH_REQUEST:
      case TRACKING_AREA_UPADTE_REQUEST:
        MSG_HIGH_DS_0(MM_SUB, "=EMM= Send SIM_UPDATE_REQ to RRC ");
        emm_send_sim_update(emm_ctrl_data_ptr,TRUE);
      break;
	  case CP_SERVICE_REQUEST:
        MSG_HIGH_DS_1(MM_SUB,"=EMM= Debug: emm_ctrl_data_ptr->reset_nas_ul_cnt_rel15 %d",emm_ctrl_data_ptr->reset_nas_ul_cnt_rel15);
		if(emm_ctrl_data_ptr->reset_nas_ul_cnt_rel15 == TRUE)
		{
		  emm_send_sim_update(emm_ctrl_data_ptr,FALSE);
		}
		else				
        {		  
		  emm_send_sim_update(emm_ctrl_data_ptr,TRUE);
		}
	    
	  break;
      case SECURITY_MODE_COMPLETE:
#if defined(FEATURE_LTE_REL9) && !defined(FEATURE_NAS_IMS_VOICE_DISABLED)
#ifdef FEATURE_NAS_E911_ENABLE

  
          MSG_HIGH_DS_1(MM_SUB, "=EMM= current_ctxt exists, type %d ",
                        current_ctxt->context_type);
          memset(nas_invalid_kasme, 0xFF, sizeof(nas_invalid_kasme));
  
          if ((emm_ctrl_data_ptr->emc_srv_status != LTE_NAS_EMC_SRV_NONE) && 
              (memcmp(current_ctxt->nas_kasme,nas_invalid_kasme, sizeof(nas_invalid_kasme)) == 0))
          {
            MSG_HIGH_DS_0(MM_SUB, "=EMM= Rel 9: emergency services: KASME is 0xFF, send SIM_UPDATE_REQ to RRC (invalid KASME) ");
            emm_ctrl_data_ptr->reset_nas_context = TRUE;
            emm_send_sim_update(emm_ctrl_data_ptr,TRUE);
          }
          else
#else
          {
            MSG_HIGH_DS_0(MM_SUB, "=EMM= Rel 9: emergency services: KASME is Valid, send SIM_UPDATE_REQ to RRC ");
            emm_send_sim_update(emm_ctrl_data_ptr,TRUE);
          }
#endif
#else
          MSG_HIGH_DS_0(MM_SUB, "=EMM= Send SIM_UPDATE_REQ to RRC ");
          emm_send_sim_update(emm_ctrl_data_ptr,TRUE); 
#endif
        
      break;
      default:
        MSG_MED_DS_1(MM_SUB, "=EMM= Default, Do not send SIM_UPDATE_REQ to RRC for msg_id = %d",
                     emm_message_ptr->nas_hdr.msg_id);
      break;
    }/* end switch */
  } 
  else /* No current context */
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= NO current_ctxt exists");
  }
} /* end of emm_send_rrc_data_request() */

/*===========================================================================

FUNCTION    EMM_RRC_ABORT_CONNECTION

DESCRIPTION
  This function sends abort signaling connection request to RRC

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_rrc_abort_connection
(
  emm_ctrl_data_type *emm_ctrl_data_ptr,
  boolean            cell_barring_reqd,
  lte_rrc_conn_abort_cause_e abort_cause
)
{ 
  unsigned long long curr_time = 0;
  lte_rrc_conn_abort_req_s conn_abort_req ;
  emm_connection_state_type emm_connection_state = EMM_IDLE_STATE ;

   

  emm_connection_state = EMM_GET_CONNECTION_STATE() ;

  if (((emm_connection_state == EMM_WAITING_FOR_RRC_CONFIRMATION_STATE) || 
       (emm_connection_state == EMM_CONNECTED_STATE)) &&
      (emm_ctrl_data_ptr->aborting_conn == FALSE))
  {
    /* marks all existing release clients to be processed upon receiving 
       release indication with cause of aborted */
    emm_conn_abort_mark_release_client(emm_ctrl_data_ptr);
    /* marks all existing pended messages to be processed upon receiving 
       release indication with cause of aborted */
    emm_conn_abort_mark_pended_message(emm_ctrl_data_ptr);
#ifdef FEATURE_LTE_TO_1X
    emm_conn_abort_mark_pending_irat_msg(emm_ctrl_data_ptr);
#endif
    msgr_init_hdr((msgr_hdr_s *)&conn_abort_req.msg_hdr, MSGR_NAS_EMM, LTE_RRC_CONN_ABORT_REQ) ;

    /*Stop the MAK and/or SQN timer(s) and bar the active cell if MAK & SQN timers
      were running during HO*/
    if((abort_cause == LTE_RRC_CONN_ABORT_CAUSE_IRAT_SUCCESS) &&
       (emm_stop_auth_timers(emm_ctrl_data_ptr) == TRUE))
    {
      MSG_HIGH_DS_2(MM_SUB, "=EMM= MAK/SQN timer stopped during IRAT HO. Bar cell id = %d, earfcn = %d.",
                    emm_ctrl_data_ptr->phy_cell_id,
                    emm_ctrl_data_ptr->freq);
      cell_barring_reqd = TRUE;
    }

    conn_abort_req.cell_barring_is_reqd = cell_barring_reqd ;

    /*This cause value will be LTE_RRC_CONN_ABORT_CAUSE_IRAT_SUCCESS for PS_HO, 
      for all others it will be defaulted to LTE_RRC_CONN_ABORT_CAUSE_NORMAL*/
    conn_abort_req.cause = abort_cause;

    /* if cell barring is required, indicate the cell ID and the cell frequency information 
       - this information is saved in the emm ctrl data when the LTE RRC SERVICE IND is received
    */
    if( cell_barring_reqd )
    {
      conn_abort_req.phy_cell_id = emm_ctrl_data_ptr->phy_cell_id;
      conn_abort_req.earfcn      = emm_ctrl_data_ptr->freq;
    }

    /*during auth procedure, if there is user mode change 
      auth response could be received while aborting conn*/
    MSG_HIGH_DS_0(MM_SUB, "=EMM= Cancel Authentication");
    emm_cancel_authentication();

    emm_msgr_send((msgr_hdr_s *)&conn_abort_req, sizeof(lte_rrc_conn_abort_req_s));
    MSG_HIGH_DS_0(MM_SUB, "=EMM= Sent LTE_RRC_CONN_ABORT_REQ");
	if(mm_get_ulog_handle() != NULL)
     {
      curr_time = time_get_ms_native();
      ULOG_RT_PRINTF_1(mm_get_ulog_handle(), "=EMM= Sent LTE_RRC_CONN_ABORT_REQ  to RRC time %llu", curr_time);
     }
    
    /* we are sending ABORT_REQ, so set this flag to TRUE */
    emm_ctrl_data_ptr->aborting_conn = TRUE;

    /* If EMM is in connected state, move EMM to releasing state. EMM will move
       back to idle state upon receiving release ind and service ind later.
       If EMM is in waiting rrc cnf state, let EMM stay in this state. EMM will
       move back to idle state upon receiving rrc est cnf with cause aborted */
    if(emm_connection_state == EMM_CONNECTED_STATE)
    {
      EMM_MOVES_TO_RELEASING_RRC_CONNECTION_STATE();
    }
  }
} /* end of emm_rrc_abort_connection() */

/*===========================================================================

FUNCTION    EMM_SEND_RRC_CELL_BAR_REQ

DESCRIPTION
  This function sends LTE_RRC_CELL_BAR_REQ to RRC

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_send_rrc_cell_bar_req
(
  emm_ctrl_data_type *emm_ctrl_data_ptr
) 
{
  lte_rrc_cell_bar_req_s   rrc_cell_bar_req;

  

  msgr_init_hdr(&rrc_cell_bar_req.msg_hdr, MSGR_NAS_EMM, 
                LTE_RRC_CELL_BAR_REQ);

  rrc_cell_bar_req.phy_cell_id = emm_ctrl_data_ptr->phy_cell_id;
  rrc_cell_bar_req.earfcn      = emm_ctrl_data_ptr->freq;

  emm_msgr_send((msgr_hdr_s *)&rrc_cell_bar_req, 
               sizeof(lte_rrc_cell_bar_req_s));
  MSG_HIGH_DS_2(MM_SUB, "=EMM= Sent LTE_RRC_CELL_BAR_REQ for cell id = %d, earfcn = %d",
                  rrc_cell_bar_req.phy_cell_id,
                  rrc_cell_bar_req.earfcn);
} /* end of emm_send_rrc_cell_bar_req() */

/*===========================================================================

FUNCTION    EMM_SEND_RRC_DEACT_REQ

DESCRIPTION
  This function sends LTE_RRC_DEACTIVATE_REQ to RRC

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_send_rrc_deact_req
(
  sys_stop_mode_reason_e_type   stop_mode_reason,
  emm_ctrl_data_type *emm_ctrl_data_ptr
) 
{
  lte_rrc_deactivate_req_s   rrc_deactivate_req;

  

  msgr_init_hdr(&rrc_deactivate_req.msg_hdr, MSGR_NAS_EMM, 
                LTE_RRC_DEACTIVATE_REQ);
#ifdef FEATURE_DUAL_SIM
  if (stop_mode_reason == SYS_STOP_MODE_REASON_DSDS_SUSPEND)
  {
    /* mapping it to deep sleep */
    stop_mode_reason = SYS_STOP_MODE_REASON_ALT_SCAN;
  }
#endif

  rrc_deactivate_req.deactivate_reason = stop_mode_reason;

  emm_set_tau_trm_priority(FALSE);
  
#ifdef FEATURE_WWAN_GNSS_DYNAMIC
  if ((gmm_check_wwan_tech_active() == FALSE &&
       EMM_GET_STATE() != EMM_NULL) && (rrc_deactivate_req.deactivate_reason == SYS_STOP_MODE_REASON_IRAT || 
       	rrc_deactivate_req.deactivate_reason ==	SYS_STOP_MODE_REASON_ALT_SCAN))
  {
    emm_send_rrc_wwan_set_priority(TRUE);
    gmm_set_wwan_resumption_req(TRUE);
  }
#endif   

  emm_msgr_send((msgr_hdr_s *)&rrc_deactivate_req, 
               sizeof(lte_rrc_deactivate_req_s));
  MSG_HIGH_DS_1(MM_SUB, "=EMM= Sent LTE_RRC_DEACTIVATE_REQ with reason = %d",
                  stop_mode_reason);
#ifdef FEATURE_NAS_GW
  /* Set Ignore to TRUE. GW will check this flag before proceeding to act on the activation_ind */
  emm_ctrl_data_ptr->ignore_GW_activation_ind = TRUE;
#endif

  emm_ctrl_data_ptr->rrc_deact_req.deact_cnf_pending = TRUE;
#ifdef FEATURE_CIOT
if ((rrc_deactivate_req.deactivate_reason == SYS_STOP_MODE_REASON_IRAT) || 
	(rrc_deactivate_req.deactivate_reason == SYS_STOP_MODE_REASON_DEEP_SLEEP) ||
	(rrc_deactivate_req.deactivate_reason == SYS_STOP_MODE_REASON_ALT_SCAN))
{
	MSG_HIGH_DS_0(MM_SUB, "=EMM=  Do not reset Connection suspened var ");
}
else
{
  emm_ctrl_data_ptr->is_conn_suspended = FALSE; 
  emm_ctrl_data_ptr->conn_suspended_rat = SYS_RAT_NONE;  
}
  emm_ctrl_data_ptr->drb_pending_post_resume = FALSE;
#endif  

} /* end of emm_send_rrc_deact_req() */

/*===========================================================================

FUNCTION    EMM_SEND_RRC_CSFB_CALL_STATUS_IND

DESCRIPTION
  This function sends rrc_csfb_call_status to RRC

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_send_rrc_csfb_call_status_ind
(
  lte_rrc_csfb_call_status_type_e call_status
) 
{
  lte_rrc_csfb_call_status_ind_s   rrc_csfb_status_ind;

  

  msgr_init_hdr(&rrc_csfb_status_ind.msg_hdr, MSGR_NAS_EMM, 
                LTE_RRC_CSFB_CALL_STATUS_IND);

  rrc_csfb_status_ind.call_status_e = call_status;

  emm_msgr_send((msgr_hdr_s *)&rrc_csfb_status_ind, 
               sizeof(lte_rrc_csfb_call_status_ind_s));
  MSG_HIGH_DS_1(MM_SUB, "=EMM= LTE_RRC_CSFB_CALL_STATUS_IND = %d",
                  call_status);
} /* end of emm_send_rrc_csfb_call_status_ind() */

/*===========================================================================

FUNCTION    EMM_INIT_RRC_DEACT_REQ

DESCRIPTION
  This function sends RRC_DEACT_REQ or aborts the connection depending on EMM
  connection state    

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_init_rrc_deact_req
(
  sys_stop_mode_reason_e_type   stop_mode_reason,
  emm_ctrl_data_type           *emm_ctrl_data_ptr
) 
{
  byte  index = 0;
  emm_connection_state_type  emm_connection_state;
  boolean is_t3247_need_not_be_stopped = FALSE;

  ASSERT(emm_ctrl_data_ptr != NULL);
  ASSERT(emm_ctrl_data_ptr->last_rrc_service_ind_ptr != NULL);
  ASSERT(emm_ctrl_data_ptr->is_sim_ps_invalid_due_to_dos != NULL); 
  ASSERT(emm_ctrl_data_ptr->is_sim_cs_invalid_due_to_dos != NULL); 

  if(emm_ctrl_data_ptr->rrc_deact_req.deact_cnf_pending == TRUE)
  {
    if (emm_ctrl_data_ptr->rrc_deact_req.deact_reason == stop_mode_reason)
    {
      MSG_HIGH_DS_0(MM_SUB, "=EMM= deact req already sent to LTE RRC with same cause");
      return;
	}
	else if (emm_ctrl_data_ptr->rrc_deact_req.deact_reason != stop_mode_reason &&
		     emm_ctrl_data_ptr->reg_req_pended_indication == TRUE &&
		     emm_ctrl_data_ptr->reg_req_pending_mm_cmd_ptr->cmd.hdr.message_id == MMR_REG_REQ &&
		     emm_ctrl_data_ptr->reg_req_pending_mm_cmd_ptr->cmd.mmr_reg_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_LIMITED &&
		     emm_ctrl_data_ptr->reg_req_pending_mm_cmd_ptr->cmd.mmr_reg_req.type == MMR_REG_REQ_FULL_RAT_MODE)
	{
	  // This is to handle cases where CM sends stop mode deep sleep followed by
      // service request full RAT mode to get out of OOS. Set this flag to remember
      // to process pended reg req only after receiving deact confirmation for IRAT
      // and not deep sleep	  
      is_last_stop_reason_diff = TRUE;
	}
  }
  emm_ctrl_data_ptr->rrc_deact_req.deact_reason = stop_mode_reason;
#ifdef FEATURE_NAS_GW
  emm_ctrl_data_ptr->ignore_GW_activation_ind = TRUE;
#endif
  if(emm_ctrl_data_ptr->cell_barred_tau_pending == TRUE)
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= reset the flag cell_barred_tau_pending");
    emm_ctrl_data_ptr->cell_barred_tau_pending = FALSE;
  }
  emm_connection_state = EMM_GET_CONNECTION_STATE();
  if(emm_ctrl_data_ptr->emm_sms_active == TRUE)
  {
    /* send release indication to CNM to indicate failure of SMS started in LTE*/
    emm_send_mmcnm_rel_ind(emm_ctrl_data_ptr, PS_SMS_FAIL);
  }
  if((stop_mode_reason == SYS_STOP_MODE_REASON_ALT_SCAN) &&
     (emm_ctrl_data_ptr->last_rrc_service_ind_ptr->t311_rem_time > 0) &&
     (emm_ctrl_data_ptr->reg_req_pending_ptr != NULL) &&
     (mm_checkif_rat_pri_list_has_lte_rat(&emm_ctrl_data_ptr->reg_req_pending_ptr->rat_pri_list_info) == TRUE))
  {
    emm_send_rrc_deact_req(stop_mode_reason,emm_ctrl_data_ptr);
    emm_ctrl_data_ptr->rrc_deact_req.deact_req_pending = FALSE;
  }
#if defined FEATURE_SGLTE || defined FEATURE_GPS_WWAN_PRIORITY_SEARCH
  else if ((stop_mode_reason == SYS_STOP_MODE_REASON_SUSPEND_STACK) &&
           (emm_ctrl_data_ptr->last_rrc_service_ind_ptr->t311_rem_time > 0))
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= Aborting connection for SYS_STOP_MODE_REASON_SUSPEND_STACK ");
    emm_rrc_abort_connection(emm_ctrl_data_ptr, FALSE,LTE_RRC_CONN_ABORT_CAUSE_NORMAL);
    emm_ctrl_data_ptr->rrc_deact_req.deact_req_pending = TRUE; 
  }
#endif
  else
  {
    /* Set t311_rem_time = 0 & no_svc_cause = SYS_NO_SVC_CAUSE_NORMAL
       to prevent Service request to be sent in connected state during
       w2l handover*/
    emm_ctrl_data_ptr->last_rrc_service_ind_ptr->t311_rem_time = 0;
    emm_ctrl_data_ptr->last_rrc_service_ind_ptr->no_svc_cause = 
                                                SYS_NO_SVC_CAUSE_NORMAL;
    switch(emm_connection_state)
    {
      case EMM_IDLE_STATE:
        emm_send_rrc_deact_req(stop_mode_reason, emm_ctrl_data_ptr);
        emm_ctrl_data_ptr->rrc_deact_req.deact_req_pending = FALSE;      
        break;
      case EMM_RELEASING_RRC_CONNECTION_STATE:
        if(emm_ctrl_data_ptr->aborting_conn == FALSE)
        {
          emm_send_rrc_deact_req(stop_mode_reason, emm_ctrl_data_ptr);
          emm_ctrl_data_ptr->rrc_deact_req.deact_req_pending = FALSE;
        }
        else
        {
          emm_ctrl_data_ptr->rrc_deact_req.deact_req_pending = TRUE;
        }
        break;
      case EMM_WAITING_FOR_RRC_CONFIRMATION_STATE:
        if(emm_ctrl_data_ptr->aborting_conn == FALSE)
        {
          if(stop_mode_reason == SYS_STOP_MODE_REASON_IRAT)
          {
            MSG_HIGH_DS_0(MM_SUB, "=EMM= converting SYS_STOP_MODE_REASON_IRAT to LTE_RRC_CONN_ABORT_CAUSE_IRAT_SUCCESS for RRC");
            emm_rrc_abort_connection(emm_ctrl_data_ptr, FALSE,LTE_RRC_CONN_ABORT_CAUSE_IRAT_SUCCESS);
          }
          else
          {

            emm_rrc_abort_connection(emm_ctrl_data_ptr, FALSE,LTE_RRC_CONN_ABORT_CAUSE_NORMAL);
          }
        }
        emm_ctrl_data_ptr->rrc_deact_req.deact_req_pending = TRUE;   
        break;
      default:
        /* Must abort connection first. RRC deact req will be sent upon receiving 
         RRC connection abort response */
        if(stop_mode_reason == SYS_STOP_MODE_REASON_IRAT)
        {
          MSG_HIGH_DS_0(MM_SUB, "=EMM= converting SYS_STOP_MODE_REASON_IRAT to LTE_RRC_CONN_ABORT_CAUSE_IRAT_SUCCESS for RRC");
          emm_rrc_abort_connection(emm_ctrl_data_ptr, FALSE,LTE_RRC_CONN_ABORT_CAUSE_IRAT_SUCCESS);
        }
        else
        {

          emm_rrc_abort_connection(emm_ctrl_data_ptr, FALSE,LTE_RRC_CONN_ABORT_CAUSE_NORMAL);
        }
        emm_ctrl_data_ptr->rrc_deact_req.deact_req_pending = TRUE; 
        break;
    }
  }
  if ( (stop_mode_reason == SYS_STOP_MODE_REASON_MODE_CHANGE)     || 
       (stop_mode_reason == SYS_STOP_MODE_REASON_IRAT)            ||
       (stop_mode_reason == SYS_STOP_MODE_REASON_LOCAL_DETACH)    || 
       (stop_mode_reason ==  SYS_STOP_MODE_REASON_UE_MODE_CHANGE) )
  {
    is_t3247_need_not_be_stopped = TRUE;
  }

  /* Do not stop timers for the scenarios
     - DEEP SLEEP
     - ALT SCAN (OOS, RLF and BSR)
  */
  if((stop_mode_reason != SYS_STOP_MODE_REASON_DEEP_SLEEP) &&
     (stop_mode_reason != SYS_STOP_MODE_REASON_ALT_SCAN) &&
#ifdef FEATURE_DUAL_SIM
     (stop_mode_reason != SYS_STOP_MODE_REASON_DSDS_SUSPEND )&&
#endif
#if defined FEATURE_SGLTE || defined FEATURE_GPS_WWAN_PRIORITY_SEARCH
     (stop_mode_reason != SYS_STOP_MODE_REASON_SUSPEND_STACK) &&
#endif
     (stop_mode_reason != SYS_STOP_MODE_REASON_DUAL_SWITCH))
  {
    /* Stop all EMM timers */
    for(index = MIN_EMM_TIMER; index <= MAX_EMM_TIMER; index++)
    {
      /* Stopping T3411 and T3402 for IRAT cases may not be correct here.
         Need to relook at this part of the code */
      if(mm_timer_status[index] != TIMER_STOPPED)
      {
        if(index != EMM_FORBIDDEN_TAI_CLEAR_TIMER
             &&!(((stop_mode_reason == SYS_STOP_MODE_REASON_IRAT) ||
                  (stop_mode_reason==SYS_STOP_MODE_REASON_UE_MODE_CHANGE))
               &&(index == TIMER_T3412 || index == TIMER_T3423 ||
                  index == TIMER_T3442 || index == TIMER_T3346 ||
                  index == TIMER_T3246 || index == TIMER_T3448)) &&
               !(index == TIMER_T3247 && is_t3247_need_not_be_stopped))
        {

          if( (mm_get_t3247_behavior_enabled() == TRUE) &&
              ( (stop_mode_reason == SYS_STOP_MODE_REASON_LPM)||
                (stop_mode_reason == SYS_STOP_MODE_REASON_POWER_OFF) ) &&
                (index == TIMER_T3247)
            )
          {
            if((stop_mode_reason == SYS_STOP_MODE_REASON_LPM) && 
               ((*emm_ctrl_data_ptr->is_sim_cs_invalid_due_to_dos || *emm_ctrl_data_ptr->is_sim_ps_invalid_due_to_dos) &&
		 	    ((emm_ctrl_data_ptr->plmn_service_state->sim_state == SYS_SIM_STATE_NOT_AVAILABLE) ||
				 (emm_ctrl_data_ptr->plmn_service_state->sim_state == SYS_SIM_STATE_CS_PS_INVALID))))
            {
		mmgsdi_session_manage_illegal_subscription(mm_sim_mmgsdi_info.session_id,MMGSDI_SESSION_APP_IS_LEGAL);

            }
            mm_emm_clear_dos_counters();
          }
#ifdef FEATURE_CIOT
          /* Do not stop these timers for cause IRAT. They will be stopped when we acquire service on another rat*/
          if ((stop_mode_reason == SYS_STOP_MODE_REASON_IRAT)&&((index == TIMER_T3411)||(index == TIMER_T3402)))
          {
              continue;
          }
#endif
          mm_stop_timer(index);
		  MSG_HIGH_DS_1(MM_SUB, "=EMM= Deactivating RRC - Stop timer %d", index);
          mm_timer_status[index] = TIMER_STOPPED;
        }
      }
    }

#if defined(FEATURE_NAS_REL10) || defined(FEATURE_LTE_REL10)
    if((mm_timer_status[TIMER_T3346] == TIMER_ACTIVE) &&
       (stop_mode_reason != SYS_STOP_MODE_REASON_IRAT)&&
       (stop_mode_reason != SYS_STOP_MODE_REASON_ALT_SCAN)
#ifdef FEATURE_DUAL_SIM
       && (stop_mode_reason != SYS_STOP_MODE_REASON_DSDS_SUSPEND)
#endif
       && (stop_mode_reason !=SYS_STOP_MODE_REASON_UE_MODE_CHANGE)
        )
    {
         /* Find out how much timeout is left */
      gmm_t3346_backoff_timer_value =(dword) gs_enquire_timer_ext ( 
                                                          get_mm_timer_queue(),
                                                          TIMER_T3346
                                                        ); 
      mm_stop_timer(TIMER_T3346);
#ifdef FEATURE_CIOT
     if(gmm_t3346_backoff_timer_value <= 0)
     {
      emm_ctrl_data_ptr->is_t3346_started_for_mo_excp = FALSE;
     }
 #endif
#ifdef FEATURE_NAS_GW
      gmm_set_gmm_is_congestion_due_to_rau(FALSE);
#endif
    }
#endif
  }

#ifdef FEATURE_CIOT
   emm_ctrl_data_ptr->emm_start_mo_excp_signalling = FALSE;
 #endif

  /* clear all cell barring timers while leaving LTE */
      emm_stop_cell_barring_timers(emm_ctrl_data_ptr
#ifdef FEATURE_NAS_EAB
                                   ,TRUE
#endif      
                                   );

  emm_ctrl_data_ptr->rrc_active = FALSE;

} /* end of emm_init_rrc_deact_req() */

void emm_send_sim_update
(
  emm_ctrl_data_type *emm_ctrl_data_ptr,
  boolean emm_is_key_need_to_update
)
{
  int index = 0;
#ifdef FEATURE_SGLTE
  if(MM_SUB_IS_SGLTE &&
     (mm_as_id == mm_cs_stack_id) && (mm_cs_stack_id != mm_ps_stack_id))
  {
    MSG_HIGH_DS_0(MM_SUB, "not sending sim update on CS stack");
  }
  else
#endif
  {
    lte_rrc_sim_update_req_s rrc_sim_update_req ;
    emm_security_context_info *context_ptr = NULL;

     

    msgr_init_hdr(&rrc_sim_update_req.msg_hdr, MSGR_NAS_EMM, LTE_RRC_SIM_UPDATE_REQ) ;

    rrc_sim_update_req.access_class = emm_ctrl_data_ptr->access_class;
    /* update the NAS security information */
    context_ptr = emm_get_context_by_state( emm_ctrl_data_ptr,CONTEXT_STATE_CURRENT );

    if( (context_ptr != NULL) && (emm_is_key_need_to_update == TRUE))
    {
      MSG_HIGH_DS_2(MM_SUB, "=EMM= Current context - type %d , with KSI %d ",
                    context_ptr->context_type,
                    context_ptr->nasKSI.keyset_id);
      rrc_sim_update_req.security_info_is_valid = TRUE;
      rrc_sim_update_req.security_info.eksi = context_ptr->nasKSI.keyset_id;  /* Key Set Identifier */
      memscpy((void*)&rrc_sim_update_req.security_info.k_asme[0],
            sizeof(rrc_sim_update_req.security_info.k_asme),
            (void*)context_ptr->nas_kasme,
            sizeof(context_ptr->nas_kasme));  /* KASME */

      /* NAS UL COUNT */
      rrc_sim_update_req.security_info.nas_count = 
      ((uint32)context_ptr->nas_ul_cnt[3] << 24) +
      ((uint32)context_ptr->nas_ul_cnt[2] << 16) + 
      ((uint32)context_ptr->nas_ul_cnt[1] << 8) + 
       (uint32)context_ptr->nas_ul_cnt[0];

      rrc_sim_update_req.security_info.flush_nas_security_context = emm_ctrl_data_ptr->reset_nas_context;
    }
    else
      rrc_sim_update_req.security_info_is_valid = FALSE ; 

    if ((emm_ctrl_data_ptr->plmn_service_state->sim_state == SYS_SIM_STATE_AVAILABLE) ||
        (emm_ctrl_data_ptr->plmn_service_state->sim_state == SYS_SIM_STATE_CS_INVALID)||
		(emm_ctrl_data_ptr->plmn_service_state->sim_state == SYS_SIM_STATE_CP_SMS_INVALID)
#if defined(FEATURE_LTE_REL9) && !defined(FEATURE_NAS_IMS_VOICE_DISABLED)
#ifdef FEATURE_NAS_E911_ENABLE

		||
        (emm_ctrl_data_ptr->emc_srv_status == LTE_NAS_EMC_SRV_ONLY)
#endif
#endif
       )
    {
      /* EMM provides RRC with the UE identity info */
      rrc_sim_update_req.ue_id_list_is_valid = TRUE;
      rrc_sim_update_req.ue_id_list.num_ue_identities = index;
      if( emm_ctrl_data_ptr->plmn_service_state->sim_state != SYS_SIM_STATE_NOT_AVAILABLE )
      {
        /* IMSI */
        rrc_sim_update_req.ue_id_list.num_ue_identities = index+1 ;
        rrc_sim_update_req.ue_id_list.ue_id[index].type = LTE_RRC_UE_ID_IMSI ;
        memscpy((void*)&rrc_sim_update_req.ue_id_list.ue_id[index].value.imsi,
              sizeof(rrc_sim_update_req.ue_id_list.ue_id[index].value.imsi),
              (void*)&emm_ctrl_data_ptr->imsi,
              sizeof(emm_ctrl_data_ptr->imsi));
        rrc_sim_update_req.access_class = emm_ctrl_data_ptr->access_class;
        index = index+1;
      }

      if (emm_ctrl_data_ptr->emm_guti_valid == TRUE)
      {
        /* S-TMSI */
        rrc_sim_update_req.ue_id_list.num_ue_identities = index+1 ;
        rrc_sim_update_req.ue_id_list.ue_id[index].type = LTE_RRC_UE_ID_STMSI ;
        rrc_sim_update_req.ue_id_list.ue_id[index].value.s_tmsi.mmec = emm_ctrl_data_ptr->emm_guti.mme_code ;
        rrc_sim_update_req.ue_id_list.ue_id[index].value.s_tmsi.m_tmsi = 
         ((uint32)emm_ctrl_data_ptr->emm_guti.m_tmsi[3] << 24) +
         ((uint32)emm_ctrl_data_ptr->emm_guti.m_tmsi[2] << 16) +
         ((uint32)emm_ctrl_data_ptr->emm_guti.m_tmsi[1] << 8) +
          (uint32)emm_ctrl_data_ptr->emm_guti.m_tmsi[0];
      }
    }
    else
    {
      rrc_sim_update_req.ue_id_list_is_valid = FALSE;
    }

#ifdef FEATURE_NAS_EAB
    if((emm_ctrl_data_ptr->eab_barring_flag == TRUE ) &&
       (reg_sim_per_subs_chk_low_pri_cfg((sys_modem_as_id_e_type)mm_sub_id) == TRUE) &&
       (reg_sim_per_subs_chk_eab_cfg((sys_modem_as_id_e_type)mm_sub_id) == TRUE))
    {
      rrc_sim_update_req.eab_enabled = TRUE;
    }
    else
#endif
    {
      rrc_sim_update_req.eab_enabled = FALSE;
    }
    
    emm_msgr_send((msgr_hdr_s *)&rrc_sim_update_req, sizeof(lte_rrc_sim_update_req_s));
    MSG_HIGH_DS_0(MM_SUB, "=EMM= Sent LTE_RRC_SIM_UPDATE_REQ");
  }
} /* end of emm_send_sim_update() */

/*===========================================================================

FUNCTION EMM_SEND_RRC_SYSTEM_UPDATE

DESCRIPTION
  This function sends LTE_RRC_SYSTEM_UPDATE_REQ to RRC

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_send_rrc_system_update
(
  emm_ctrl_data_type *emm_ctrl_data_ptr
)
{

#ifdef FEATURE_SGLTE
  if(MM_SUB_IS_SGLTE &&
     (mm_as_id == mm_cs_stack_id) && (mm_cs_stack_id != mm_ps_stack_id))
  {
    MSG_HIGH_DS_0(MM_SUB, "not sending system update on CS stack");
  }
  else
#endif
  {
  lte_rrc_system_update_req_s  *rrc_system_update_req_ptr;
#ifdef FEATURE_EQUIVALENT_HPLMN
  uint32 plmn_index = 0;
#endif

#if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW)
  uint16 index;
  uint8 length;
#endif 

   

    /* Allocate memory and initialize it */
  rrc_system_update_req_ptr = emm_modem_mem_alloc(sizeof(lte_rrc_system_update_req_s),
                                                  emm_ctrl_data_ptr);
  mm_check_for_null_ptr((void*)rrc_system_update_req_ptr);
  msgr_init_hdr(&rrc_system_update_req_ptr->msg_hdr, MSGR_NAS_EMM, LTE_RRC_SYSTEM_UPDATE_REQ) ;

/* Populate EHPLMN list or HPLMN */
  rrc_system_update_req_ptr->hplmn_info_is_valid = FALSE;
  rrc_system_update_req_ptr->ehplmn_list.num_plmns = 0;

  if (emm_ctrl_data_ptr->home_mcc)
  {
     rrc_system_update_req_ptr->freq_list_type =  LTE_RRC_FREQ_LIST_CPFL;
  }
  else
  {
      rrc_system_update_req_ptr->freq_list_type =  LTE_RRC_FREQ_LIST_DEFAULT;
  }

  if((emm_ctrl_data_ptr->plmn_service_state->sim_state == SYS_SIM_STATE_AVAILABLE) || 
     (emm_ctrl_data_ptr->plmn_service_state->sim_state == SYS_SIM_STATE_CS_INVALID))
  {  
#ifdef FEATURE_EQUIVALENT_HPLMN
    reg_sim_plmn_list_s_type *ehplmn_list_ptr;
    ehplmn_list_ptr = (reg_sim_plmn_list_s_type *) emm_modem_mem_alloc(
                                              sizeof(reg_sim_plmn_list_s_type),
                                              emm_ctrl_data_ptr);
    mm_check_for_null_ptr((void *)ehplmn_list_ptr);
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
    reg_sim_per_nas_stacks_ehplmn_list_get_ptr(ehplmn_list_ptr, (sys_modem_as_id_e_type)mm_as_id);
#else
    reg_sim_ehplmn_list_get_ptr(ehplmn_list_ptr);
#endif
    if(ehplmn_list_ptr->length > SYS_EHPLMN_LIST_MAX_LENGTH)
    {
      MSG_HIGH_DS_1(MM_SUB, "=EMM= EHPLMN list too long %d - RRC SYTEM UPDATE REQ",
                    ehplmn_list_ptr->length);
      rrc_system_update_req_ptr->ehplmn_list.num_plmns = SYS_EHPLMN_LIST_MAX_LENGTH;
    }
    else
    {
      rrc_system_update_req_ptr->ehplmn_list.num_plmns = 
                                       ehplmn_list_ptr->length;
    }
    for(plmn_index=0; plmn_index<rrc_system_update_req_ptr->ehplmn_list.num_plmns; plmn_index++)
    {
      rrc_system_update_req_ptr->ehplmn_list.plmn[plmn_index] = 
        emm_convert_nas_plmn_id_to_rrc_plmn_id(ehplmn_list_ptr->plmn[plmn_index]);
    }
    emm_modem_mem_free(ehplmn_list_ptr, emm_ctrl_data_ptr);
    ehplmn_list_ptr = NULL;
#else
    /* HPLMN */
#if defined FEATURE_DUAL_SIM|| defined FEATURE_SGLTE
    emm_ctrl_data_ptr->emm_home_plmn = reg_sim_per_subs_hplmn_get(mm_as_id);
#else
    emm_ctrl_data_ptr->emm_home_plmn = reg_sim_hplmn_get(); 
#endif

    rrc_system_update_req_ptr->hplmn_info_is_valid = TRUE;
    rrc_system_update_req_ptr->hplmn = emm_convert_nas_plmn_id_to_rrc_plmn_id(emm_ctrl_data_ptr->emm_home_plmn);
#endif
  } 

  /* RPLMN */
  emm_set_rrc_rplmn(&rrc_system_update_req_ptr->rplmn_info_is_valid,
                    &rrc_system_update_req_ptr->rplmn,
                    emm_ctrl_data_ptr);

  /* Forbidden TAI lists for roaming and manual selection */
  emm_set_rrc_forbidden_ta_list(&rrc_system_update_req_ptr->forbidden_ta_list,
                                &rrc_system_update_req_ptr->forbidden_manual_ta_list,
                                emm_ctrl_data_ptr);
  /* EPLMN list */
  emm_set_rrc_eplmn_list(&rrc_system_update_req_ptr->eplmn_list, emm_ctrl_data_ptr);

#if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW)
  if(*(emm_ctrl_data_ptr->nv_csg_support) > SYS_CSG_SUPPORT_DISABLED)
  {
    rrc_system_update_req_ptr->csg_whitelist_type = LTE_RRC_CSG_LIST_INITIALIZED;
    
    length = 0;
  
    for(index = 0; index < emm_ctrl_data_ptr->operator_csg_list->length; index++)
    {
      if(emm_ctrl_data_ptr->operator_csg_list->csg[index].timer_count == MM_TIMER_DISABLED)
      {
        rrc_system_update_req_ptr->csg_whitelist.csg_info[length].plmn = 
                                         emm_convert_nas_plmn_id_to_rrc_plmn_id(emm_ctrl_data_ptr->operator_csg_list->csg[index].plmn);
        rrc_system_update_req_ptr->csg_whitelist.csg_info[length].csg_id = emm_ctrl_data_ptr->operator_csg_list->csg[index].csg_id;
        length++;
      }
    }
  
    for(index = 0; index < emm_ctrl_data_ptr->allowed_csg_list->length; index++)
    {
      if(emm_ctrl_data_ptr->allowed_csg_list->csg[index].csg_id != SYS_CSG_ID_INVALID)
      {
        rrc_system_update_req_ptr->csg_whitelist.csg_info[length].plmn = 
                                  emm_convert_nas_plmn_id_to_rrc_plmn_id(emm_ctrl_data_ptr->allowed_csg_list->csg[index].plmn);
        rrc_system_update_req_ptr->csg_whitelist.csg_info[length].csg_id = emm_ctrl_data_ptr->allowed_csg_list->csg[index].csg_id;
        length++;
      }
    }
  
    rrc_system_update_req_ptr->csg_whitelist.num_entries = length;
  }
  else
  {
    rrc_system_update_req_ptr->csg_whitelist_type = LTE_RRC_CSG_LIST_UNINITIALIZED;
  }
#endif 
  rrc_system_update_req_ptr->ce_restriction = emm_ctrl_data_ptr->plmn_service_state->eps_nw_feature_support.ce_restriction;
  emm_msgr_send((msgr_hdr_s *)rrc_system_update_req_ptr, 
                             sizeof(lte_rrc_system_update_req_s));
  MSG_HIGH_DS_1(MM_SUB, "=EMM= Sent LTE_RRC_SYSTEM_UPDATE_REQ CE rest %d",rrc_system_update_req_ptr->ce_restriction);

  /* Free the memory block */
  emm_modem_mem_free(rrc_system_update_req_ptr, emm_ctrl_data_ptr);
 }

} /* emm_send_rrc_system_update*/

/*===========================================================================

FUNCTION    EMM_SEND_RRC_UE_CAP_CHANGE_INFO

DESCRIPTION
  This function sends the UE capabilities info to RRC

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void emm_send_rrc_ue_cap_change_info
(
  mm_cmd_type  *cmd_ptr,
  emm_ctrl_data_type *emm_ctrl_data_ptr
)
{

  lte_rrc_ue_cap_change_ind_s            rrc_ue_cap_info_ind;

  msgr_init_hdr(&rrc_ue_cap_info_ind.msg_hdr, MSGR_NAS_EMM, LTE_RRC_UE_CAP_CHANGE_IND) ;
  rrc_ue_cap_info_ind.enable_srvcc_cap = cmd_ptr->cmd.cm_mm_ue_capabilities_info.srvcc_on_fly_value;

  emm_msgr_send((msgr_hdr_s *)&rrc_ue_cap_info_ind, sizeof(lte_rrc_ue_cap_change_ind_s));
  MSG_HIGH_DS_1(MM_SUB, "=EMM= Sent LTE_RRC_UE_CAP_CHANGE_IND and srvcc value is %d", rrc_ue_cap_info_ind.enable_srvcc_cap);
} /* end of emm_send_rrc_ue_cap_info() */

/*===========================================================================

FUNCTION EMM_SEND_RRC_NW_SEL_MODE_RESET_REQ

DESCRIPTION
  This function sends LTE_RRC_NW_SEL_MODE_RESET_REQ to RRC using the given 
  network selection mode as payload

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_send_rrc_nw_sel_mode_reset_req
(
  sys_network_selection_mode_e_type  network_select_mode,
  sys_rat_pri_list_info_s_type      *rat_pri_list_ptr,
  emm_ctrl_data_type                *emm_ctrl_data_ptr
)
{
  lte_rrc_nw_sel_mode_reset_req_s  rrc_nw_sel_mode_rest_req;
  
  

  /* Init */
  memset(&rrc_nw_sel_mode_rest_req,0,sizeof(lte_rrc_nw_sel_mode_reset_req_s));
  msgr_init_hdr((msgr_hdr_s *)&rrc_nw_sel_mode_rest_req.msg_hdr, 
                MSGR_NAS_EMM, 
                LTE_RRC_NW_SEL_MODE_RESET_REQ) ;

  switch(network_select_mode)
  {
    case SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION:
      /* RRC does not support LIMITED_MANUAL_ACQUISITION yet. Map it to LIMITED */
      rrc_nw_sel_mode_rest_req.network_select_mode = 
                                            SYS_NETWORK_SELECTION_MODE_LIMITED;
      break;
#ifdef FEATURE_1XSRLTE
    case SYS_NETWORK_SELECTION_MODE_AUTOMATIC_CAMP_ONLY:
      rrc_nw_sel_mode_rest_req.network_select_mode = 
                                            SYS_NETWORK_SELECTION_MODE_AUTOMATIC;
      break;

    case SYS_NETWORK_SELECTION_MODE_MANUAL_CAMP_ONLY:
      rrc_nw_sel_mode_rest_req.network_select_mode =  
                                            SYS_NETWORK_SELECTION_MODE_MANUAL;
      break;
#endif 
    default:
      rrc_nw_sel_mode_rest_req.network_select_mode = network_select_mode;
      break;
  }

  if(rat_pri_list_ptr != NULL)
  {
    rrc_nw_sel_mode_rest_req.update_rat_pri_list = TRUE;
    memscpy((void *)&rrc_nw_sel_mode_rest_req.rat_pri_list,
            sizeof(rrc_nw_sel_mode_rest_req.rat_pri_list),
            (void *)rat_pri_list_ptr, 
            sizeof(sys_rat_pri_list_info_s_type));
  }

  emm_msgr_send((msgr_hdr_s *)&rrc_nw_sel_mode_rest_req, 
               sizeof(lte_rrc_nw_sel_mode_reset_req_s));
  MSG_HIGH_DS_1(MM_SUB, "=EMM= Sent LTE_RRC_NW_SEL_MODE_RESET_REQ with network_select_mode= %u ",
                  network_select_mode);

} /* end of emm_send_rrc_nw_sel_mode_reset_req() */

/*===========================================================================

FUNCTION EMM_SEND_RRC_ACT_RSP

DESCRIPTION
  This function sends LTE_RRC_ACTIVATION_RSP to RRC 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#ifdef FEATURE_NAS_GW
void emm_send_rrc_act_rsp
(
  boolean  status,
  emm_ctrl_data_type                *emm_ctrl_data_ptr
)
{
  lte_rrc_act_rsp_s act_rsp;


  

    /* Init */
  memset(&act_rsp,0x0,sizeof(lte_rrc_act_rsp_s));
  msgr_init_hdr((msgr_hdr_s *)&act_rsp.msg_hdr, MSGR_NAS_EMM, LTE_RRC_ACTIVATION_RSP) ;

  act_rsp.status = status;
  act_rsp.plmn   = emm_convert_nas_plmn_id_to_rrc_plmn_id(emm_ctrl_data_ptr->plmn_service_state->plmn);

  emm_msgr_send((msgr_hdr_s *)&act_rsp, 
               sizeof(lte_rrc_act_rsp_s));
  MSG_HIGH_DS_1(MM_SUB, "=EMM= Sent LTE_RRC_ACTIVATION_RSP with status= %u ",
                  status);

  mmsend_rat_change_event();
}

/*===========================================================================

FUNCTION EMM_SEND_NAS_UMTS_KEY_RSP

DESCRIPTION
  This function sends LTE_RRC_NAS_UMTS_KEY_RSP TO RRC 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_send_nas_umts_key_rsp
(
  auth_gprs_cipherkey_type    Ck,
  auth_gprs_integritykey_type Ik,
  auth_gprs_cipherkey_type    Ck_srvcc,
  auth_gprs_integritykey_type Ik_srvcc
)
{
  lte_rrc_nas_umts_key_rsp_s umts_key_rsp;

    /* Init */
  memset(&umts_key_rsp,0x0,sizeof(lte_rrc_nas_umts_key_rsp_s));
  msgr_init_hdr((msgr_hdr_s *)&umts_key_rsp.msg_hdr, MSGR_NAS_EMM, LTE_RRC_NAS_UMTS_KEY_RSP) ;

  memscpy((void*)&umts_key_rsp.sec_info.ck[0],
          sizeof(umts_key_rsp.sec_info.ck),
          (void*)&Ck.value_data,
          sizeof(Ck.value_data));
  
  memscpy((void*)&umts_key_rsp.sec_info.ik[0],
          sizeof(umts_key_rsp.sec_info.ik),
          (void*)&Ik.value_data,
          sizeof(Ik.value_data));

  /* Change needed in //source/qcom/qct/modem/lte/api/main/latest/lte_irat_types.h#21 to add ck_srvcc and ik_srvcc */
  memscpy((void*)&umts_key_rsp.sec_info.ck_srvcc[0],
          sizeof(umts_key_rsp.sec_info.ck_srvcc),
          (void*)&Ck_srvcc.value_data,
          sizeof(Ck_srvcc.value_data));
  memscpy((void*)&umts_key_rsp.sec_info.ik_srvcc[0],
          sizeof(umts_key_rsp.sec_info.ik_srvcc),
          (void *)&Ik_srvcc.value_data,
          sizeof(Ik_srvcc.value_data));

  emm_msgr_send((msgr_hdr_s *)&umts_key_rsp, 
               sizeof(lte_rrc_nas_umts_key_rsp_s));
  MSG_HIGH_DS_0(MM_SUB, "=EMM= Sent LTE_RRC_NAS_UMTS_KEY_RSP ");

}/* emm_send_nas_umts_key_rsp */

/*===========================================================================

FUNCTION EMM_SEND_NAS_GSM_KEY_RSP

DESCRIPTION
  This function sends LTE_RRC_NAS_GSM_KEY_RSP TO RRC 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_send_nas_gsm_key_rsp
(
  auth_gprs_cipherkey_type    Ck,
  auth_gprs_integritykey_type Ik,
  auth_gprs_cipherkey_type    Ck_srvcc,
  auth_gprs_integritykey_type Ik_srvcc,
  auth_gsm_cipherkey_type     Kc,
  auth_gsm_cipherkey_type     gsm_kc_128bit,
  byte                        gsm_kcsn
)
{
  lte_rrc_nas_umts_key_rsp_s gsm_key_rsp;

    /* Init */
  memset(&gsm_key_rsp,0x0,sizeof(lte_rrc_nas_umts_key_rsp_s));
  msgr_init_hdr((msgr_hdr_s *)&gsm_key_rsp.msg_hdr, MSGR_NAS_EMM, LTE_RRC_NAS_GSM_KEY_RSP) ;

  memscpy((void*)&gsm_key_rsp.sec_info.ck[0],
          sizeof(gsm_key_rsp.sec_info.ck),
          (void*)&Ck.value_data,
          sizeof(Ck.value_data));
  
  memscpy((void*)&gsm_key_rsp.sec_info.ik[0],
          sizeof(gsm_key_rsp.sec_info.ik),
          (void*)&Ik.value_data,
          sizeof(Ik.value_data));

  /* Change needed in //source/qcom/qct/modem/lte/api/main/latest/lte_irat_types.h#21 to add ck_srvcc and ik_srvcc */
  memscpy((void*)&gsm_key_rsp.sec_info.ck_srvcc[0],
          sizeof(gsm_key_rsp.sec_info.ck_srvcc),
          (void*)&Ck_srvcc.value_data,
          sizeof(Ck_srvcc.value_data));
  memscpy((void*)&gsm_key_rsp.sec_info.ik_srvcc[0],
          sizeof(gsm_key_rsp.sec_info.ik_srvcc),
          (void *)&Ik_srvcc.value_data,
          sizeof(Ik_srvcc.value_data));  

  memscpy((void*)&gsm_key_rsp.sec_info.kc[0],
          sizeof(gsm_key_rsp.sec_info.kc),
          (void *)&Kc.value_data,
          sizeof(Kc.value_data));
  memscpy((void*)&gsm_key_rsp.sec_info.kc_128bit[0],
          sizeof(gsm_key_rsp.sec_info.kc_128bit),
          (void*)&gsm_kc_128bit.value_data,
          sizeof(gsm_kc_128bit.value_data));

  gsm_key_rsp.sec_info.kcsn = gsm_kcsn;
  
  emm_msgr_send((msgr_hdr_s *)&gsm_key_rsp, 
               sizeof(lte_rrc_nas_umts_key_rsp_s));
  MSG_HIGH_DS_0(MM_SUB, "=EMM= Sent LTE_RRC_NAS_GSM_KEY_RSP ");

}/* emm_send_nas_gsm_key_rsp */
#endif

/*===========================================================================

FUNCTION EMM_SEND_NAS_LTE_KEY_RSP

DESCRIPTION
  This function sends LTE_RRC_NAS_LTE_KEY_RSP TO RRC 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_send_nas_lte_key_rsp
(
  byte *nas_kasme,
  byte *nas_ul_cnt
)
{
  lte_rrc_nas_lte_key_rsp_s lte_key_rsp;

    /* Init */
  memset(&lte_key_rsp,0x0,sizeof(lte_rrc_nas_lte_key_rsp_s));
  msgr_init_hdr((msgr_hdr_s *)&lte_key_rsp.msg_hdr, MSGR_NAS_EMM, LTE_RRC_NAS_LTE_KEY_RSP) ;

  memscpy((void*)&lte_key_rsp.security_info.k_asme[0],
          sizeof(lte_key_rsp.security_info.k_asme),
          (void*)nas_kasme,
          sizeof(lte_key_rsp.security_info.k_asme));
  memscpy((void*)&lte_key_rsp.security_info.nas_count,
          sizeof(lte_key_rsp.security_info.nas_count),
          (void*)nas_ul_cnt,
          sizeof(lte_key_rsp.security_info.nas_count));

  emm_msgr_send((msgr_hdr_s *)&lte_key_rsp, 
               sizeof(lte_rrc_nas_lte_key_rsp_s));
  MSG_HIGH_DS_0(MM_SUB, "=EMM= Sent LTE_RRC_NAS_LTE_KEY_RSP ");

}/* emm_send_nas_lte_key_rsp */

/*===========================================================================

FUNCTION    EMM_SEND_RRC_DRX_UPDATE

DESCRIPTION
  This function sends the new DRX value that was requested by QCHAT application to L-RRC.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void emm_send_rrc_drx_update
(
  void
)
{
#ifdef FEATURE_NAS_GW
  if(mm_is_multimode_sub())
#endif
  {
    lte_rrc_drx_info_req_s            rrc_drx_info_req;
    sys_drx_cn_coefficient_s1_e_type  drx_coeff;
  
    /* Init */
    memset((void *)&rrc_drx_info_req,0x0,sizeof(lte_rrc_drx_info_req_s));
  
    msgr_init_hdr(&rrc_drx_info_req.msg_hdr, MSGR_NAS_EMM, LTE_RRC_DRX_INFO_REQ) ;
    drx_coeff = emm_get_gmm_drx_cn_coff_s1();
    MSG_HIGH_DS_1(MM_SUB, "=EMM= DRX Value Rx %d ", drx_coeff);
  
    switch(drx_coeff)
    {
      case SYS_DRX_NOT_SPECIFIED_BY_MS:
        rrc_drx_info_req.paging_cycle = LTE_RRC_DRX_CN_NOT_SPECIFIED ;
        break;
      case SYS_DRX_CN_COEFFICIENT_S1_CN6_T32:
        rrc_drx_info_req.paging_cycle = LTE_RRC_DRX_CN_COEFFICIENT_S1_T32 ;
        break;
      case SYS_DRX_CN_COEFFICIENT_S1_CN7_T64:
        rrc_drx_info_req.paging_cycle = LTE_RRC_DRX_CN_COEFFICIENT_S1_T64 ;
        break;
      case SYS_DRX_CN_COEFFICIENT_S1_CN8_T128:
        rrc_drx_info_req.paging_cycle = LTE_RRC_DRX_CN_COEFFICIENT_S1_T128 ;
        break;
      case SYS_DRX_CN_COEFFICIENT_S1_CN9_T256:
        rrc_drx_info_req.paging_cycle = LTE_RRC_DRX_CN_COEFFICIENT_S1_T256 ;
        break;
      default:
        MSG_HIGH_DS_0(MM_SUB, "=EMM= Invalid DRX Value, use default DRX_CN_NOT_SPECIFIED ");
        rrc_drx_info_req.paging_cycle = LTE_RRC_DRX_CN_NOT_SPECIFIED ;
        break;
    }
  
    MSG_HIGH_DS_1(MM_SUB, "=EMM= Sending DRX Value %d to RRC ", rrc_drx_info_req.paging_cycle);
  
    emm_msgr_send((msgr_hdr_s *)&rrc_drx_info_req, sizeof(lte_rrc_drx_info_req_s));
  }
} /* end of emm_send_rrc_drx_update() */


/*===========================================================================

FUNCTION    EMM_SEND_RRC_FORBIDDEN_TA_LA_LIST_RESET

DESCRIPTION
  This function sends the forbidden TA/LA list reset to RRC
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void emm_send_rrc_forbidden_ta_la_list_reset
(
  void
)
{
  lte_rrc_forbidden_ta_list_reset_req_s            forbidden_ta_list_reset_req;


  /* Init */
  memset((void *)&forbidden_ta_list_reset_req,0x0,sizeof(lte_rrc_forbidden_ta_list_reset_req_s));

  msgr_init_hdr(&forbidden_ta_list_reset_req.msg_hdr, MSGR_NAS_EMM, LTE_RRC_FORBIDDEN_TA_LIST_RESET_REQ) ;

  MSG_HIGH_DS_0(MM_SUB, "=EMM= Sending LTE_RRC_FORBIDDEN_TA_LIST_RESET_REQ reset to RRC ");

  emm_msgr_send((msgr_hdr_s *)&forbidden_ta_list_reset_req, sizeof(lte_rrc_forbidden_ta_list_reset_req_s));
} /* end of emm_send_rrc_forbidden_ta_la_list_reset() */

#if defined FEATURE_SGLTE || defined FEATURE_1XSRLTE ||defined FEATURE_SVLTE_DUAL_SIM
void emm_send_ue_mode_change_ind(sys_ue_mode_e_type   ue_mode,boolean   ue_submode_is_srlte)
{
  lte_rrc_ue_mode_ind_s  rrc_ue_mode_ind ;
  msgr_init_hdr(&rrc_ue_mode_ind.msg_hdr, MSGR_NAS_EMM, LTE_RRC_UE_MODE_IND) ;
  rrc_ue_mode_ind.ue_mode = ue_mode;
  rrc_ue_mode_ind.is_ue_mode_substate_srlte = ue_submode_is_srlte;
  emm_msgr_send((msgr_hdr_s *)&rrc_ue_mode_ind, sizeof(lte_rrc_ue_mode_ind_s));
  MSG_HIGH_DS_0(MM_SUB, "=EMM= Sent LTE_RRC_UE_MODE_IND");
}
#endif

#ifdef FEATURE_DUAL_SIM
/*===========================================================================

FUNCTION    EMM_SEND_DS_STAT_CHANGE_REQ

DESCRIPTION
  This function sends the DS stat change request to RRC

  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/

void emm_send_rrc_ds_stat_change_req
(
  boolean dual_standby_mode,
  sys_modem_dual_standby_pref_e_type dual_standby_mode_info,
  sys_modem_device_mode_e_type   device_mode
)
{
  lte_rrc_ds_status_change_req_s  rrc_ds_state_req ;
  msgr_init_hdr(&rrc_ds_state_req.msg_hdr, MSGR_NAS_EMM, LTE_RRC_DS_STATUS_CHANGE_REQ) ;
  rrc_ds_state_req.dual_standby_mode = dual_standby_mode;
  rrc_ds_state_req.standby_mode = dual_standby_mode_info;
  rrc_ds_state_req.device_mode = device_mode;
  emm_msgr_send((msgr_hdr_s *)&rrc_ds_state_req, sizeof(lte_rrc_ds_status_change_req_s));
  MSG_HIGH_DS_0(MM_SUB, "=EMM= Sent LTE_RRC_DS_STATUS_CHANGE_REQ");
}
#endif

#if defined FEATURE_DUAL_SIM || defined FEATURE_1XSRLTE
/*===========================================================================

FUNCTION    EMM_SEND_RRC_TRM_PRIORITY

DESCRIPTION
  This function sends the TRM priority to LRRC 

  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/

void emm_send_rrc_trm_priority
(
  lte_trm_priority_e trm_priority
)
{
  lte_dsds_trm_priority_req_s  rrc_dsds_trm_priority_req ;

  msgr_init_hdr(&rrc_dsds_trm_priority_req.msg_hdr, MSGR_NAS_EMM, LTE_RRC_DSDS_TRM_PRIORITY_REQ) ;

  rrc_dsds_trm_priority_req.client_id = LTE_TRM_PRIORITY_REQ_FROM_NAS;
  rrc_dsds_trm_priority_req.trm_priority = trm_priority;

  emm_msgr_send((msgr_hdr_s *)&rrc_dsds_trm_priority_req, sizeof(lte_dsds_trm_priority_req_s));
  MSG_HIGH_DS_1(MM_SUB, "=EMM= Sent LTE_RRC_DSDS_TRM_PRIORITY_REQ,  TRM Priority = %d",
                  trm_priority);
}


#endif /*FEATURE_DUAL_SIM */

/*===========================================================================
FUNCTION    EMM_SEND_RRC_EDRX_UPDATE

DESCRIPTION
  This function sends LTE_RRC_EDRX_CFG_REQ to RRC

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_send_rrc_edrx_update
(
  emm_ctrl_data_type *emm_ctrl_data_ptr,
  boolean            valid
)
{
  lte_rrc_edrx_cfg_req_s   rrc_edrx_cfg_req;
  byte edrx_params;

  msgr_init_hdr(&rrc_edrx_cfg_req.msg_hdr, MSGR_NAS_EMM, 
                LTE_RRC_EDRX_CFG_REQ);

  edrx_params = (byte)((emm_ctrl_data_ptr->edrx_params.ptw << 4) | 
                            emm_ctrl_data_ptr->edrx_params.cycle_length);

  rrc_edrx_cfg_req.edrx_params_valid = valid;
  rrc_edrx_cfg_req.edrx_params  = edrx_params;

  emm_msgr_send((msgr_hdr_s *)&rrc_edrx_cfg_req, sizeof(lte_rrc_edrx_cfg_req_s));
  MSG_HIGH_DS_2(MM_SUB, "=EMM= Sent LTE_RRC_EDRX_CFG_REQ. edrx_params_valid = %d, edrx_params = 0x%x",
                rrc_edrx_cfg_req.edrx_params_valid,
                edrx_params);
} /* end of emm_send_rrc_edrx_update() */

#ifdef FEATURE_WWAN_GNSS_DYNAMIC
/*===========================================================================
FUNCTION    EMM_SEND_RRC_WWAN_SET_PRIORITY

DESCRIPTION
  This function sends LTE_RRC_WWAN_SET_PRIORITY_REQ to RRC

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_send_rrc_wwan_set_priority
(
  boolean             high_priority
)
{
  lte_rrc_wwan_set_priority_req_s   rrc_wwan_set_priority_req;

  MSG_HIGH_DS_1(MM_SUB, "=EMM= Sent LTE_RRC_WWAN_SET_PRIORITY_REQ. high_priority  = %d ",
                high_priority);
				
  if (high_priority == TRUE)
  {
	dog_hb_report(DOG_MM_RPT);
	lte_rrc_send_receive_wwan_high_priority_req();
  }
  else
  {
    msgr_init_hdr(&rrc_wwan_set_priority_req.msg_hdr, MSGR_NAS_EMM, 
                   LTE_RRC_WWAN_SET_PRIORITY_REQ);

    rrc_wwan_set_priority_req.high_priority = high_priority;

    emm_msgr_send((msgr_hdr_s *)&rrc_wwan_set_priority_req, sizeof(lte_rrc_wwan_set_priority_req_s));
  }

} /* end of emm_send_rrc_wwan_set_priority() */
#endif

#endif /*FEATURE_LTE*/
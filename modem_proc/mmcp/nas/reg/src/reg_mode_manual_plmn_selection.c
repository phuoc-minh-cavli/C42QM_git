/*==============================================================================

                             COPYRIGHT INFORMATION

Copyright (c) 2002-2011 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/reg/vcs/reg_mode_manual_plmn_selection.c_v   1.2   06 Jun 2002 10:49:00   kabraham  $
$Header: //components/rel/mmcp.mpss/6.1.10/nas/reg/src/reg_mode_manual_plmn_selection.c#1 $ $DateTime: 2023/04/25 14:39:18 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
02/13/02   kwa     Initial release
05/29/02   kwa     Changes to support sending a single RAT rather than a mode
                   preference in the mmr_reg_req message.
05/31/02   kwa     Added RPLMN in call to save equivalent PLMN list.
06/27/02   kwa     Modified function that sends an mmr_reg_req due to
                   interface change.
10/18/02   kwa     Added FEATURE_REG_DEEP_SLEEP.
10/28/02   kwa     Added call to reg_send_mmr_stop_mode_req with reason
                   SYS_STOP_MODE_REASON_DEEP_SLEEP.
11/02/02   kwa     Added code to send cm_deep_sleep_ind only after the plmn
                   search period timer has reached its maximum value.
05/08/03   kwa     Added call to reg_mode_clear_attempted_plmn_list when
                   starting manual PLMN selection.  Fixes a bug where REG
                   failed to search both access technologies.
05/21/03   kwa     Restructured code to eliminate lint warnings.
06/04/03   kwa     Fixed one remaining lint warning.
09/08/03   kwa     Removed FEATURE_REG_DEEP_SLEEP.
09/18/03   kwa     Added support for band preferences.
10/22/03   kwa     Removed the parameters update_equivalent_plmn_list and
                   equivalent_plmn_list_p from the function finished.
11/16/03   kwa     Replaced call to reg_mode_initial_rat with a call to
                   reg_mode_initial_manual_rat in start_manual_plmn_selection.
                   This change should allow the phone to camp more quickly on
                   the manually selected PLMN.
11/19/03   kwa     Removed obsolete function for retrieving the mode name.
01/20/04   kwa     Added unexpected function handler
                   reg_mode_unexpected_start_plmn_selection_on_specified_plmn.
02/04/04   kwa     Added support for PS follow on request.
05/16/04   kwa     Added support for CS follow on request.
05/20/04   kwa     Removed calls to reg_mode_req_plmn_set since this function
                   is now called in reg_mode_update_attempted_plmn_list.  Added
                   code to resend an MMR_REG_REQ if an MMR_REG_CNF message was
                   received for the wrong RAT.
07/13/04   kwa     Added code to only resend an MMR_REG_REQ if an MMR_REG_CNF
                   message was received for the wrong RAT when the phone is
                   operating in single rather than dual mode.
02/17/05   up      Modified start_manual_plmn_selection to add a new
                   argument of sys_network_selection_mode_e_type to update
                   n/w selection mode dynamically by the caller. Updated
                   'finished' to consider LIMITED SERVICE in n/w selection mode
                   LIMITED_MANUAL_ACQUISITION as success.
02/20/06   sn      EHPLMN Feature
10/12/06   sn      WCDMA_1X mode preference implementation.
09/07/11   jbk      Start recovery from RPLMN AcT if we are in service on user selected plmn 
==============================================================================*/

/*==============================================================================

                            INCLUDE FILES FOR MODULE

==============================================================================*/

#include "mmcp_variation.h"
#include "customer.h"
#include "reg_mode.h"
#include "reg_send.h"
#include "reg_timers.h"
#include "reg_sim_v.h"
#include "sys_eplmn_list_v.h"
#include "reg_nv.h"
#include "reg_state.h"
#include <stringl/stringl.h> //FRACQ
/*==============================================================================

                                TYPE DEFINITIONS

==============================================================================*/

/*==============================================================================

                            STATIC DATA DECLARATIONS

==============================================================================*/
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE 
static boolean reg_mode_manual_plmn_selection_first_attempt_sim[MAX_NAS_STACKS];
#define reg_mode_manual_plmn_selection_first_attempt \
        reg_mode_manual_plmn_selection_first_attempt_sim[reg_as_id]
#else

static boolean reg_mode_manual_plmn_selection_first_attempt;

#endif /* FEATURE_DUAL_SIM ||  FEATURE_SGLTE */

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE 
#define sys_eplmn_list_equivalent_plmn(a) sys_eplmn_per_nas_stacks_list_equivalent_plmn(a, (sys_modem_as_id_e_type)reg_sub_id, (sys_modem_as_id_e_type)reg_as_id) 
#define reg_state_is_pwr_on_manual_plmn_selection reg_state_is_pwr_on_manual_plmn_selection_sim[reg_as_id]
#endif

/*==============================================================================

                          STATIC FUNCTION DEFINITIONS

==============================================================================*/


/*==============================================================================

FUNCTION NAME

  start_plmn_selection_on_specified_plmn

==============================================================================*/


void start_manual_plmn_selection_on_specified_plmn
(
  sys_plmn_id_s_type                   plmn,
  sys_network_selection_mode_e_type    network_selection_mode,
  boolean                              cs_for_request,
  boolean                              ps_for_request,
  sys_srv_domain_e_type                req_service_domain,
#ifdef FEATURE_RAT_PRIORITY_LIST
  const sys_rat_pri_list_info_s_type  *rat_pri_list_info,
#endif
  sys_radio_access_tech_e_type         active_rat
#ifdef FEATURE_LTE
  , boolean sms_only,
    uint32  additional_info
#endif
 , mmr_reg_req_e_type reg_req_type 
)
{
  sys_radio_access_tech_e_type initial_rat;
#ifdef FEATURE_CIOT
  mmr_reg_interlace_scan_s_type    scan_info; //FRACQ
#endif
  sys_srv_domain_e_type        allowed_service_domain;

#ifdef FEATURE_RAT_PRIORITY_LIST
  reg_mode_3gpp_capability_s_type rat_search_order;
#endif

  reg_mode_clear_attempted_plmn_list();
#ifdef FEATURE_CIOT
  scan_info.new_scan  = TRUE;
  scan_info.use_timer = FALSE;
  scan_info.scan_time = 0;//FRACQ
   scan_info.nb1_acqdb_timer_status = 0;
  scan_info.nb1_fullband_timer_status= 0;
#endif
  allowed_service_domain = reg_mode_allowed_service_domain
  (
    plmn,                    /* plmn                   */
    req_service_domain       /* req_service_domain     */
  );

  /*
  ** Attempt to acquire service on the specified PLMN if possible.
  */
  if ((!sys_plmn_id_is_undefined(plmn)) &&
      (reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_MANUAL) &&
      (allowed_service_domain != SYS_SRV_DOMAIN_NO_SRV))
  {
    initial_rat = active_rat;
#ifndef FEATURE_RAT_PRIORITY_LIST
    reg_mode_reset_available_plmn_list
    (
      plmn,
      reg_mode_mode_pref_get()
    );
#else
    reg_mode_get_3gpp_rat_capabilty(&rat_search_order, rat_pri_list_info
#ifdef FEATURE_SGLTE
      ,TRUE
#endif                                  
     );
       
    if(!reg_mode_is_rat_present_in_rat_pri_list(active_rat, rat_pri_list_info, TRUE))
    {
       initial_rat = rat_search_order.rat_list[0];
    }
  
    reg_mode_reset_available_plmn_list
    (
      plmn,
      &rat_search_order
    );
#endif /*FEATURE_RAT_PRIORITY_LIST*/
    reg_mode_update_attempted_plmn_list
    (
      plmn,
      initial_rat
    );

    reg_send_mmr_reg_req
    (
      network_selection_mode,  /* network_selection_mode */
      plmn,                    /* plmn                   */
      allowed_service_domain,  /* req_service_domain     */
      reg_mode_req_mode_pref_get(),  /* mode_pref              */
      cs_for_request,          /* cs_for_request         */
      ps_for_request,          /* ps_for_request         */
      initial_rat             /* rat                    */
#ifdef FEATURE_GSM_BAND_PREF
      ,reg_mode_band_pref_get()  /* band_pref              */
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
      ,rat_pri_list_info
#endif
      ,reg_req_type
#ifdef FEATURE_LTE
      , sms_only,
        additional_info
#endif
#ifdef FEATURE_FEMTO_CSG
      ,SYS_CSG_ID_INVALID
#endif 
	  ,scan_info

    );

    reg_mode_manual_plmn_selection_first_attempt = TRUE;
  }
  else
  {
    MSG_ERROR_DS_1( REG_SUB, "=REG= start_plmn_selection_on_specified_plmn (%d)",
             reg_mode_p);
  }
}


/*==============================================================================

FUNCTION NAME

  start_manual_plmn_selection

==============================================================================*/

void start_manual_plmn_selection
(
  sys_network_selection_mode_e_type    network_selection_mode,
  sys_plmn_id_s_type                   plmn,
  boolean                              cs_for_request,
  boolean                              ps_for_request,
  sys_srv_domain_e_type                req_service_domain
#ifdef FEATURE_RAT_PRIORITY_LIST
  ,const sys_rat_pri_list_info_s_type  *rat_pri_list_info
#endif
  ,reg_cm_service_req_e_type         type
#ifdef FEATURE_LTE
 ,boolean  sms_only,
  uint32   additional_info
#endif 
)
{
  sys_radio_access_tech_e_type initial_rat;
  sys_plmn_id_s_type plmn1;
#ifdef FEATURE_CIOT
  mmr_reg_interlace_scan_s_type    scan_info; //FRACQ
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
  reg_mode_3gpp_capability_s_type rat_priority_list;
  reg_mode_3gpp_capability_s_type effective_rat_priority_list;
#endif

#ifndef FEATURE_RAT_PRIORITY_LIST
    sys_mode_pref_e_type         hplmn_rat_search_order;
#else
    reg_mode_3gpp_capability_s_type  hplmn_rat_search_order;
#endif
  
#ifdef FEATURE_EQUIVALENT_HPLMN
        reg_sim_plmn_list_s_type     ehplmn_list;
#else
        sys_plmn_id_s_type           hplmn;
#endif
#ifdef FEATURE_DUAL_SIM
  boolean use_last_camped_rat = FALSE;
#endif
  

  if(type == REG_CM_SERVICE_REQ_SYNC_UP)
  {
#ifdef FEATURE_DUAL_SIM
    if(reg_state_acq_status_get() == SYS_ACQ_STATUS_NO_RESOURCE)
    {
      use_last_camped_rat = TRUE;
    }
#endif
    type = REG_CM_SERVICE_REQ_NORMAL;
  }

  sys_plmn_undefine_plmn_id(&plmn1);
  initial_rat = SYS_RAT_NONE;

 

#ifdef FEATURE_CIOT
  scan_info.new_scan  = TRUE;
  scan_info.use_timer = FALSE;
  scan_info.scan_time = 0;//FRACQ
   scan_info.nb1_acqdb_timer_status = 0;
  scan_info.nb1_fullband_timer_status= 0;
#endif
  reg_mode_clear_attempted_plmn_list();
  /*
  ** Attempt to acquire service on the specified PLMN.
  */
  if (!sys_plmn_id_is_undefined(plmn))
  {
    reg_timers_stop_hplmn_search_period_timer();
    
    if(type == REG_CM_SERVICE_REQ_PERIODIC)
    {
      if(sys_eplmn_list_equivalent_plmn(plmn) || 
         sys_plmn_match(plmn, reg_sim_read_rplmn(reg_mode_req_service_domain_get()) ) ) 
      {
        /*
        ** Get the next available equivalent RPLMN.
        */
        reg_mode_next_equivalent_rplmn
        (
          &plmn1,
          &initial_rat
        );
      }

      if(sys_plmn_id_is_undefined(plmn1))
      {
        reg_mode_next_available_plmn(&plmn1, &initial_rat);
      }
    }
    
    /* If plmn1 is undefined, then either type is other than PERIODIC or NO available PLMN found in type periodic. */
    if(sys_plmn_id_is_undefined(plmn1))
    {      
#ifndef FEATURE_RAT_PRIORITY_LIST
      /* REG needs to start Manual PLMN selection on current active 
      ** RAT if REG has FULL service on the same requested PLMN (from CM) */
      if((type != REG_CM_SERVICE_REQ_USER_SELECTION) &&
          (((reg_state_active_rat_get() == SYS_RAT_GSM_RADIO_ACCESS) &&
#ifdef FEATURE_UMTS_1X_HANDOVER_UMTSMSM
            #error code not present
#endif
            (reg_mode_mode_pref_get() != SYS_MODE_PREF_UMTS_RADIO_ACCESS_ONLY)) ||
           ((reg_state_active_rat_get() == SYS_RAT_UMTS_RADIO_ACCESS) &&
            (reg_mode_mode_pref_get() != SYS_MODE_PREF_GSM_RADIO_ACCESS_ONLY))
          ) &&
          sys_plmn_match(reg_state_service_plmn_get(),plmn) &&
         (reg_state_service_status_get() == SYS_SRV_STATUS_SRV))
      {
        initial_rat = reg_state_active_rat_get();
      }
      else
      {
        initial_rat = reg_mode_initial_manual_rat(plmn, mode_pref);
      }
      reg_mode_reset_available_plmn_list
      (
        plmn,
        mode_pref
      );
#else
      reg_mode_get_3gpp_rat_capabilty(&rat_priority_list, rat_pri_list_info
#ifdef FEATURE_SGLTE
      ,TRUE
#endif                                  
     );
     memscpy(&effective_rat_priority_list, sizeof(reg_mode_3gpp_capability_s_type), &rat_priority_list, sizeof(reg_mode_3gpp_capability_s_type));


#if defined(FEATURE_LTE) || defined(FEATURE_WRLF_SYSTEM_SEL)
      if(type == REG_CM_SERVICE_REQ_SUITABLE_SEARCH)
      {
        if (reg_mode_is_rat_present_in_rat_pri_list(reg_state_rlf_rat_get(), rat_pri_list_info, TRUE))
        {
          initial_rat = reg_state_rlf_rat_get();
        }
        else
        {
          initial_rat = reg_mode_initial_manual_rat(plmn,rat_pri_list_info);
        }
      }
      else 
#endif 
#ifdef FEATURE_DUAL_SIM
      if(((type == REG_CM_SERVICE_REQ_LAST_CAMPED_RAT) ||
           (use_last_camped_rat == TRUE))&&
         (sys_plmn_match(reg_sim_read_rplmn(reg_mode_req_service_domain_get()),plmn)))
      {
        initial_rat = reg_sim_read_rplmn_rat();
        reg_sim_read_rplmn_rat_search_order(&rat_priority_list);
        if((reg_mode_is_rat_present_in_rat_pri_list(initial_rat,rat_pri_list_info,TRUE) == FALSE)
#ifdef FEATURE_SGLTE
            || (REG_SGLTE_DUAL_STACK &&(reg_as_id == reg_ps_stack_id)&&(initial_rat == SYS_RAT_GSM_RADIO_ACCESS))
#endif
           )
        {
          initial_rat = rat_priority_list.rat_list[0];
        }
      }
      else
#endif
#ifdef FEATURE_NAS_GW
      if(type == REG_CM_SERVICE_REQ_USER_PREFERRED_PLMN_RAT)
      {
         initial_rat = reg_state_user_resel_get_rat();
         if((reg_mode_is_rat_present_in_rat_pri_list(initial_rat,rat_pri_list_info,TRUE) == FALSE)
#ifdef FEATURE_SGLTE
            || (REG_SGLTE_DUAL_STACK &&(reg_as_id == reg_ps_stack_id)&&(initial_rat == SYS_RAT_GSM_RADIO_ACCESS))
#endif
           )
         {
           initial_rat = reg_mode_initial_manual_rat(plmn,rat_pri_list_info);
         }
      }
#endif 
#ifdef FEATURE_NAS_GW
      else if((type == REG_CM_SERVICE_REQ_CAMPED_PLMN) && 
         (reg_state_plmn_reg_type_get() != SYS_REG_DOMAIN_NO_SERVICE) &&
         (reg_state_plmn_reg_type_get() != SYS_REG_DOMAIN_NO_SERVICE))
      {
        if(reg_mode_is_rat_present_in_rat_pri_list(reg_state_active_rat_get(), 
                                                    rat_pri_list_info, TRUE))
        {
          initial_rat = reg_state_active_rat_get();
        }
        else
        {
          initial_rat = reg_mode_initial_manual_rat(plmn,rat_pri_list_info);
        }
      }
      else
#endif 
#if defined(FEATURE_LTE) && defined(FEATURE_LTE_REL9) && !defined(FEATURE_NAS_IMS_VOICE_DISABLED)
      if (type == REG_CM_SERVICE_REQ_SRV_MODE_UPDATE)
      {
        if((reg_bit_mask_check(additional_info,
                         (byte)SYS_ADDTNL_INFO_EMERG_CALL) == TRUE)
           &&
           (reg_state_is_active_rat_lte(reg_state_active_rat_get()))
           &&
           (reg_state_service_status_get() == SYS_SRV_STATUS_LIMITED)
           &&
           (reg_mode_is_rat_present_in_rat_pri_list(
                 reg_state_active_rat_get(), rat_pri_list_info, TRUE) == TRUE))
        {
          initial_rat = reg_state_active_rat_get();
        }
        else
        {
          initial_rat = reg_mode_initial_manual_rat(plmn,rat_pri_list_info);
          type = REG_CM_SERVICE_REQ_NORMAL;
        }
      }
      else
#endif
      if((type != REG_CM_SERVICE_REQ_USER_SELECTION) &&
              (reg_mode_is_rat_present_in_rat_pri_list(reg_state_active_rat_get(),
                                                       rat_pri_list_info,TRUE)) &&
              (sys_plmn_match(reg_state_service_plmn_get(),plmn)) &&
         (reg_state_service_status_get() == SYS_SRV_STATUS_SRV))
      {
        initial_rat = reg_state_active_rat_get();
      }
      else if(type == REG_CM_SERVICE_REQ_NORMAL && sys_plmn_match(reg_sim_read_rplmn(reg_mode_req_service_domain_get()),plmn))
      {
#ifdef FEATURE_NAS_GW
        reg_sim_read_rplmn_rat_search_order(&rat_priority_list);
#endif
        initial_rat = rat_priority_list.rat_list[0];
      }
	  else if(type == REG_CM_SERVICE_REQ_SYSTEM_CHANGE)
      {
        initial_rat = rat_priority_list.rat_list[0];
      }
      else
      {
        initial_rat = reg_mode_initial_manual_rat(plmn,rat_pri_list_info);
      }
#ifdef FEATURE_LTE
      //If PLMN is backoff forbidden then adjust rat list to move LTE to last.
      reg_mode_adjust_rat_order_plmn_backoff_forbidden(&plmn,&rat_priority_list);
      if((reg_state_is_active_rat_lte(initial_rat)) &&
         (reg_sim_plmn_backoff_forbidden(plmn,(sys_modem_as_id_e_type)reg_sub_id)))
      {
        initial_rat = rat_priority_list.rat_list[0];
      }
#endif
      reg_mode_combine_effective_rat_list(initial_rat,&effective_rat_priority_list);

      reg_mode_reset_available_plmn_list(plmn, &rat_priority_list);
#endif
    }
    else
    {
      /* If plmn1 is defined, then set the value of plmn to plmn1 */
      plmn = plmn1;
    }

	 reg_mode_set_effective_acq_db_rat_list(&effective_rat_priority_list);
	 reg_mode_set_initial_full_band_plmn(plmn, initial_rat);
	
	if ((reg_mode_scan_scope_get() == SYS_SCAN_SCOPE_FULL_BAND) &&
	    (cs_for_request == FALSE)&&
		(SYS_RAT_LTE_NB1_RADIO_ACCESS == initial_rat)&& 
		(reg_nv_interlace_scan_timer_value_get() != INTERLACE_SCAN_DISABLED) &&
		 (type != REG_CM_SERVICE_REQ_PERIODIC))
	{
		 /* Create the acq db rat list and enable all the RATs (L/W/G/T) */
		
		 reg_mode_set_interleave_acq_db_rat_list();

		 

		 scan_info.use_timer = TRUE;
		 scan_info.scan_time = (uint32)reg_nv_interlace_scan_timer_value_get();
		 scan_info.nb1_acqdb_timer_status = reg_is_timer_active(REG_CIOT_NB1_ACQDB_TIER3_TIMER);
                 scan_info.nb1_fullband_timer_status= reg_is_timer_active(REG_CIOT_NB1_FULLBAND_TIER3_TIMER);
	
	}
	else
	{
		 reg_mode_clear_interleave_acq_db_rat();
	}

    reg_mode_update_attempted_plmn_list
    (
      plmn,
      initial_rat
    );

    reg_send_mmr_reg_req
    (
      network_selection_mode, /* network_selection_mode */
      plmn,                              /* plmn                   */
      req_service_domain,                /* req_service_domain     */
      reg_mode_req_mode_pref_get(),      /* mode_pref              */
      cs_for_request,                    /* cs_for_request         */
      ps_for_request,                    /* ps_for_request         */
      initial_rat                       /* rat                    */
#ifdef FEATURE_GSM_BAND_PREF
      ,reg_mode_band_pref_get()         /* band_pref              */
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
      ,rat_pri_list_info
#endif
      ,reg_mode_convert_cm_req_type_to_mm(type, network_selection_mode)
#ifdef FEATURE_LTE
     , sms_only,
       additional_info
#endif 
#ifdef FEATURE_FEMTO_CSG
     ,SYS_CSG_ID_INVALID
#endif 
	 ,scan_info
    );

    reg_mode_manual_plmn_selection_first_attempt = TRUE;
  }
  else if((sys_plmn_id_is_undefined(plmn))
     && (reg_state_is_pwr_on_manual_plmn_selection == TRUE))
  {  
#ifdef FEATURE_EQUIVALENT_HPLMN
    ehplmn_list = reg_sim_ehplmn_list_get();
#else
    hplmn = reg_sim_read_hplmn();
#endif
            
#ifndef FEATURE_RAT_PRIORITY_LIST
    hplmn_rat_search_order = reg_sim_read_hplmn_rat_search_order
    (
      reg_mode_req_mode_pref_get()
    );
            
    initial_rat = reg_mode_initial_rat(hplmn_rat_search_order);
            
    reg_mode_reset_available_plmn_list
    (
#ifdef FEATURE_EQUIVALENT_HPLMN
      ehplmn_list.plmn[0],
#else
      hplmn,
#endif
      hplmn_rat_search_order
    );
#else
    reg_mode_get_3gpp_rat_capabilty(&hplmn_rat_search_order, rat_pri_list_info
#ifdef FEATURE_SGLTE
      ,TRUE
#endif                                  
     );
#ifdef FEATURE_NAS_GW
    reg_sim_read_hplmn_rat_search_order(&hplmn_rat_search_order);
#endif               
    initial_rat = hplmn_rat_search_order.rat_list[0];      
              
    reg_mode_reset_available_plmn_list
    (
#ifdef FEATURE_EQUIVALENT_HPLMN
       ehplmn_list.plmn[0],
#else
       hplmn,
#endif
       &hplmn_rat_search_order
    );
#endif /*ifndef FEATURE_RAT_PRIORITY_LIST*/
            
     reg_mode_set_effective_acq_db_rat_list(&hplmn_rat_search_order);
#ifdef FEATURE_EQUIVALENT_HPLMN
	 reg_mode_set_initial_full_band_plmn(ehplmn_list.plmn[0], initial_rat);
#else
	 reg_mode_set_initial_full_band_plmn(hplmn, initial_rat);
#endif

	 if ((reg_mode_scan_scope_get() == SYS_SCAN_SCOPE_FULL_BAND) &&
		   (cs_for_request == FALSE)&&
		   (reg_nv_interlace_scan_timer_value_get() != INTERLACE_SCAN_DISABLED) &&
		   (SYS_RAT_LTE_NB1_RADIO_ACCESS == initial_rat))
	  {
		 /* Create the acq db rat list and enable all the RATs (L/W/G/T) */
		 
		 reg_mode_set_interleave_acq_db_rat_list();
		 scan_info.use_timer = TRUE;
		 scan_info.scan_time = (uint32)reg_nv_interlace_scan_timer_value_get();
                 scan_info.nb1_acqdb_timer_status = reg_is_timer_active(REG_CIOT_NB1_ACQDB_TIER3_TIMER);
                 scan_info.nb1_fullband_timer_status= reg_is_timer_active(REG_CIOT_NB1_FULLBAND_TIER3_TIMER);
	  }
	  else
	  {
		 reg_mode_clear_interleave_acq_db_rat();
	  }

            
    reg_mode_update_attempted_plmn_list
    (
#ifdef FEATURE_EQUIVALENT_HPLMN
      ehplmn_list.plmn[0],
#else
      hplmn,
#endif
      initial_rat
    );

    reg_send_mmr_reg_req
    (
      network_selection_mode,  /* network_selection_mode */      
#ifdef FEATURE_EQUIVALENT_HPLMN
          ehplmn_list.plmn[0],
#else
             hplmn,                               /* plmn                   */
#endif                         /* plmn                   */
      req_service_domain,  /* req_service_domain     */
      reg_mode_req_mode_pref_get(),               /* mode_pref              */
      cs_for_request,          /* cs_for_request         */
      ps_for_request,          /* ps_for_request         */
      initial_rat             /* rat                    */
#ifdef FEATURE_GSM_BAND_PREF
      ,reg_mode_band_pref_get()                /* band_pref              */
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
      ,rat_pri_list_info
#endif
      ,reg_mode_convert_cm_req_type_to_mm(type, network_selection_mode)
#ifdef FEATURE_LTE
       , sms_only,
         additional_info
#endif 
#ifdef FEATURE_FEMTO_CSG
       ,SYS_CSG_ID_INVALID
#endif
	   ,scan_info
    );
  }
  /*
  ** The RPLMN stored on the SIM is undefined.
  */
  else
  {
    reg_mode_start_limited_service
    (
      reg_mode_network_selection_mode_get(),
      reg_mode_req_mode_pref_get(),
      FALSE,
      FALSE,
#ifdef FEATURE_GSM_BAND_PREF
      req_service_domain,
      reg_mode_band_pref_get()
#else
      req_service_domain
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
      ,rat_pri_list_info
#endif
     ,REG_CM_SERVICE_REQ_NONE
#ifdef FEATURE_LTE
     , sms_only,
       additional_info
#endif 
    );
  }
}


/*==============================================================================

FUNCTION NAME

  finished

==============================================================================*/

boolean finished_manual_selection
(
        sys_plmn_service_state_s_type  service_state,
  const sys_detailed_plmn_list_s_type* available_plmn_list_p
)
{
  sys_radio_access_tech_e_type rat;
  sys_plmn_id_s_type           plmn;
  #ifdef FEATURE_CIOT
  mmr_reg_interlace_scan_s_type    scan_info; //FRACQ
  #endif
  boolean                      success = FALSE;
  boolean                      finished_plmn_selection = FALSE;

  (void) available_plmn_list_p;

#ifdef FEATURE_CIOT
  scan_info.new_scan  = TRUE;
  scan_info.use_timer = FALSE;
  scan_info.scan_time = 0;//FRACQ
   scan_info.nb1_acqdb_timer_status = 0;
  scan_info.nb1_fullband_timer_status= 0;
#endif


  success = reg_mode_successful_registration
  (
    service_state.plmn,
    service_state.service_status,
    service_state.active_rat,
    service_state.plmn_reg_type
  );

  /*
  ** Registration is considered as success if n/w selection mode is 
  ** LIMITED_MANUAL_ACQUISITION and service_status is LIMITED.
  */
  if (!success)
  {
    success = (boolean) ((reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION) &&
                       (service_state.service_status == SYS_SRV_STATUS_LIMITED));
  }
    
  /*
  ** The MS is registered on the requested PLMN.
  */
  if (success)
  {
    reg_mode_clear_attempted_plmn_list();
    reg_mode_clear_available_plmn_list();
    
    finished_plmn_selection = TRUE;
    mm_clear_manual_mode_user_selection_proc_flags();
  
#ifdef FEATURE_TDSCDMA
     if (reg_nv_is_forced_irat_enabled())
     {
           /* Start the timer if roaming is off
           ** and PLMN meets criteria
           */
           if ((service_state.roaming_ind == SYS_ROAM_STATUS_OFF)&&
             ((service_state.active_rat == SYS_RAT_TDS_RADIO_ACCESS) ||
               (service_state.active_rat == SYS_RAT_GSM_RADIO_ACCESS)) &&
               reg_mode_is_usim_compliant() && 
             reg_mode_is_rat_present_in_rat_pri_list(SYS_RAT_LTE_RADIO_ACCESS, reg_mode_rat_pri_list_get(), FALSE))
           {
             reg_mode_start_appropriate_irat_timers();
           }
      }   
#endif
  }
  /*
  ** The MS is not registered on the requested PLMN.
  */
  else
  {


#ifdef FEATURE_CIOT

	   if ( (reg_send_last_reg_req_scan_scope_get() == SYS_SCAN_SCOPE_FULL_BAND) &&
#ifdef FEATURE_DUAL_SIM
		   (reg_mode_get_rat_search_done_flag(reg_mode_get_full_band_initial_rat(), (sys_modem_as_id_e_type)reg_as_id) == TRUE) 
#else
		   (reg_mode_get_rat_search_done_flag(reg_mode_get_full_band_initial_rat()) == TRUE)&&
#endif
		   (reg_mode_get_num_interleav_acq_db_list() > 0)

		 )
	  {
		   MSG_HIGH_DS_0(REG_SUB, "=REG= Full band scan finished");
		   reg_mode_clear_interleave_acq_db_rat();
		   reg_mode_reset_full_band_initial_rat();
		 
	  } //FRACQ
#endif
    /*
    ** If the MS is not on the requested RAT then retry one more time.
    */
#ifndef FEATURE_RAT_PRIORITY_LIST
    if ( (service_state.active_rat != reg_mode_req_rat_get()) &&
         ( (reg_mode_mode_pref_get() == SYS_MODE_PREF_GSM_RADIO_ACCESS_ONLY) ||
#ifdef FEATURE_UMTS_1X_HANDOVER_UMTSMSM
           #error code not present
#endif
           (reg_mode_mode_pref_get() == SYS_MODE_PREF_UMTS_RADIO_ACCESS_ONLY)) &&
         (reg_mode_manual_plmn_selection_first_attempt == TRUE) )               
    {
      reg_send_last_mmr_reg_req();

      reg_mode_manual_plmn_selection_first_attempt = FALSE;
    }
    else
#else /*FEATURE_RAT_PRIORITY_LIST*/
    if((reg_mode_manual_plmn_selection_first_attempt == TRUE)&&
       (service_state.active_rat != reg_mode_req_rat_get()) &&
       !(reg_mode_is_rat_present_in_rat_pri_list(service_state.active_rat,
                                            reg_mode_rat_pri_list_get(),TRUE)))
    {
      reg_send_last_mmr_reg_req();
      reg_mode_manual_plmn_selection_first_attempt = FALSE;
    }
#ifdef FEATURE_CIOT
    else if (reg_mode_get_num_interleav_acq_db_list() > 0)
    {
    /* Add the RAT back to top of the list if full band scan is not completed (currently only applicable to LTE)*/
	  if ( (reg_send_last_reg_req_scan_scope_get() == SYS_SCAN_SCOPE_FULL_BAND) &&
#ifdef FEATURE_DUAL_SIM
		   (reg_mode_get_rat_search_done_flag(reg_mode_get_full_band_initial_rat(), (sys_modem_as_id_e_type)reg_as_id) == FALSE) &&
#else
		   (reg_mode_get_rat_search_done_flag(reg_mode_get_full_band_initial_rat()) == FALSE)&&
#endif
		 (reg_mode_get_num_interleav_acq_db_list() > 0)

		 )
	  {
		 reg_mode_remove_from_attempted_plmn_list(reg_send_last_reg_req_plmn_get(),reg_mode_get_full_band_initial_rat());
		 reg_mode_remove_from_available_plmn_list(reg_send_last_reg_req_plmn_get(),reg_mode_get_full_band_initial_rat());
		  if ((reg_state_is_pwr_on_manual_plmn_selection) ||
          (!sys_plmn_id_is_undefined(reg_mode_req_plmn_get())&&
#ifdef FEATURE_EQUIVALENT_HPLMN
            reg_sim_is_ehplmn(reg_mode_req_plmn_get()))
#else
            sys_plmn_match(reg_sim_read_hplmn(), reg_mode_req_plmn_get()))
#endif
         )
        {
		 reg_mode_update_available_plmn_list
	      (
	        REG_MODE_AUTOMATIC_MODE_MANUAL,
#ifndef FEATURE_RAT_PRIORITY_LIST        
	        reg_mode_mode_pref_get(),
#else
	        reg_mode_rat_pri_list_get(),
#endif 
	        reg_mode_req_service_domain_get(),
	        available_plmn_list_p
	      );
         }
		 reg_mode_set_update_avaialble_pending(TRUE);
	  } //FRACQ
          reg_mode_perform_interlace_acq_db_scan();
	}
#endif
    else
#endif /*FEATURE_RAT_PRIORITY_LIST*/
    {

	  if(reg_mode_get_update_avaialble_pending() == TRUE)
      {
	    reg_mode_add_initial_plmn_avaialbe_plmn_list();
	    reg_mode_set_update_avaialble_pending(FALSE);
      }
      /*
      ** Get the next prioritized PLMN-RAT combination.
      */
      reg_mode_next_available_plmn
      (
        &plmn,
        &rat
      );
      if ((reg_state_is_pwr_on_manual_plmn_selection) ||
          (!sys_plmn_id_is_undefined(reg_mode_req_plmn_get())&&
#ifdef FEATURE_EQUIVALENT_HPLMN
            reg_sim_is_ehplmn(reg_mode_req_plmn_get()))
#else
            sys_plmn_match(reg_sim_read_hplmn(), reg_mode_req_plmn_get()))
#endif
         )   
      {

        reg_mode_update_available_plmn_list
        (
          REG_MODE_AUTOMATIC_MODE_MANUAL,
#ifndef FEATURE_RAT_PRIORITY_LIST        
          reg_mode_mode_pref_get(),
#else
          reg_mode_rat_pri_list_get(),
#endif 
          reg_mode_req_service_domain_get(),
          available_plmn_list_p
        ); 

        if (sys_plmn_id_is_undefined(plmn))
        {
          reg_mode_next_available_plmn
          (
            &plmn,
            &rat
          );
        }

        if (!sys_plmn_id_is_undefined(plmn)&&
#ifdef FEATURE_EQUIVALENT_HPLMN
            !reg_sim_is_ehplmn(plmn)
#else
            !sys_plmn_match(reg_sim_read_hplmn(), plmn)
#endif 
            &&
            !sys_plmn_match(plmn,reg_mode_req_plmn_get()))

        { 
          sys_plmn_undefine_plmn_id(&plmn);
        }
      }
      /*
      ** Attempt to acquire service on the PLMN.
      */
      if (!sys_plmn_id_is_undefined(plmn))
      {
	if ((reg_mode_scan_scope_get() == SYS_SCAN_SCOPE_FULL_BAND) &&
	     (reg_mode_cs_for_request_get() == FALSE) &&
		 (reg_nv_interlace_scan_timer_value_get() != INTERLACE_SCAN_DISABLED) &&
	     rat == SYS_RAT_LTE_NB1_RADIO_ACCESS)
	{
		  //Reset the ACQ DB RAT priority list
		  reg_mode_set_interleave_acq_db_rat_list();
		 
                  if (rat == reg_mode_get_full_band_initial_rat())
		  {
			scan_info.new_scan = FALSE;
		  }

		  reg_mode_set_initial_full_band_plmn(plmn,rat);
		  scan_info.use_timer = TRUE;
		  scan_info.scan_time = (uint32)reg_nv_interlace_scan_timer_value_get();
		  scan_info.nb1_acqdb_timer_status = reg_is_timer_active(REG_CIOT_NB1_ACQDB_TIER3_TIMER);
                  scan_info.nb1_fullband_timer_status= reg_is_timer_active(REG_CIOT_NB1_FULLBAND_TIER3_TIMER);
	}
	  
        reg_mode_update_attempted_plmn_list
        (
          plmn,
          rat
        );

        reg_send_mmr_reg_req
        (
          reg_mode_network_selection_mode_get(),  /* network_selection_mode */
          plmn,                               /* plmn                   */
          reg_mode_req_service_domain_get(),  /* req_service_domain     */
          reg_mode_mode_pref_get(),           /* mode_pref              */
          reg_mode_cs_for_request_get(),      /* cs_for_request         */
          reg_mode_ps_for_request_get(),      /* ps_for_request         */
          rat                                /* rat                    */
#ifdef FEATURE_GSM_BAND_PREF
          ,reg_mode_band_pref_get()            /* band_pref              */
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
          ,reg_mode_rat_pri_list_get()                /*ue_capabilty*/
#endif 
          ,MMR_REG_REQ_NORMAL
#ifdef FEATURE_LTE
          ,reg_mode_sms_only_get()
          ,reg_mode_additional_info_get()
#endif 
#ifdef FEATURE_FEMTO_CSG
          ,SYS_CSG_ID_INVALID
#endif 
		  ,scan_info
        );

        reg_mode_manual_plmn_selection_first_attempt = TRUE;
      }
      else
      {
        finished_plmn_selection = TRUE;

        reg_mode_manual_plmn_selection_first_attempt = FALSE;
        mm_clear_manual_mode_user_selection_proc_flags();
      }
    }
  }

  return finished_plmn_selection;
}



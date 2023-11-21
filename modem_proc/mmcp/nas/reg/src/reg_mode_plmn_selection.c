/*==============================================================================

                             COPYRIGHT INFORMATION

Copyright (c) 2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/reg/vcs/reg_mode_plmn_selection.c_v   1.2   06 Jun 2002 10:49:12   kabraham  $
$Header: //components/rel/mmcp.mpss/6.1.10/nas/reg/src/reg_mode_plmn_selection.c#1 $ $DateTime: 2023/04/25 14:39:18 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
02/13/02   kwa     Initial release
05/29/02   kwa     Changes to support sending a single RAT rather than a mode
                   preference in the mmr_reg_req message.
05/31/02   kwa     Added RPLMN in call to save equivalent PLMN list.
06/27/02   kwa     Modified function that sends an mmr_reg_req due to
                   interface change.
09/23/02   kwa     Added code to determine the allowed service domain.
10/18/02   kwa     Added FEATURE_REG_DEEP_SLEEP.
10/28/02   kwa     Added call to reg_send_mmr_stop_mode_req with reason
                   SYS_STOP_MODE_REASON_DEEP_SLEEP.
11/02/02   kwa     Added code to send cm_deep_sleep_ind only after the plmn
                   search period timer has reached its maximum value.
05/21/03   kwa     Restructured code to eliminate lint warnings.
06/04/03   kwa     Fixed one remaining lint warning.
09/07/03   kwa     Modified start_plmn_selection to handle changes to the
                   initial value of the HPLMN Search Period Timer.
09/08/03   kwa     Removed FEATURE_REG_DEEP_SLEEP.
09/18/03   kwa     Added support for band preferences.
10/15/03   kwa     Added check to prevent searching for service on the RPLMN if
                   the RPLMN is forbidden.
10/17/03   kwa     Fixed a bug that prevent REG from searching both RATs when
                   the phone is in manual mode and the RPLMN is undefined or
                   forbidden by calling reg_mode_start_manual_plmn_selection
                   to search for service on the HPLMN.
10/22/03   kwa     Removed the parameters update_equivalent_plmn_list and
                   equivalent_plmn_list_p from the function finished.
11/19/03   kwa     Removed obsolete function for retrieving the mode name.
01/20/04   kwa     Added start_plmn_selection_on_specified_plmn to
                   properly handle a location updating reject with cause #15.
02/04/04   kwa     Added support for PS follow on request.
05/16/04   kwa     Added support for CS follow on request.
05/20/04   kwa     Removed calls to reg_mode_req_plmn_set since this function
                   is now called in reg_mode_update_attempted_plmn_list.  Added
                   code to resend an MMR_REG_REQ if an MMR_REG_CNF message was
                   received for the wrong RAT.
07/13/04   kwa     Added code to only resend an MMR_REG_REQ if an MMR_REG_CNF
                   message was received for the wrong RAT when the phone is
                   operating in single rather than dual mode.
05/20/05   up      Updated start_plmn_selection_on_specified_plmn and
                   start_plmn_selection  to start LIMITED PLMN selection
                   when RPLMN is undefined in MANUAL mode.
07/07/05   up      Updated start_plmn_selection not to reset service domain
                   if HPLMN is in GPRS forbidden list (for GCF 12.2.1.5d).
12/02/05   up      Updated start_plmn_selection send req_service_domain
                   in MMR_REG_REQ instead of allowed_service_domain.
08/28/06   sn      Reject cause 15 handling change. Do not make UE single mode
                   on PLMN selection on specified plmn.
10/12/06   sn      WCDMA_1X mode preference implementation.
==============================================================================*/

/*==============================================================================

                            INCLUDE FILES FOR MODULE

==============================================================================*/

#include "mmcp_variation.h"
#include "customer.h"
#include "reg_mode.h"
#include "reg_send.h"
#include "reg_sim_v.h"
#include "reg_timers.h"
#include "reg_nv.h"
#include "sys_eplmn_list_v.h"
#include "reg_state.h"
#include "cm_v.h"
#include <stringl/stringl.h>

/*==============================================================================

                                TYPE DEFINITIONS

==============================================================================*/

/*==============================================================================

                            STATIC DATA DECLARATIONS

==============================================================================*/
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE 
static boolean reg_mode_plmn_selection_first_attempt_sim[MAX_NAS_STACKS];
#define reg_mode_plmn_selection_first_attempt reg_mode_plmn_selection_first_attempt_sim[reg_as_id]
#else
static boolean reg_mode_plmn_selection_first_attempt;
#endif /* FEATURE_DUAL_SIM || FEATURE_SGLTE */

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE 
#define sys_eplmn_list_equivalent_plmn(a) sys_eplmn_per_nas_stacks_list_equivalent_plmn(a, (sys_modem_as_id_e_type)reg_sub_id, (sys_modem_as_id_e_type)reg_as_id)

static reg_mode_automatic_mode_e_type reg_mode_plmn_selection_mode_sim[MAX_NAS_STACKS];
#define reg_mode_plmn_selection_mode reg_mode_plmn_selection_mode_sim[reg_as_id]

static mmr_reg_req_e_type reg_mode_plmn_selection_reg_req_type_sim[MAX_NAS_STACKS];
#define reg_mode_plmn_selection_reg_req_type reg_mode_plmn_selection_reg_req_type_sim[reg_as_id]

#define reg_state_is_pwr_on_manual_plmn_selection reg_state_is_pwr_on_manual_plmn_selection_sim[reg_as_id]
#else
static reg_mode_automatic_mode_e_type reg_mode_plmn_selection_mode;
static mmr_reg_req_e_type reg_mode_plmn_selection_reg_req_type;
#endif



/*==============================================================================

                          STATIC FUNCTION DEFINITIONS

==============================================================================*/

/*==============================================================================

FUNCTION NAME

  update_plmn_selection_mode

==============================================================================*/

void update_plmn_selection_mode (reg_mode_automatic_mode_e_type  plmn_selection_mode)
{
   reg_mode_plmn_selection_mode = plmn_selection_mode;
};

/*==============================================================================

FUNCTION NAME

  start_plmn_selection

==============================================================================*/

void start_normal_plmn_selection
(
  sys_network_selection_mode_e_type    network_selection_mode,
  boolean                              cs_for_request,
  boolean                              ps_for_request,
  sys_srv_domain_e_type                req_service_domain,
  boolean                              clear_hplmn_search_timer
#ifdef FEATURE_RAT_PRIORITY_LIST
  ,const sys_rat_pri_list_info_s_type  *rat_pri_list_info
#endif
  ,reg_cm_service_req_e_type        type
#ifdef FEATURE_LTE
  , boolean sms_only,
    uint32  additional_info
#endif
)
{
  sys_radio_access_tech_e_type initial_rat;
 #ifdef FEATURE_CIOT
   mmr_reg_interlace_scan_s_type    scan_info; //FRACQ
 #endif
  sys_plmn_id_s_type           rplmn;
  sys_plmn_id_s_type           plmn;
  sys_plmn_id_s_type           attempted_hplmn;
  uint16 i;
  sys_srv_domain_e_type        allowed_service_domain;

#ifndef FEATURE_RAT_PRIORITY_LIST
  sys_mode_pref_e_type         rplmn_rat_search_order;
#else
  reg_mode_3gpp_capability_s_type rplmn_rat_search_order;
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


  mmr_reg_req_e_type reg_req_type = MMR_REG_REQ_NORMAL;
#ifdef FEATURE_CIOT
   scan_info.new_scan  = TRUE;
  scan_info.use_timer = FALSE;
  scan_info.scan_time = 0; //FRACQ
  scan_info.nb1_acqdb_timer_status = 0;
  scan_info.nb1_fullband_timer_status= 0;
#endif


  reg_mode_plmn_selection_reg_req_type = MMR_REG_REQ_NORMAL;

#ifdef FEATURE_LTE_TO_1X
  if(type == REG_CM_SERVICE_REQ_MMSS_START_RPLMN)
  {
    reg_mode_plmn_selection_mode = REG_MODE_AUTOMATIC_MODE_MMSS_RPLMN;
  }
  else
#endif
  {
    reg_mode_plmn_selection_mode = REG_MODE_AUTOMATIC_MODE_NORMAL;
  }
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

  if (clear_hplmn_search_timer)
  {
    reg_timers_stop_hplmn_search_period_timer();
  }

  reg_mode_clear_attempted_plmn_list();

  rplmn = reg_sim_read_rplmn(req_service_domain);

  allowed_service_domain = reg_mode_allowed_service_domain
  (
    rplmn,                   /* plmn                   */
    req_service_domain       /* req_service_domain     */
  );

  sys_plmn_undefine_plmn_id(&plmn);
  initial_rat = SYS_RAT_NONE;
  
  /* If type is REG_CM_SERVICE_REQ_PERIODIC, do not delete available PLMN list and pick the next available equivalent PLMN 
  ** or next available PLMN 
  */
  if(type == REG_CM_SERVICE_REQ_PERIODIC)
  {
      /*
      ** Get the next available equivalent RPLMN.
      */
      reg_mode_next_equivalent_rplmn
      (
        &plmn,
        &initial_rat
      );
  
    if(sys_plmn_id_is_undefined(plmn))
    {
      /*
      ** Get the next prioritized PLMN.
      */
      reg_mode_next_available_plmn
      (
        &plmn,
        &initial_rat
      );
    }

    if(!sys_plmn_id_is_undefined(plmn))
    {
      allowed_service_domain = reg_mode_allowed_service_domain
      (
        plmn,                   /* plmn                   */
        req_service_domain       /* req_service_domain     */
      );
    }    
  }

#ifdef FEATURE_HDR_TO_LTE
  else if(type == REG_CM_SERVICE_REQ_IRAT_TO_LTE)
  {
    reg_mode_clear_available_plmn_list();
    plmn = reg_sim_read_hplmn();
    initial_rat = SYS_RAT_LTE_RADIO_ACCESS;
    reg_req_type = MMR_REG_REQ_IRAT_TO_LTE;
  }
#endif
#ifdef FEATURE_NAS_GW
  else if(type == REG_CM_SERVICE_REQ_EF_RAT_REFRESH)
  {
    reg_req_type = MMR_REG_REQ_EF_RAT_REFRESH;
  }
  else if (type == REG_CM_SERVICE_REQ_USER_PREFERRED_PLMN_RAT)
  {          
    reg_state_user_resel_plmn_rat_get(&plmn, &initial_rat);

    reg_mode_get_3gpp_rat_capabilty(&rplmn_rat_search_order, rat_pri_list_info
#ifdef FEATURE_SGLTE
      ,TRUE
#endif                                  
     );
#ifdef FEATURE_LTE  
  if (!reg_mode_is_only_req_rat_present_in_rat_pri_list_lte(rat_pri_list_info))
  {
    reg_mode_adjust_rat_order_plmn_backoff_forbidden(&plmn,&rplmn_rat_search_order);
  }  
#endif
    if((reg_mode_is_rat_present_in_rat_pri_list(initial_rat,rat_pri_list_info,TRUE) == FALSE)
#ifdef FEATURE_SGLTE
       || (REG_SGLTE_DUAL_STACK &&(reg_as_id == reg_ps_stack_id)&&(initial_rat == SYS_RAT_GSM_RADIO_ACCESS))
#endif 
#ifdef FEATURE_LTE
       ||((reg_state_is_active_rat_lte(initial_rat)) &&
          (reg_sim_plmn_rat_forbidden(plmn,SYS_RAT_LTE_RADIO_ACCESS,(sys_modem_as_id_e_type)reg_sub_id)))
#endif
     )
    {
      initial_rat = rplmn_rat_search_order.rat_list[0];
    }
    reg_mode_reset_available_plmn_list
    (
       plmn,
       &rplmn_rat_search_order
    );
  } 
#endif 
#ifdef FEATURE_DUAL_SIM
  else if((type == REG_CM_SERVICE_REQ_LAST_CAMPED_RAT) ||
          (use_last_camped_rat == TRUE))
  {
    
    initial_rat = reg_sim_read_rplmn_rat();
    plmn = rplmn;
    reg_mode_get_3gpp_rat_capabilty(&rplmn_rat_search_order, rat_pri_list_info
#ifdef FEATURE_SGLTE
      ,TRUE
#endif                                  
     );
#ifdef FEATURE_NAS_GW
     if((reg_mode_scan_scope_get() == SYS_SCAN_SCOPE_ACQ_DB) && 
        (type == REG_CM_SERVICE_REQ_LAST_CAMPED_RAT))
     {
       reg_mode_plmn_selection_mode = REG_MODE_AUTOMATIC_MODE_CAMPED_PLMN_RAT;
     }
#endif 
#ifdef FEATURE_NAS_GW
    reg_sim_read_rplmn_rat_search_order(&rplmn_rat_search_order);
#ifdef FEATURE_LTE    
    if (!reg_mode_is_only_req_rat_present_in_rat_pri_list_lte(rat_pri_list_info))
    {
      reg_mode_adjust_rat_order_plmn_backoff_forbidden(&plmn,&rplmn_rat_search_order);
    }    
#endif
#endif
    if((reg_mode_is_rat_present_in_rat_pri_list(initial_rat,rat_pri_list_info,TRUE) == FALSE)
#ifdef FEATURE_SGLTE
       || (REG_SGLTE_DUAL_STACK &&(reg_as_id == reg_ps_stack_id)&&(initial_rat == SYS_RAT_GSM_RADIO_ACCESS))
#endif
#ifdef FEATURE_LTE
       ||((reg_state_is_active_rat_lte(initial_rat)) &&
          (reg_sim_plmn_rat_forbidden(plmn,SYS_RAT_LTE_RADIO_ACCESS,(sys_modem_as_id_e_type)reg_sub_id)))
#endif
     )
    {
      initial_rat = rplmn_rat_search_order.rat_list[0];
    }
    reg_mode_reset_available_plmn_list
    (
       plmn,
       &rplmn_rat_search_order
    );
  }
#endif
  /*
  ** If plmn is undefined means either type is not periodic or available PLMN list is empty.
  ** So, attempt to acquire service on the RPLMN if possible.
  */
  if ((sys_plmn_id_is_undefined(plmn)) &&
      ((!sys_plmn_id_is_undefined(rplmn)) &&
#ifdef FEATURE_DUAL_SIM
       (!reg_sim_per_subs_plmn_forbidden(rplmn, (sys_modem_as_id_e_type)reg_sub_id)) &&
#else
       (!reg_sim_plmn_forbidden(rplmn)) &&
#endif
       (allowed_service_domain != SYS_SRV_DOMAIN_NO_SRV)))
  {
#ifndef FEATURE_RAT_PRIORITY_LIST
    rplmn_rat_search_order = reg_sim_read_rplmn_rat_search_order
    (
      reg_mode_req_mode_pref_get()
    );

    initial_rat = reg_mode_initial_rat(rplmn_rat_search_order);

    reg_mode_reset_available_plmn_list
    (
      rplmn,
      rplmn_rat_search_order
    );
    plmn = rplmn; /* Set plmn equal to rplmn */
#else   
    reg_mode_get_3gpp_rat_capabilty(&rplmn_rat_search_order, rat_pri_list_info
#ifdef FEATURE_SGLTE
      ,TRUE
#endif                                  
     );

    plmn = rplmn;
    if(reg_state_get_power_up_system_selection() == TRUE) 
    {
      reg_mode_determine_first_plmn(&plmn, &rplmn_rat_search_order, rat_pri_list_info);
    }
#if defined(FEATURE_NAS_GW)||defined(FEATURE_CIOT)
    if(sys_plmn_match(plmn, rplmn))
    {
      reg_sim_read_rplmn_rat_search_order(&rplmn_rat_search_order);
    }

#ifdef FEATURE_LTE   
   /* If there is no other rats than LTE, do not reshuffle  */
  if (!reg_mode_is_only_req_rat_present_in_rat_pri_list_lte(rat_pri_list_info))
  {
    reg_mode_adjust_rat_order_plmn_backoff_forbidden(&plmn,&rplmn_rat_search_order);
  }
#endif
#endif    
    initial_rat = rplmn_rat_search_order.rat_list[0];  

	reg_mode_set_effective_acq_db_rat_list(&rplmn_rat_search_order);
    reg_mode_set_initial_full_band_plmn(plmn, initial_rat);
	

    if ((reg_mode_scan_scope_get() == SYS_SCAN_SCOPE_FULL_BAND) &&
       (cs_for_request == FALSE) &&
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
    if(!((reg_state_get_power_up_system_selection() == TRUE)&&(reg_mode_get_available_plmn_list_populated() == TRUE)))	
    {
      reg_mode_reset_available_plmn_list
      (
        plmn,
        &rplmn_rat_search_order
      );
    }
#endif  /*FEATURE_RAT_PRIORITY_LIST*/

    
  }
  
  /* If plmn is defined (either plmn from available list or rplmn, and allowed service domain is not no_service then 
  ** send mmr_reg_req on this plmn.
  */
  if((!sys_plmn_id_is_undefined(plmn)) &&
     (allowed_service_domain != SYS_SRV_DOMAIN_NO_SRV))
  {
    reg_mode_update_attempted_plmn_list
    (
      plmn,
      initial_rat
    );

    reg_send_mmr_reg_req
    (
      network_selection_mode,  /* network_selection_mode */
      plmn,                   /* plmn                   */
      req_service_domain,  /* req_service_domain     */
      reg_mode_req_mode_pref_get(),    /* mode_pref              */
      cs_for_request,          /* cs_for_request         */
      ps_for_request,          /* ps_for_request         */
      initial_rat             /* rat                    */
#ifdef FEATURE_GSM_BAND_PREF
      ,reg_mode_band_pref_get()     /* band_pref              */
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
      ,rat_pri_list_info
#endif
      ,reg_req_type
#ifdef FEATURE_LTE
      ,sms_only,
       additional_info
#endif
#ifdef FEATURE_FEMTO_CSG
      ,SYS_CSG_ID_INVALID
#endif 
#ifdef FEATURE_CIOT
  ,scan_info //FRACQ
#endif
    );
    reg_mode_plmn_selection_first_attempt = TRUE;
  }
  /*
  ** The RPLMN stored on the SIM is undefined or the requested
  ** service is not allowed on the RPLMN.
  */
  else
  {
    if (network_selection_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC
#ifdef FEATURE_1XSRLTE
        ||
        network_selection_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC_CAMP_ONLY
#endif
        )
    {
      /*
      **  If service request was of type MMSS RPLMN then set reg mode type to MMSS HPLMN
      **  as we are starting automatic mode PLMN selection.
      */
      reg_mode_automatic_mode_e_type automatic_mode_type;
      if(type == REG_CM_SERVICE_REQ_MMSS_START_RPLMN)
      {
        automatic_mode_type = REG_MODE_AUTOMATIC_MODE_MMSS_HPLMN;
      }
      else
      {
        automatic_mode_type = reg_mode_plmn_selection_mode;
      }
      reg_mode_start_automatic_plmn_selection
      (
        network_selection_mode,
        automatic_mode_type,
        reg_mode_req_mode_pref_get(),
        cs_for_request,
        ps_for_request,
        req_service_domain
#ifdef FEATURE_GSM_BAND_PREF
        ,reg_mode_band_pref_get()
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
        ,rat_pri_list_info
#endif
#ifdef FEATURE_LTE
       ,sms_only,
        additional_info
#endif
      );
    }
 
    /*
    ** The RPLMN stored on the SIM is undefined. 
    ** Camp on LIMITED service in MANUAL mode.
    */
   else if((sys_plmn_id_is_undefined(plmn))
       && (reg_state_is_pwr_on_manual_plmn_selection == TRUE) 
       && (allowed_service_domain != SYS_SRV_DOMAIN_NO_SRV))
    {  
#ifdef FEATURE_EQUIVALENT_HPLMN
      ehplmn_list = reg_sim_ehplmn_list_get();
      attempted_hplmn = ehplmn_list.plmn[0];
#else
      hplmn = reg_sim_read_hplmn();
      attempted_hplmn = hplmn;
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
      /*If G is present adjust backoff list if any plmn is backoff forbidden*/
#ifdef FEATURE_LTE  
     if (!reg_mode_is_only_req_rat_present_in_rat_pri_list_lte(rat_pri_list_info))
     {
        reg_mode_adjust_rat_order_plmn_backoff_forbidden( &attempted_hplmn,&hplmn_rat_search_order); 
      reg_mode_reset_available_plmn_list
      (
         attempted_hplmn,
         &hplmn_rat_search_order
        );
        if (reg_sim_plmn_rat_forbidden(attempted_hplmn,SYS_RAT_LTE_NB1_RADIO_ACCESS,(sys_modem_as_id_e_type)reg_sub_id))
        {
          reg_mode_remove_from_available_plmn_list(attempted_hplmn,SYS_RAT_LTE_M1_RADIO_ACCESS);
          reg_mode_remove_from_available_plmn_list(attempted_hplmn,SYS_RAT_LTE_NB1_RADIO_ACCESS);
        }
     }
     else
     {
       /*get first non blocked hplmn, in LTE only rat list*/
#ifdef FEATURE_EQUIVALENT_HPLMN
       for (i = 0; i <ehplmn_list.length; ++i )
       {
        if (!reg_sim_plmn_rat_forbidden(ehplmn_list.plmn[i],SYS_RAT_LTE_NB1_RADIO_ACCESS,(sys_modem_as_id_e_type)reg_sub_id)) 
        {
          attempted_hplmn = ehplmn_list.plmn[i];
         break;
        }
       }
       
#endif
        reg_mode_reset_available_plmn_list
        (
         attempted_hplmn,
      &hplmn_rat_search_order
      );
      }
     initial_rat = hplmn_rat_search_order.rat_list[0];
#else

     initial_rat = hplmn_rat_search_order.rat_list[0];      
        
      reg_mode_reset_available_plmn_list
      (
       attempted_hplmn,
       &hplmn_rat_search_order
      );
#endif
       
#endif /*ifndef FEATURE_RAT_PRIORITY_LIST*/
    
      reg_mode_update_attempted_plmn_list
      (
        attempted_hplmn,
        initial_rat
      );

      reg_send_mmr_reg_req
      (
         network_selection_mode,  /* network_selection_mode */      
         attempted_hplmn,                   /* plmn                   */
         req_service_domain,  /* req_service_domain     */
         reg_mode_req_mode_pref_get(),     /* mode_pref              */
         cs_for_request,          /* cs_for_request         */
         ps_for_request,          /* ps_for_request         */
         initial_rat             /* rat                    */
#ifdef FEATURE_GSM_BAND_PREF
         ,reg_mode_band_pref_get()    /* band_pref              */
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
         ,rat_pri_list_info
#endif
         ,reg_req_type
#ifdef FEATURE_LTE
      ,sms_only,
       additional_info
#endif
#ifdef FEATURE_FEMTO_CSG
      ,SYS_CSG_ID_INVALID
#endif 
#ifdef FEATURE_CIOT
	  ,scan_info //FRACQ
#endif
       );
       reg_mode_plmn_selection_first_attempt = TRUE;
    }
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
       ,sms_only,
        additional_info
#endif
      );
    }    
    reg_mode_plmn_selection_first_attempt = FALSE;
  }
}

/*==============================================================================

FUNCTION NAME

  start_plmn_selection_roaming_plmn

==============================================================================*/

void start_auto_plmn_selection_roaming_plmn
(
  sys_network_selection_mode_e_type    network_selection_mode,
  boolean                              cs_for_request,
  boolean                              ps_for_request,
  sys_srv_domain_e_type                req_service_domain,
  boolean                              clear_hplmn_search_timer
#ifdef FEATURE_RAT_PRIORITY_LIST
  ,const sys_rat_pri_list_info_s_type  *rat_pri_list_info
#endif
  ,reg_cm_service_req_e_type        type
#ifdef FEATURE_LTE
  , boolean sms_only,
    uint32  additional_info
#endif
)
{
  sys_radio_access_tech_e_type initial_rat;
  #ifdef FEATURE_CIOT
    mmr_reg_interlace_scan_s_type    scan_info; //FRACQ
 #endif
  sys_plmn_id_s_type           rplmn;
  sys_plmn_id_s_type           plmn;

#ifndef FEATURE_RAT_PRIORITY_LIST
  sys_mode_pref_e_type         rplmn_rat_search_order;
#else
  reg_mode_3gpp_capability_s_type rplmn_rat_search_order;
#endif

  reg_mode_plmn_selection_reg_req_type = MMR_REG_REQ_NORMAL;
#ifdef FEATURE_CIOT
   scan_info.new_scan  = TRUE;
  scan_info.use_timer = FALSE;
  scan_info.scan_time = 0;//FRACQ
  scan_info.nb1_acqdb_timer_status = 0;
  scan_info.nb1_fullband_timer_status= 0;
#endif
  if (clear_hplmn_search_timer)
  {
    reg_timers_stop_hplmn_search_period_timer();
  }

  reg_mode_clear_attempted_plmn_list();

  rplmn = reg_sim_read_rplmn(req_service_domain);

  sys_plmn_undefine_plmn_id(&plmn);
  initial_rat = SYS_RAT_NONE;
  plmn = rplmn;

  reg_mode_plmn_selection_mode = REG_MODE_AUTOMATIC_MODE_ROAMING_PLMN;
  
  if(reg_mode_is_china_mcc(plmn) || 
#ifdef FEATURE_EQUIVALENT_HPLMN
      (reg_sim_is_ehplmn(plmn)))
#else
      (sys_plmn_match(reg_sim_read_hplmn(), plmn)))
#endif
  {
    reg_mode_start_foreground_plmn_search();
  }
  else
  {

#ifndef FEATURE_RAT_PRIORITY_LIST
  rplmn_rat_search_order = reg_sim_read_rplmn_rat_search_order
  (
    mode_pref
  );

  initial_rat = reg_mode_initial_rat(rplmn_rat_search_order);

  reg_mode_reset_available_plmn_list
  (
    rplmn,
    rplmn_rat_search_order
  );
#else
  reg_mode_get_3gpp_rat_capabilty(&rplmn_rat_search_order, rat_pri_list_info
#ifdef FEATURE_SGLTE
    ,TRUE
#endif                                  
     );
#ifdef FEATURE_NAS_GW
  reg_sim_read_rplmn_rat_search_order(&rplmn_rat_search_order);
#ifdef FEATURE_LTE
  reg_mode_adjust_rat_order_plmn_backoff_forbidden(&rplmn,&rplmn_rat_search_order);
#endif
#endif
  initial_rat = rplmn_rat_search_order.rat_list[0];        

  reg_mode_reset_available_plmn_list
  (
    rplmn,
    &rplmn_rat_search_order
  );
#endif  /*FEATURE_RAT_PRIORITY_LIST*/

  reg_mode_set_effective_acq_db_rat_list(&rplmn_rat_search_order);
  reg_mode_set_initial_full_band_plmn(plmn, initial_rat);



  if ((reg_mode_scan_scope_get() == SYS_SCAN_SCOPE_FULL_BAND) &&
			 (cs_for_request == FALSE) &&
			 (reg_nv_interlace_scan_timer_value_get() != INTERLACE_SCAN_DISABLED) &&
			 initial_rat == SYS_RAT_LTE_NB1_RADIO_ACCESS )
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
    network_selection_mode,              /* network_selection_mode */
    plmn,
    req_service_domain,                   /* req_service_domain     */
    reg_mode_req_mode_pref_get(),         /* mode_pref              */
    cs_for_request,                       /* cs_for_request         */
    ps_for_request,                       /* ps_for_request         */
    initial_rat                            /* rat                    */
#ifdef FEATURE_GSM_BAND_PREF
    ,reg_mode_band_pref_get()             /* band_pref              */
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
    ,rat_pri_list_info
#endif
    ,MMR_REG_REQ_NORMAL
#ifdef FEATURE_LTE
   , sms_only,
     additional_info
#endif 
#ifdef FEATURE_FEMTO_CSG
  ,SYS_CSG_ID_INVALID
#endif 
#ifdef FEATURE_CIOT
  ,scan_info //FRACQ
#endif
  );
  reg_mode_plmn_selection_first_attempt = TRUE;
  }
}
//FR15492


/*==============================================================================

FUNCTION NAME

  start_plmn_selection_on_specified_plmn

==============================================================================*/

void start_auto_plmn_selection_on_specified_plmn
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

  if(reg_req_type == MMR_REG_REQ_ACQ_HPLMN_OR_RPLMN_RAT_SEARCH_ORDER)
  {
    reg_mode_plmn_selection_reg_req_type = MMR_REG_REQ_NORMAL;
  }
  else
  {
    reg_mode_plmn_selection_reg_req_type = reg_req_type;
  }
  #ifdef FEATURE_CIOT
  scan_info.new_scan  = TRUE;
  scan_info.use_timer = FALSE;
  scan_info.scan_time = 0;//FRACQ
   scan_info.nb1_acqdb_timer_status = 0;
  scan_info.nb1_fullband_timer_status= 0;
  #endif
  reg_mode_clear_attempted_plmn_list();

  allowed_service_domain = reg_mode_allowed_service_domain
  (
    plmn,                    /* plmn                   */
    req_service_domain       /* req_service_domain     */
  );

  /*
  ** Attempt to acquire service on the specified PLMN if possible.
  */
  if ( (!sys_plmn_id_is_undefined(plmn)) &&
       ((
#ifdef FEATURE_DUAL_SIM
        (!reg_sim_per_subs_plmn_forbidden(plmn, (sys_modem_as_id_e_type)reg_sub_id))
#else
        (!reg_sim_plmn_forbidden(plmn))
#endif
        ) ||
        (reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_MANUAL)||
        (reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION)) &&
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

    /* NAS to follow HPLMN RAT search order while performing PLMN selection on receiving 
       reject cause #15 �No Suitable Cells In Location Area" on HPLMN. */
#ifdef FEATURE_NAS_GW
    if((reg_req_type == MMR_REG_REQ_ACQ_HPLMN_OR_RPLMN_RAT_SEARCH_ORDER) &&
#ifdef FEATURE_EQUIVALENT_HPLMN
      (reg_sim_is_ehplmn(plmn))
#else
      (sys_plmn_match(reg_sim_read_hplmn(), plmn))
#endif
          )
    {
      reg_sim_read_hplmn_rat_search_order(&rat_search_order);
    }
    else if((reg_req_type == MMR_REG_REQ_ACQ_HPLMN_OR_RPLMN_RAT_SEARCH_ORDER) &&
             sys_plmn_match(reg_sim_read_rplmn(req_service_domain),plmn))
    {
      reg_sim_read_rplmn_rat_search_order(&rat_search_order);
#ifdef FEATURE_LTE    
   if (!reg_mode_is_only_req_rat_present_in_rat_pri_list_lte(rat_pri_list_info))
   {
      reg_mode_adjust_rat_order_plmn_backoff_forbidden(&plmn,&rat_search_order);
   }
   
#endif
    }
#endif 
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
      reg_mode_mode_pref_get(),               /* mode_pref              */
      cs_for_request,          /* cs_for_request         */
      ps_for_request,          /* ps_for_request         */
      initial_rat             /* rat                    */
#ifdef FEATURE_GSM_BAND_PREF
      ,reg_mode_band_pref_get() /* band_pref              */
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
      ,rat_pri_list_info
#endif
      ,reg_mode_plmn_selection_reg_req_type
#ifdef FEATURE_LTE
      , sms_only,
        additional_info
#endif
#ifdef FEATURE_FEMTO_CSG
      ,SYS_CSG_ID_INVALID
#endif 
#ifdef FEATURE_CIOT
	  ,scan_info //FRACQ
#endif

    );
    reg_mode_plmn_selection_mode = REG_MODE_AUTOMATIC_MODE_NORMAL;

    reg_mode_plmn_selection_first_attempt = TRUE;
  }
  /*
  ** The requested PLMN is undefined or the requested service
  ** is not allowed on the PLMN.
  */
  else
  {
    if (network_selection_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC 
#ifdef FEATURE_1XSRLTE
       ||
        network_selection_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC_CAMP_ONLY
#endif 
       )
    {
      reg_mode_start_automatic_plmn_selection
      (
        network_selection_mode,
        REG_MODE_AUTOMATIC_MODE_NORMAL,
        reg_mode_req_mode_pref_get(),
        cs_for_request,
        ps_for_request,
#ifdef FEATURE_GSM_BAND_PREF
        req_service_domain,
        reg_mode_band_pref_get()
#else
        req_service_domain
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
        ,rat_pri_list_info
#endif
#ifdef FEATURE_LTE
        , sms_only,
          additional_info
#endif
      );
    }
    /*
    ** The PLMN or RPLMN stored on the SIM is undefined. 
    ** Camp on LIMITED service in MANUAL mode.
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

    reg_mode_plmn_selection_first_attempt = FALSE;
  }
}



/*==============================================================================

FUNCTION NAME

  finished

==============================================================================*/

boolean finished_normal_selection
(
        sys_plmn_service_state_s_type  service_state,
  const sys_detailed_plmn_list_s_type* available_plmn_list_p
)
{
  sys_radio_access_tech_e_type rat = SYS_RAT_GSM_RADIO_ACCESS;
  sys_srv_domain_e_type        allowed_service_domain;
  #ifdef FEATURE_CIOT
  mmr_reg_interlace_scan_s_type    scan_info; //FRACQ
  #endif
  sys_plmn_id_s_type           plmn;
  boolean                      success                 = FALSE;
  boolean                      finished_plmn_selection = FALSE;


#ifdef FEATURE_NAS_GW
#ifndef FEATURE_NAS_CS_DISABLED
  sys_detailed_plmn_list_info_s_type managed_roaming_plmn;
#endif 
#endif
  success = reg_mode_successful_registration
  (
    service_state.plmn,
    service_state.service_status,
    service_state.active_rat,
    service_state.plmn_reg_type
  );
#ifdef FEATURE_CIOT
  scan_info.new_scan  = TRUE;
  scan_info.use_timer = FALSE;
  scan_info.scan_time = 0;//FRACQ
   scan_info.nb1_acqdb_timer_status = 0;
  scan_info.nb1_fullband_timer_status= 0;
#endif
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
  ** The MS is registered on the RPLMN.
  */
  if (success)
  {
    if (reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_AUTOMATIC &&
        service_state.roaming_ind == SYS_ROAM_STATUS_ON)
    {
      reg_timers_start_hplmn_search_period_timer();
    }
#if defined (FEATURE_TDSCDMA) || defined (FEATURE_SGLTE)
    else
    {
        MSG_HIGH_DS_0(REG_SUB, "=REG= In reg_mode_rat_search_start"); 
        reg_mode_rat_search_start();
    }
#endif

    finished_plmn_selection = TRUE;
  }
  /*
  ** The MS is not registered on the RPLMN.
  */
  else
  {
   /* Set plmn selection mode to normal service ifservice status is no service as UE is no more in RLF

        For customer specific behivor ofr starting PLMN selection from HPLMN for reject cause #15. 
        Requested PLMN is set to undefined. If requested PLMN is undefined then also lower layers are out of RLF
  */
    if((service_state.service_status != SYS_SRV_STATUS_NO_SRV
#ifdef FEATURE_HPLMN_NOT_ALLOWED_SELECT_NEXT
          ||sys_plmn_id_is_undefined(reg_mode_req_plmn_get())
#endif 
       )
#ifdef FEATURE_LTE_TO_1X
    && reg_mode_plmn_selection_mode != REG_MODE_AUTOMATIC_MODE_MMSS_RPLMN
#endif 
        &&  reg_mode_plmn_selection_mode != REG_MODE_AUTOMATIC_MODE_ROAMING_PLMN)
    {
      reg_mode_plmn_selection_mode = REG_MODE_AUTOMATIC_MODE_NORMAL;     
    }


#ifdef FEATURE_CIOT

	   if ( (reg_send_last_reg_req_scan_scope_get() == SYS_SCAN_SCOPE_FULL_BAND) &&
#ifdef FEATURE_DUAL_SIM
		   (reg_mode_get_rat_search_done_flag(reg_mode_get_full_band_initial_rat(), (sys_modem_as_id_e_type)reg_as_id) == TRUE) &&
#else
		   (reg_mode_get_rat_search_done_flag(reg_mode_get_full_band_initial_rat()) == TRUE) &&
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
           (reg_mode_mode_pref_get() == SYS_MODE_PREF_UMTS_RADIO_ACCESS_ONLY) ) &&
         (reg_mode_plmn_selection_first_attempt == TRUE) )
    {
      reg_send_last_mmr_reg_req();

      reg_mode_plmn_selection_first_attempt = FALSE;
    }
    else
#else /*FEATURE_RAT_PRIORITY_LIST*/
    if((reg_mode_plmn_selection_first_attempt == TRUE) &&
       (service_state.active_rat != reg_mode_req_rat_get()) &&
       !(reg_mode_is_rat_present_in_rat_pri_list(service_state.active_rat,
                                            reg_mode_rat_pri_list_get(),TRUE)))
    {
      reg_send_last_mmr_reg_req();
      reg_mode_plmn_selection_first_attempt = FALSE;
    }    
    else if (reg_state_repeat_srv_req_get())
    {
        reg_send_last_mmr_reg_req();
		reg_state_reset_repeat_srv_req();
    }
#ifdef FEATURE_CIOT
	else if (reg_mode_get_num_interleav_acq_db_list() > 0)
    {
        /* Add the RAT back to top of the list if full band scan is not completed (currently only applicable to LTE)*/
	  if ( (reg_send_last_reg_req_scan_scope_get() == SYS_SCAN_SCOPE_FULL_BAND) &&
#ifdef FEATURE_DUAL_SIM
		   (reg_mode_get_rat_search_done_flag(reg_mode_get_full_band_initial_rat(), (sys_modem_as_id_e_type)reg_as_id) == FALSE) &&
#else
		   (reg_mode_get_rat_search_done_flag(reg_mode_get_full_band_initial_rat()) == FALSE) &&
#endif
		 (reg_mode_get_num_interleav_acq_db_list() > 0)

		 )
	  {
	      reg_mode_remove_from_attempted_plmn_list(reg_send_last_reg_req_plmn_get(),reg_mode_get_full_band_initial_rat());
	      reg_mode_remove_from_available_plmn_list(reg_send_last_reg_req_plmn_get(),reg_mode_get_full_band_initial_rat());
	      reg_mode_update_available_plmn_list
	      (
	        reg_mode_plmn_selection_mode,
#ifndef FEATURE_RAT_PRIORITY_LIST        
	        reg_mode_mode_pref_get(),
#else
	        reg_mode_rat_pri_list_get(),
#endif 
	        reg_mode_req_service_domain_get(),
	        available_plmn_list_p
	      );
          reg_mode_set_update_avaialble_pending(TRUE);
	  } //FRACQ
       reg_mode_perform_interlace_acq_db_scan();
	}
#endif
    else
#endif /*ifndef FEATURE_RAT_PRIORITY_LIST*/        
    {
      if(reg_mode_get_update_avaialble_pending() == TRUE)
      {
	    reg_mode_add_initial_plmn_avaialbe_plmn_list();
	    reg_mode_set_update_avaialble_pending(FALSE);
      }
	  else
	  {
      reg_mode_update_available_plmn_list
      (
        reg_mode_plmn_selection_mode,
#ifndef FEATURE_RAT_PRIORITY_LIST        
        reg_mode_mode_pref_get(),
#else
        reg_mode_rat_pri_list_get(),
#endif 
        reg_mode_req_service_domain_get(),
        available_plmn_list_p
      );
	  }
      if(sys_eplmn_list_equivalent_plmn(reg_mode_req_plmn_get()) || 
         sys_plmn_match(reg_mode_req_plmn_get(), reg_sim_read_rplmn(reg_mode_req_service_domain_get()) ) ) 
      {
        /*
        ** Get the next available equivalent RPLMN.
        */
        reg_mode_next_equivalent_rplmn
        (
          &plmn,
          &rat
        );
      }
      else
      {
        sys_plmn_undefine_plmn_id(&plmn);
      }

      /*
      ** Attempt to acquire service on the equivalent RPLMN.
      */
      if (!sys_plmn_id_is_undefined(plmn)) 
      {
        if ( (reg_mode_scan_scope_get() == SYS_SCAN_SCOPE_FULL_BAND) &&
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

        allowed_service_domain = reg_mode_allowed_service_domain
        (
          plmn,                                  /* plmn                   */
          reg_mode_req_service_domain_get()      /* req_service_domain     */
        );

        reg_send_mmr_reg_req
        (
          reg_mode_network_selection_mode_get(), /* network_selection_mode */
          plmn,                                  /* plmn                   */
          allowed_service_domain,                /* req_service_domain     */
          reg_mode_mode_pref_get(),              /* mode_pref              */
          reg_mode_cs_for_request_get(),         /* cs_for_request         */
          reg_mode_ps_for_request_get(),         /* ps_for_request         */
          rat                                   /* rat                    */
#ifdef FEATURE_GSM_BAND_PREF
          ,reg_mode_band_pref_get()               /* band_pref              */
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
          ,reg_mode_rat_pri_list_get()                 /*ue_capabilty*/
#endif 
          ,reg_mode_plmn_selection_reg_req_type
#ifdef FEATURE_LTE
          , reg_mode_sms_only_get()
          ,reg_mode_additional_info_get()
#endif 
#ifdef FEATURE_FEMTO_CSG
          ,SYS_CSG_ID_INVALID
#endif 
#ifdef FEATURE_CIOT
		  ,scan_info //FRACQ
#endif
        );

        reg_mode_plmn_selection_first_attempt = TRUE;
      }
      else if (((reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_AUTOMATIC) ||
               (reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION) ||
#ifdef FEATURE_1XSRLTE
               (reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_AUTOMATIC_CAMP_ONLY) ||
#endif 
               (reg_state_is_pwr_on_manual_plmn_selection))
#if defined(FEATURE_LTE) || defined(FEATURE_WRLF_SYSTEM_SEL)
                && ((reg_mode_plmn_selection_mode != REG_MODE_AUTOMATIC_MODE_SUITABLE_SEARCH)         
#ifdef FEATURE_SGLTE
                || (REG_SGLTE_DUAL_STACK)
#endif 
                )
#endif
                )
      {
         MSG_HIGH_DS_0(REG_SUB, "=REG= RLF get next available PLMN"); 
        /*
        ** Get the next prioritized PLMN.
        */
        reg_mode_next_available_plmn
        (
          &plmn,
          &rat
        );

		 if ( (reg_mode_scan_scope_get() == SYS_SCAN_SCOPE_FULL_BAND) &&
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


        if (reg_state_is_pwr_on_manual_plmn_selection) 
        {
#ifdef FEATURE_EQUIVALENT_HPLMN
          if ( !reg_sim_is_ehplmn(plmn) ) 
#else
          if (!sys_plmn_match(reg_sim_read_hplmn(), plmn))
#endif
          {
            sys_plmn_undefine_plmn_id(&plmn);
          }
        }
        /*
        ** Attempt to acquire service on the PLMN.
        */
        if (!sys_plmn_id_is_undefined(plmn))
        {
          /*
          ** PLMN is not RPLMN if we enter this block, query CM if PLMN/RAT combination is allowed.
          ** If CM API returns False then abort PLMN selection and send service cnf to CM with no service
          ** If CM API retursn True continue with the REG request.
          */
#ifdef FEATURE_LTE_TO_1X
          if((reg_mode_plmn_selection_mode == REG_MODE_AUTOMATIC_MODE_MMSS_RPLMN) &&
             (!cm_per_subs_is_plmn_rat_allowed(plmn,rat,(sys_modem_as_id_e_type)reg_sub_id,reg_set_cm_stack_id())))
          {
            MSG_HIGH_DS_0(REG_SUB,"=REG= CM API for PLMN/RAT allowed query returned FALSE, send no service to CM");
            finished_plmn_selection = TRUE;
          }
          else
#endif 
          {
            reg_mode_update_attempted_plmn_list
            (
              plmn,
              rat
            );
  
            allowed_service_domain = reg_mode_allowed_service_domain
            (
              plmn,                                  /* plmn                   */
              reg_mode_req_service_domain_get()      /* req_service_domain     */
            );
  
            reg_send_mmr_reg_req
            (
              reg_mode_network_selection_mode_get(), /* network_selection_mode */
              plmn,                                  /* plmn                   */
              allowed_service_domain,                /* req_service_domain     */
              reg_mode_mode_pref_get(),              /* mode_pref              */
              reg_mode_cs_for_request_get(),         /* cs_for_request         */
              reg_mode_ps_for_request_get(),         /* ps_for_request         */
              rat                                   /* rat                    */
#ifdef FEATURE_GSM_BAND_PREF
              ,reg_mode_band_pref_get()               /* band_pref              */
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
              ,reg_mode_rat_pri_list_get()                 /*ue_capabilty*/
#endif 
              ,reg_mode_plmn_selection_reg_req_type
#ifdef FEATURE_LTE
            ,reg_mode_sms_only_get()
            ,reg_mode_additional_info_get()
#endif 
#ifdef FEATURE_FEMTO_CSG
              ,SYS_CSG_ID_INVALID
#endif 
#ifdef FEATURE_CIOT
			  ,scan_info //FRACQ
#endif
            );
  
            reg_mode_plmn_selection_first_attempt = TRUE;
          }
        }
        else
        {
        
#ifdef FEATURE_NAS_GW

#ifndef FEATURE_NAS_CS_DISABLED
          managed_roaming_plmn = reg_mode_managed_roaming_plmn_get();
          if(mm_managed_roaming_enabled && !sys_plmn_id_is_undefined(managed_roaming_plmn.plmn))
          {
            allowed_service_domain = reg_mode_allowed_service_domain
            (
              managed_roaming_plmn.plmn,                                 /* plmn                   */
              reg_mode_req_service_domain_get()     /* req_service_domain     */
            );
         
            reg_send_mmr_reg_req
            (
              reg_mode_network_selection_mode_get(), /* network_selection_mode */
              managed_roaming_plmn.plmn,                                 /* plmn                   */
              allowed_service_domain,               /* req_service_domain     */
              reg_mode_mode_pref_get(),             /* mode_pref              */
              reg_mode_cs_for_request_get(),        /* cs_for_request         */
              reg_mode_ps_for_request_get(),        /* ps_for_request         */
              managed_roaming_plmn.rat                                  /* rat                    */
#ifdef FEATURE_GSM_BAND_PREF
              ,reg_mode_band_pref_get()              /* band_pref              */
#endif    
#ifdef FEATURE_RAT_PRIORITY_LIST
              ,reg_mode_rat_pri_list_get()                 /*ue_capabilty*/
#endif 
              ,reg_mode_plmn_selection_reg_req_type
#ifdef FEATURE_LTE
              ,reg_mode_sms_only_get()
              ,reg_mode_additional_info_get()
#endif 
#ifdef FEATURE_FEMTO_CSG
              ,SYS_CSG_ID_INVALID
#endif 
#ifdef FEATURE_CIOT
			  ,scan_info //FRACQ
#endif
            );

            reg_mode_req_rat_set(managed_roaming_plmn.rat);
            reg_mode_req_plmn_set(managed_roaming_plmn.plmn);
            reg_mode_managed_roaming_plmn_reset();

            reg_mode_plmn_selection_first_attempt = TRUE; 
          } 
          else
#endif

          {

#endif 
            finished_plmn_selection = TRUE;

            reg_mode_plmn_selection_first_attempt = FALSE;
#ifdef FEATURE_NAS_GW
          }
#endif 
        }
      }
      else
      {
        finished_plmn_selection = TRUE;

        reg_mode_plmn_selection_first_attempt = FALSE;
      }
    }
  }

  return finished_plmn_selection;
}

#if defined(FEATURE_LTE) || defined(FEATURE_WRLF_SYSTEM_SEL)
/*==============================================================================

FUNCTION NAME

  start_suitable_cell_selection

==============================================================================*/

void start_auto_suitable_cell_selection
(
  sys_network_selection_mode_e_type    network_selection_mode,
  boolean                              cs_for_request,
  boolean                              ps_for_request,
  sys_srv_domain_e_type                req_service_domain,
  const sys_rat_pri_list_info_s_type  *rat_pri_list_info
#ifdef FEATURE_LTE
  , boolean sms_only,
    uint32  additional_info
#endif
)
{
  sys_radio_access_tech_e_type initial_rat;
  sys_rat_pri_list_info_s_type curr_rat_pri_list;
 #ifdef FEATURE_CIOT
  mmr_reg_interlace_scan_s_type    scan_info; //FRACQ
 #endif
  sys_plmn_id_s_type           rlf_plmn;

  reg_mode_3gpp_capability_s_type rlf_rat_search_order;
  reg_mode_3gpp_capability_s_type effective_rat_priority_list;

  reg_mode_plmn_selection_reg_req_type = MMR_REG_REQ_NORMAL;

  memscpy(&curr_rat_pri_list,sizeof(sys_rat_pri_list_info_s_type),rat_pri_list_info,sizeof(sys_rat_pri_list_info_s_type));

  reg_mode_clear_attempted_plmn_list();

  rlf_plmn = reg_state_rlf_plmn_get();
  
  initial_rat = reg_state_rlf_rat_get();
#ifdef FEATURE_CIOT
  scan_info.new_scan  = TRUE;
  scan_info.use_timer = FALSE;
  scan_info.scan_time = 0;//FRACQ
   scan_info.nb1_acqdb_timer_status = 0;
  scan_info.nb1_fullband_timer_status= 0;
#endif
#ifdef FEATURE_LTE
  if ((reg_state_is_active_rat_lte(initial_rat) == FALSE) &&
     (reg_sim_plmn_rat_forbidden(rlf_plmn,SYS_RAT_LTE_RADIO_ACCESS,(sys_modem_as_id_e_type)reg_sub_id) == TRUE))
  {
    reg_mode_remove_rat_from_rat_pri_list(&curr_rat_pri_list,SYS_SYS_MODE_LTE);
  }
#endif
  reg_mode_get_3gpp_rat_capabilty(&rlf_rat_search_order, &curr_rat_pri_list
#ifdef FEATURE_SGLTE
      ,TRUE
#endif                                  
     );
   
  if (!reg_mode_is_rat_present_in_rat_pri_list(initial_rat, &curr_rat_pri_list, TRUE))
  {
    initial_rat = rlf_rat_search_order.rat_list[0];
  }
   memscpy(&effective_rat_priority_list, sizeof(reg_mode_3gpp_capability_s_type), &rlf_rat_search_order, sizeof(reg_mode_3gpp_capability_s_type));
   reg_mode_combine_effective_rat_list(initial_rat,&effective_rat_priority_list);
  
  reg_mode_set_effective_acq_db_rat_list(&effective_rat_priority_list);
  reg_mode_set_initial_full_band_plmn(rlf_plmn, initial_rat);


  if ((reg_mode_scan_scope_get() == SYS_SCAN_SCOPE_FULL_BAND) &&
      (cs_for_request == FALSE) &&
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

  reg_mode_reset_available_plmn_list
  (
   rlf_plmn,
   &rlf_rat_search_order
  );

  reg_mode_update_attempted_plmn_list
  (
    rlf_plmn,
    initial_rat
  );
#ifdef FEATURE_CIOT
  reg_mode_remove_other_rats();
#endif

  reg_mode_plmn_selection_mode = REG_MODE_AUTOMATIC_MODE_SUITABLE_SEARCH;

  reg_send_mmr_reg_req
  (
    network_selection_mode,  /* network_selection_mode */
    rlf_plmn,                   /* plmn                   */
    req_service_domain,  /* req_service_domain     */
    reg_mode_mode_pref_get(),   /* mode_pref              */
    cs_for_request,          /* cs_for_request         */
    ps_for_request,          /* ps_for_request         */
    initial_rat             /* rat                    */
  #ifdef FEATURE_GSM_BAND_PREF
    ,reg_mode_band_pref_get()     /* band_pref              */
  #endif
    ,&curr_rat_pri_list
    ,MMR_REG_REQ_NORMAL
#ifdef FEATURE_LTE
   ,sms_only,
    additional_info
#endif
#ifdef FEATURE_FEMTO_CSG
   ,SYS_CSG_ID_INVALID
#endif 
#ifdef FEATURE_CIOT
   ,scan_info //FRACQ
#endif
  );    
  reg_mode_plmn_selection_first_attempt = TRUE;
}

#endif


/*==============================================================================

                              FUNCTION DEFINITIONS

==============================================================================*/
reg_mode_automatic_mode_e_type reg_mode_get_plmn_selection_mode (void)
{
   return reg_mode_plmn_selection_mode;
};

/*==============================================================================

                             COPYRIGHT INFORMATION

Copyright (c) 2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/reg/vcs/reg_mode_higher_priority_plmn_selection.c_v   1.2   06 Jun 2002 10:48:28   kabraham  $
$Header: //components/rel/mmcp.mpss/6.1.10/nas/reg/src/reg_mode_higher_priority_plmn_selection.c#1 $ $DateTime: 2023/04/25 14:39:18 $ $Author: pwbldsvc $

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
05/21/03   kwa     Restructured code to eliminate lint warnings.
06/04/03   kwa     Fixed one remaining lint warning.
09/07/03   kwa     Replaced call to reg_mode_start_plmn_selection with
                   reg_mode_reinitiate_plmn_selection to properly reset the
                   HPLMN search period timer.
09/08/03   kwa     Removed FEATURE_REG_DEEP_SLEEP.
09/18/03   kwa     Added support for band preferences.
10/22/03   kwa     Removed the parameters update_equivalent_plmn_list and
                   equivalent_plmn_list_p from the function finished.
11/19/03   kwa     Removed obsolete function for retrieving the mode name.
01/20/04   kwa     Added unexpected function handler
                   reg_mode_unexpected_start_plmn_selection_on_specified_plmn.
02/04/04   kwa     Added support for PS follow request.
03/03/04   kwa     Removed calls to clear the attempted and available PLMN
                   lists each time the start_higher_priority_plmn_selection
                   function is called.
04/28/04   kwa     Force the mode to SYS_NETWORK_SELECTION_MODE_AUTOMATIC in
                   finished to handle the case when normal service has been
                   acquired while looking for limited service.
05/16/04   kwa     Added support for CS follow request.
05/20/04   kwa     Removed calls to reg_mode_req_plmn_set since this function
                   is now called in reg_mode_update_attempted_plmn_list.  Added
                   code to resend an MMR_REG_REQ if an MMR_REG_CNF message was
                   received for the wrong RAT.
07/13/04   kwa     Added code to only resend an MMR_REG_REQ if an MMR_REG_CNF
                   message was received for the wrong RAT when the phone is
                   operating in single rather than dual mode.
02/17/05   kwa     Replaced calls to reg_mode_mode_pref_get with
                   reg_mode_req_mode_pref_get to properly handle the search
                   for a higher priority network after receiving an LAU or RAU
                   reject with cause #15.
12/21/04   up      Modified function finished to create a LOG PACKET to
                   indicate that higher priority plmn search is finished.
01/18/06   up      Calling of reg_log_umts_nas_hplmn_search_end is kept under
                   feature FEATURE_GTOW_BACKGROUND_PLMN_SEARCH.
10/12/06   sn      WCDMA_1x mode preference implementation.
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
#include "reg_log.h"
#include "reg_nv.h"
#include <stringl/stringl.h>
/*==============================================================================

                                TYPE DEFINITIONS

==============================================================================*/

/*==============================================================================

                            STATIC DATA DECLARATIONS

==============================================================================*/
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE 

static boolean reg_mode_higher_priority_plmn_selection_first_attempt_sim[MAX_NAS_STACKS];
#define reg_mode_higher_priority_plmn_selection_first_attempt   reg_mode_higher_priority_plmn_selection_first_attempt_sim[reg_as_id]

#else

static boolean reg_mode_higher_priority_plmn_selection_first_attempt;

#endif /* FEATURE_DUAL_SIM || FEATURE_SGLTE */


static reg_mode_automatic_mode_e_type reg_mode_automatic_type;
/*==============================================================================

                          STATIC FUNCTION DEFINITIONS

==============================================================================*/
/*==============================================================================

FUNCTION NAME

  reg_mode_low_priority_rat_from_3gpp_rat_list

  DESCRIPTION
    This function will remove the rats situated below the passed in rat

==============================================================================*/
static void reg_mode_low_priority_rat_from_3gpp_rat_list(reg_mode_3gpp_capability_s_type *reg_3gpp_rat_list,
    const sys_radio_access_tech_e_type rat)
{
  uint8 i;      
  for (i= 0; i < reg_3gpp_rat_list->num_items; i++)  
  {    
      if(reg_3gpp_rat_list->rat_list[i] == rat)   
      {      
          reg_3gpp_rat_list->num_items = i;
          break;   
      } 
  } 
}
/*==============================================================================

FUNCTION NAME

  start_higher_priority_plmn_selection

==============================================================================*/

boolean start_higher_priority_plmn_selection
(
  const sys_detailed_plmn_list_s_type*       available_plmn_list_p
)
{
  sys_radio_access_tech_e_type rat;
  sys_srv_domain_e_type        allowed_service_domain;
  sys_plmn_id_s_type           plmn;
  boolean                      started = FALSE;
#ifdef FEATURE_CIOT  
mmr_reg_interlace_scan_s_type    scan_info; //FRACQ
#endif
  reg_mode_clear_attempted_plmn_list();
  reg_mode_clear_available_plmn_list();
  reg_mode_automatic_type = REG_MODE_AUTOMATIC_MODE_HIGH_PRIORITY_SEARCH;
  reg_mode_update_available_plmn_list
  (
    REG_MODE_AUTOMATIC_MODE_HIGH_PRIORITY_SEARCH,
#ifndef FEATURE_RAT_PRIORITY_LIST
    reg_mode_mode_pref_get(),
#else
    reg_mode_rat_pri_list_get(),
#endif
    reg_mode_req_service_domain_get(),
    available_plmn_list_p
  );

#ifdef FEATURE_CIOT 
  scan_info.new_scan  = TRUE;
  scan_info.use_timer = FALSE;
  scan_info.scan_time = 0;//FRACQ
   scan_info.nb1_acqdb_timer_status = 0;
  scan_info.nb1_fullband_timer_status= 0;
#endif
  /*
  ** Get the next prioritized PLMN.
  */
  reg_mode_next_higher_priority_plmn
  (
#ifndef FEATURE_RAT_PRIORITY_LIST
    reg_mode_req_mode_pref_get(),
#endif
    &plmn,
    &rat
  );

  /*
  ** Attempt to acquire service on the PLMN.
  */
  if (!sys_plmn_id_is_undefined(plmn))
  {
    reg_mode_update_attempted_plmn_list
    (
      plmn,
      rat
    );

    allowed_service_domain = reg_mode_allowed_service_domain
    (
      plmn,                                 /* plmn                   */
      reg_mode_req_service_domain_get()     /* req_service_domain     */
    );

    reg_send_mmr_reg_req
    (
      SYS_NETWORK_SELECTION_MODE_AUTOMATIC, /* network_selection_mode */
      plmn,                                 /* plmn                   */
      allowed_service_domain,               /* req_service_domain     */
      reg_mode_req_mode_pref_get(),         /* mode_pref              */
      FALSE,                                /* cs_for_request         */
      FALSE,                                /* ps_for_request         */
      rat                                  /* rat                    */
#ifdef FEATURE_GSM_BAND_PREF
      ,reg_mode_band_pref_get()              /* band_pref              */
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
      ,reg_mode_rat_pri_list_get()                 /*rat_pri_list*/
#endif 
      ,MMR_REG_REQ_NORMAL
#ifdef FEATURE_LTE
      ,reg_mode_sms_only_get(),
       0
#endif
#ifdef FEATURE_FEMTO_CSG
      ,SYS_CSG_ID_INVALID
#endif 
	  ,scan_info
    );

    started = TRUE;

    reg_mode_higher_priority_plmn_selection_first_attempt = TRUE;
  }

  /*
  ** LOG a PACKET to indicate the list of available PLMNs
  ** after higher priority PLMN search is finished.
  */
#if defined(FEATURE_GTOW_BACKGROUND_PLMN_SEARCH) || defined(FEATURE_RAT_PRIORITY_LIST)
  reg_log_umts_nas_hplmn_search_end(reg_mode_available_plmn_list_get());
#endif

  return started;
}
#ifdef FEATURE_CIOT
/*==============================================================================

FUNCTION NAME

  reg_mode_start_higher_priority_plmn_selection_ciot

DESCRIPTION

  Function that starts the higher priority PLMN selection process for CIOT

RETURN VALUE

  boolean  Flag indicating whether registration on a higher priority PLMN
           was initiated (TRUE) or not (FALSE).

==============================================================================*/
boolean start_higher_priority_plmn_selection_ciot
(
  void
)
{
    sys_radio_access_tech_e_type initial_rat;
    reg_mode_3gpp_capability_s_type  hplmn_rat_search_order;
#ifdef FEATURE_CIOT
    mmr_reg_interlace_scan_s_type    scan_info; //FRACQ
#endif
#ifdef FEATURE_EQUIVALENT_HPLMN
    reg_sim_plmn_list_s_type     ehplmn_list;
    sys_plmn_id_s_type           plmn_initial;
#else
    sys_plmn_id_s_type           hplmn;
#endif
    
    reg_mode_higher_priority_plmn_selection_first_attempt = TRUE;
    /* Use the same prioritization */
    reg_mode_automatic_type = REG_MODE_AUTOMATIC_MODE_HIGH_PRIORITY_SEARCH;    
    reg_mode_clear_attempted_plmn_list();

#ifdef FEATURE_CIOT
	scan_info.new_scan  = TRUE;
  scan_info.use_timer = FALSE;
  scan_info.scan_time = 0;//FRACQ
  scan_info.nb1_acqdb_timer_status = 0;
   scan_info.nb1_fullband_timer_status= 0;
#endif
#ifdef FEATURE_EQUIVALENT_HPLMN
    ehplmn_list = reg_sim_ehplmn_list_get();
#else
    hplmn = reg_sim_read_hplmn();
#endif

#ifdef FEATURE_EQUIVALENT_HPLMN
    plmn_initial = ehplmn_list.plmn[0];
    /* If RPLMN is one of the EHPLNM then start PLMN selection from RPLMN 
    ** else use EHPLMN[0]
    */    
#endif
    reg_mode_get_3gpp_rat_capabilty(&hplmn_rat_search_order, reg_mode_rat_pri_list_get()
#ifdef FEATURE_SGLTE
        ,TRUE
#endif
        );
#ifdef FEATURE_NAS_GW
    reg_sim_read_hplmn_rat_search_order(&hplmn_rat_search_order);
#endif
#ifdef FEATURE_LTE
    reg_mode_adjust_rat_order_plmn_backoff_forbidden
        (
#ifdef FEATURE_EQUIVALENT_HPLMN
        &(plmn_initial),
#else
        &hplmn,
#endif
        &hplmn_rat_search_order
        );
#endif

//    initial_rat = hplmn_rat_search_order.rat_list[0];        

    initial_rat = reg_state_active_rat_get();

    reg_mode_reset_available_plmn_list
    (
#ifdef FEATURE_EQUIVALENT_HPLMN
        plmn_initial,
#else
        hplmn,
#endif
        &hplmn_rat_search_order
    );

    reg_mode_update_attempted_plmn_list
    (
#ifdef FEATURE_EQUIVALENT_HPLMN
        plmn_initial,
#else
        hplmn,
#endif
        initial_rat
    );

    reg_send_mmr_reg_req
        (
        SYS_NETWORK_SELECTION_MODE_AUTOMATIC,
#ifdef FEATURE_EQUIVALENT_HPLMN
        plmn_initial,
#else
        hplmn,                                /* plmn                   */
#endif
        reg_mode_req_service_domain_get(),
        reg_mode_req_mode_pref_get(),
        reg_mode_cs_for_request_get(),
        reg_mode_ps_for_request_get(),
        initial_rat                            /* rat                    */
#ifdef FEATURE_GSM_BAND_PREF
        ,reg_mode_band_pref_get()
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
        ,reg_mode_rat_pri_list_get(),
#endif        
        MMR_REG_REQ_NORMAL
#ifdef FEATURE_LTE
        ,reg_mode_sms_only_get(),
        0
#endif 
#ifdef FEATURE_FEMTO_CSG
       ,SYS_CSG_ID_INVALID
#endif 
	   ,scan_info
        );

    reg_mode_higher_priority_plmn_selection_first_attempt = TRUE;

    return TRUE;
}
#endif
#ifdef FEATURE_CIOT
void start_higher_priority_irat_plmn_selection_ciot
(
  void
)
{    
    reg_mode_3gpp_capability_s_type  hplmn_rat_search_order;
#ifdef FEATURE_CIOT
	mmr_reg_interlace_scan_s_type    scan_info; //FRACQ
#endif
    sys_radio_access_tech_e_type initial_rat = SYS_RAT_NONE;
    sys_rat_pri_list_info_s_type rat_list_local;
    sys_sys_mode_e_type rat_list[REG_NV_MAX_RATS_LENGTH];    
    boolean plmn_id_is_undefined;
    boolean mnc_includes_pcs_digit;
    uint32  mcc, mnc, i;    
      
#ifdef FEATURE_EQUIVALENT_HPLMN
    reg_sim_plmn_list_s_type     ehplmn_list;
    sys_plmn_id_s_type           plmn_initial;
#else
    sys_plmn_id_s_type           hplmn;
#endif

    reg_mode_higher_priority_plmn_selection_first_attempt = TRUE;    
    reg_mode_automatic_type = REG_MODE_AUTOMATIC_MODE_CIOT_IRAT_SEARCH;  
    reg_mode_clear_attempted_plmn_list();
#ifdef FEATURE_CIOT
    scan_info.new_scan  = TRUE;
  scan_info.use_timer = FALSE;
  scan_info.scan_time = 0;//FRACQ
   scan_info.nb1_acqdb_timer_status = 0;
  scan_info.nb1_fullband_timer_status= 0;
#endif
   if(reg_state_roaming_ind_get() == SYS_ROAM_STATUS_OFF)
   {
#ifdef FEATURE_EQUIVALENT_HPLMN
	    ehplmn_list = reg_sim_ehplmn_list_get();
#else
	    hplmn = reg_sim_read_hplmn();
#endif

#ifdef FEATURE_EQUIVALENT_HPLMN
	    plmn_initial =  ehplmn_list.plmn[0];
	    /* If RPLMN is one of the EHPLNM then start PLMN selection from RPLMN 
	    ** else use EHPLMN[0]
	    */    
#endif
   }
   else
   {
		plmn_initial =	reg_send_last_reg_req_plmn_get();
   }
    rat_list_local = *reg_mode_rat_pri_list_get();
    
    sys_plmn_get_mcc_mnc
    (
         plmn_initial,
         &plmn_id_is_undefined,
         &mnc_includes_pcs_digit,
         &mcc,
         &mnc
    );
        
    /* Check if this MCC has a subset of RATs to be searched */
    if (reg_nv_search_mcc_rats(mcc, &rat_list[0], REG_NV_MAX_RATS_LENGTH))
    {
        /* Found the MCC in the database */
        /* Check if at least one RAT in that list matches the UE capability and modify
         * the rat_list_local accordingly
         */
        if (reg_nv_filter_rats(rat_list, REG_NV_MAX_RATS_LENGTH, &rat_list_local))
        {
            /* At least one rat matches the UE capability */
            for (i = 0; i < rat_list_local.num_items; ++i )
            {
                MSG_HIGH_DS_2(REG_SUB, "=REG= CIoT IRAT filtered RAT %d for MCC %d", 
                    rat_list_local.priority_list_info[i].acq_sys_mode, mcc);
            }
        }
    }
    /* At this stage rat_list_local can be the same as the UE capability or
     * can be its subset
     */

    /* Extract the RAT search order from the local list */
    reg_mode_get_3gpp_rat_capabilty(&hplmn_rat_search_order, &rat_list_local
#ifdef FEATURE_SGLTE
        ,TRUE
#endif
        );

    /* Set the initial_rat RAT to the most significant RAT */
    initial_rat = hplmn_rat_search_order.rat_list[0];

    /* Remove the rats situated below the active rat. If no service is acquired
    ** NAS will reinitiate starting from RPLMN rat
    */      
    reg_mode_low_priority_rat_from_3gpp_rat_list(&hplmn_rat_search_order,reg_state_active_rat_get());
      
#ifdef FEATURE_LTE
    reg_mode_adjust_rat_order_plmn_backoff_forbidden
        (
#ifdef FEATURE_EQUIVALENT_HPLMN
        &(plmn_initial),
#else
        &hplmn,
#endif
        &hplmn_rat_search_order
        );
#endif
   
    reg_mode_reset_available_plmn_list
    (
#ifdef FEATURE_EQUIVALENT_HPLMN
        plmn_initial,
#else
        hplmn,
#endif
        &hplmn_rat_search_order
    );

    reg_mode_update_attempted_plmn_list
    (
#ifdef FEATURE_EQUIVALENT_HPLMN
        plmn_initial,
#else
        hplmn,
#endif
        initial_rat
    );

    reg_send_mmr_reg_req
        (
        SYS_NETWORK_SELECTION_MODE_AUTOMATIC,
#ifdef FEATURE_EQUIVALENT_HPLMN
        plmn_initial,
#else
        hplmn,                                /* plmn                   */
#endif
        reg_mode_req_service_domain_get(),
        reg_mode_req_mode_pref_get(),
        reg_mode_cs_for_request_get(),
        reg_mode_ps_for_request_get(),
        initial_rat                            /* rat                    */
#ifdef FEATURE_GSM_BAND_PREF
        ,reg_mode_band_pref_get()
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
        ,reg_mode_rat_pri_list_get(),
#endif        
        MMR_REG_REQ_NORMAL
#ifdef FEATURE_LTE
        ,reg_mode_sms_only_get(),
        0
#endif 
#ifdef FEATURE_FEMTO_CSG
       ,SYS_CSG_ID_INVALID
#endif 
	   ,scan_info
        );

    reg_mode_higher_priority_plmn_selection_first_attempt = TRUE;    
}
#endif
#if defined (FEATURE_TDSCDMA) || defined (FEATURE_SGLTE)
/*==============================================================================

FUNCTION NAME

  start_higher_priority_irat_plmn_selection

==============================================================================*/
boolean start_higher_priority_irat_plmn_selection
(
  const sys_detailed_plmn_list_s_type*       available_plmn_list_p
)
{
    sys_radio_access_tech_e_type rat;
    sys_srv_domain_e_type        allowed_service_domain;
    sys_plmn_id_s_type           plmn;
    boolean                      started = FALSE;
	boolean                      reg_start = TRUE;
    mmr_reg_req_e_type           type;
#ifdef FEATURE_CIOT
   mmr_reg_interlace_scan_s_type    scan_info; //FRACQ
  scan_info.new_scan  = TRUE;
  scan_info.use_timer = FALSE;
  scan_info.scan_time = 0;//FRACQ
   scan_info.nb1_acqdb_timer_status = 0;
  scan_info.nb1_fullband_timer_status= 0;
#endif
    reg_mode_clear_attempted_plmn_list();
    reg_mode_clear_available_plmn_list();
    reg_mode_automatic_type = REG_MODE_AUTOMATIC_MODE_IRAT_HIGH_PRIORITY_SEARCH;

    /* Add the current RPLMN to the available reg*/
    reg_mode_update_available_plmn_list
    (
        REG_MODE_AUTOMATIC_MODE_IRAT_HIGH_PRIORITY_SEARCH,
        reg_mode_rat_pri_list_get(),
        reg_mode_req_service_domain_get(),
        available_plmn_list_p
    );

#if defined FEATURE_TDSCDMA || defined FEATURE_SGLTE
      reg_mode_update_lte_lists();
      reg_state_set_irat_hplmn_timer_search_ongoing(FALSE);
      reg_state_set_irat_lte_timer_search_ongoing(FALSE);
#endif
    
    /*
    ** Get the next prioritized PLMN.
    */
    reg_mode_next_available_plmn 
    (
       &plmn,
       &rat
    );

    /*
    ** Attempt to acquire service on the PLMN.
    */
    if (!sys_plmn_id_is_undefined(plmn))
    {
        reg_mode_update_attempted_plmn_list
        (
            plmn,
            rat
        );

        allowed_service_domain = reg_mode_allowed_service_domain
        (
            plmn,                                 /* plmn                   */
            reg_mode_req_service_domain_get()     /* req_service_domain     */
        );
        /* For SGLTE case, determine if split is needed. If so, send
           a split request with MMR_REG_REQ_SGLTE_SPLIT_REQUEST.
           The following conditions need to be met
           ds 1 active, ds2 inactive
           SGLTE mode

        */
        type = MMR_REG_REQ_NORMAL;
 #ifdef FEATURE_SGLTE
        if( IS_UE_IN_SGLTE_STATE_4 &&
            reg_state_get_active_rat_per_subs((sys_modem_as_id_e_type)reg_as_id) == SYS_RAT_GSM_RADIO_ACCESS &&
            reg_state_get_service_status_per_subs((sys_modem_as_id_e_type)reg_as_id)== SYS_SRV_STATUS_SRV &&
            (rat == SYS_RAT_LTE_RADIO_ACCESS ||
            rat == SYS_RAT_TDS_RADIO_ACCESS)
           )
         {
           if(IS_SGLTE_SPLIT_IN_PROGRESS)
            {
              reg_start = FALSE;
            }
            else
            {
              type = MMR_REG_REQ_SGLTE_PS_ONLY_ACQUISITION;
            }
         }
#endif

		if(reg_start)
        {
          reg_send_mmr_reg_req
          (
            SYS_NETWORK_SELECTION_MODE_AUTOMATIC, /* network_selection_mode */
            plmn,                                 /* plmn                   */
            allowed_service_domain,               /* req_service_domain     */
            reg_mode_req_mode_pref_get(),         /* mode_pref              */
            FALSE,                                /* cs_for_request         */
            FALSE,                                /* ps_for_request         */
            rat                                  /* rat                    */
#ifdef FEATURE_GSM_BAND_PREF
            ,reg_mode_band_pref_get()              /* band_pref              */
#endif
            ,reg_mode_rat_pri_list_get()                 /*rat_pri_list*/
            ,type
#ifdef FEATURE_LTE
            ,reg_mode_sms_only_get(),
             0
#endif
#ifdef FEATURE_FEMTO_CSG
            ,SYS_CSG_ID_INVALID
#endif 
			,scan_info
          );
          started = TRUE;
          reg_mode_higher_priority_plmn_selection_first_attempt = TRUE;
	   }
    }
    
    return started;
}
#endif
/*==============================================================================

FUNCTION NAME

  finished

==============================================================================*/

boolean finished_high_priority_selection
(
        sys_plmn_service_state_s_type  service_state,
  const sys_detailed_plmn_list_s_type* available_plmn_list_p
)
{
  sys_radio_access_tech_e_type rat;
  sys_srv_domain_e_type        allowed_service_domain;
#ifdef FEATURE_CIOT
  mmr_reg_interlace_scan_s_type    scan_info; //FRACQ
#endif
  sys_plmn_id_s_type           plmn;
  boolean                      success = FALSE;
  boolean                      finished_higher_priority_plmn_selection = FALSE;

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
  ** The MS is registered.
  */
  if (success)
  {
    if (service_state.roaming_ind == SYS_ROAM_STATUS_ON)
    {
      reg_timers_start_hplmn_search_period_timer();
#if defined (FEATURE_TDSCDMA) || defined (FEATURE_SGLTE)
    }
    else
    {
        MSG_HIGH_DS_0(REG_SUB, "=REG= Starting reg_mode_rat_search_start");
        reg_mode_rat_search_start();
    }
#else

    }
#endif
    finished_higher_priority_plmn_selection = TRUE;
  }
  /*
  ** The MS is not registered.
  */
  else
  {
    /*
    ** If the MS is not on the requested RAT then retry one more time.
    */
#ifndef FEATURE_RAT_PRIORITY_LIST
    if ( (service_state.active_rat != reg_mode_req_rat_get()) &&
         ( (reg_mode_req_mode_pref_get() == SYS_MODE_PREF_GSM_RADIO_ACCESS_ONLY) ||
#ifdef FEATURE_UMTS_1X_HANDOVER_UMTSMSM
           #error code not present
#endif
           (reg_mode_req_mode_pref_get() == SYS_MODE_PREF_UMTS_RADIO_ACCESS_ONLY) ) &&
         (reg_mode_higher_priority_plmn_selection_first_attempt == TRUE) )
    {
      reg_send_last_mmr_reg_req();

      reg_mode_higher_priority_plmn_selection_first_attempt = FALSE;
    }
    else
#else /*FEATURE_RAT_PRIORITY_LIST*/
    if((reg_mode_higher_priority_plmn_selection_first_attempt == TRUE) &&
        (service_state.active_rat != reg_mode_req_rat_get()) &&
       !(reg_mode_is_rat_present_in_rat_pri_list(service_state.active_rat,
                                            reg_mode_rat_pri_list_get(),TRUE)))
    {
      reg_send_last_mmr_reg_req();
      reg_mode_higher_priority_plmn_selection_first_attempt = FALSE;
    }
    else
#endif /*FEATURE_RAT_PRIORITY_LIST*/
    {
      reg_mode_update_available_plmn_list
      (
        reg_mode_automatic_type,
#ifndef FEATURE_RAT_PRIORITY_LIST
        reg_mode_mode_pref_get(),
#else
        reg_mode_rat_pri_list_get(),
#endif
        reg_mode_req_service_domain_get(),
        available_plmn_list_p
      );

     if (reg_mode_automatic_type == REG_MODE_AUTOMATIC_MODE_HIGH_PRIORITY_SEARCH)
     {
    /*
    ** Get the next prioritized PLMN.
    */
      reg_mode_next_higher_priority_plmn
      (
#ifndef FEATURE_RAT_PRIORITY_LIST
        reg_mode_req_mode_pref_get(),
#endif
        &plmn,
        &rat
      );
     }
#if defined(FEATURE_TDSCDMA)||defined(FEATURE_CIOT)
     else 
     {
      reg_mode_next_available_plmn      
      (
        &plmn,
        &rat
      );
     }
#endif
      /*
      ** Attempt to acquire service on the PLMN.
      */
      if (!sys_plmn_id_is_undefined(plmn))
      {
        reg_mode_update_attempted_plmn_list
        (
          plmn,
          rat
        );
  
        allowed_service_domain = reg_mode_allowed_service_domain
        (
          plmn,                                 /* plmn                   */
          reg_mode_req_service_domain_get()     /* req_service_domain     */
        );
  
        reg_send_mmr_reg_req
        (
          SYS_NETWORK_SELECTION_MODE_AUTOMATIC, /* network_selection_mode */
          plmn,                                 /* plmn                   */
          allowed_service_domain,               /* req_service_domain     */
          reg_mode_req_mode_pref_get(),         /* mode_pref              */
          FALSE,                                /* cs_for_request         */
          FALSE,                                /* ps_for_request         */
          rat                                  /* rat                    */
#ifdef FEATURE_GSM_BAND_PREF
          ,reg_mode_band_pref_get()              /* band_pref              */
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
          ,reg_mode_rat_pri_list_get()                    /*ue_capabilty*/
#endif 
          ,MMR_REG_REQ_NORMAL
#ifdef FEATURE_LTE
          ,reg_mode_sms_only_get(),
           0
#endif
#ifdef FEATURE_FEMTO_CSG
          ,SYS_CSG_ID_INVALID
#endif 
		  ,scan_info
        );

        reg_mode_higher_priority_plmn_selection_first_attempt = TRUE;

      }
      /*
      ** All attempts to register on a higher priority PLMN have failed.
      */
      else
      {
     /* Do not attempt to recove service if no PLMN has been found furing HPPLMN search */
#ifndef FEATURE_CIOT
        MSG_HIGH_DS_0(REG_SUB, "=REG= Reinitiate PLMN selection ");
        reg_mode_reinitiate_plmn_selection
        (
          SYS_NETWORK_SELECTION_MODE_AUTOMATIC,
          reg_mode_req_mode_pref_get(),
          FALSE,
          FALSE,
#ifdef FEATURE_GSM_BAND_PREF
          reg_mode_req_service_domain_get(),
          reg_mode_band_pref_get()
#else
          reg_mode_req_service_domain_get()
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
          ,reg_mode_rat_pri_list_get()
#endif
#ifdef FEATURE_LTE
          ,reg_mode_sms_only_get(),
           0
#endif 
        );
#endif
        reg_mode_higher_priority_plmn_selection_first_attempt = FALSE;
        /* This ensures that NAS indicates no service to CM when no HPPLMN was found  */
        return TRUE;
      }
    }
  }

  return finished_higher_priority_plmn_selection;
}


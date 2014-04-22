/*==============================================================================

                             COPYRIGHT INFORMATION

Copyright (c) 2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/reg/vcs/reg_state_idle.c_v   1.4   07 Jun 2002 14:23:46   kabraham  $
$Header: //components/rel/mmcp.mpss/6.1.10/nas/reg/src/reg_state_idle.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
02/13/02   kwa     Initial release
05/20/02   kwa     Added output_name function.
06/05/02   kwa     Filled roaming_ind when receiving an mmr_reg_cnf or
                   mmr_service_ind.
06/06/02   kwa     Replaced mode_pref with dual_mode and rat in
                   mmr_plmn_search_req.
06/27/02   kwa     Modified function that sends an mmr_plmn_search_req due to
                   interface change.
08/26/02   kwa     Removed limited service from check for loss of service in
                   process_mmr_service_ind.
08/26/02   kwa     Added call to reg_sim_write_rplmn_rat in 
                   process_mmr_service_ind to update the last used RAT when
                   service is reacquired on the RPLMN.
09/19/02   kwa     Added function to process the cm_act_req message.
10/04/02   kwa     Added code to properly handle reject causes in
                   process_mmr_service_ind.
10/10/02   kwa     Added code to handle expiration of plmn search period timer
                   in limited service network selection mode.
10/18/02   kwa     Added FEATURE_REG_DEEP_SLEEP.
11/05/02   kwa     Added process_mmr_stop_mode_cnf to handle an
                   mmr_stop_mode_cnf received in reponse to an mmr_stop_mode_req
                   sent to request deep sleep.
12/31/02   kwa     Added code to support equivalent PLMNs in
                   process_mmr_service_ind.
03/13/03   kwa     Modified process_mmr_service_ind so REG does not initiate
                   PLMN selection when the phone loss services and
                   FEATURE_REG_DEEP_SLEEP is not defined.  CM/MMoC are
                   responsible for initiating PLMN selection in this scenario.
03/27/03   kwa     Moved functionality of process_mmr_service_ind to
                   reg_state_process_mmr_service_ind.
04/03/03   kwa     Added code in process_hplmn_search_timer_expiration to
                   limit the search for a higher priority PLMN.  The search
                   is only initiated when a higher priority PLMN is contained
                   on the SIM in the same country as the RPLMN.
05/21/03   kwa     Restructured code to eliminate lint warnings.
06/04/03   kwa     Removed one remaining lint warning.
09/08/03   kwa     Removed FEATURE_REG_DEEP_SLEEP.
09/18/03   kwa     Added support for band preferences.
09/29/03   kwa     Added code to handle the new enumerated value
                   SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION
                   in process_hplmn_search_timer_expiration.
11/19/03   kwa     Removed obsolete function for retrieving the state name.
01/12/04   kwa     Added call to reg_state_automatic_search_initiated to support
                   the search for a higher priority network on both RATs.
01/22/04   kwa     Added call to reg_state_service_status to support an
                   optimization to avoid performing a full search for service
                   when the user initiates PLMN selection and the phone is
                   already camped on the HPLMN. 
04/28/04   kwa     Modified process_hplmn_search_timer_expiration to initiate
                   a search for a higher priority network if normal service was
                   acquired while searching for limited service.
05/17/04   kwa     Added function to process the mmr_camped_ind message.
07/13/04   kwa     Replaced function process_mmr_reg_cnf with the more
                   general and correct reg_state_process_mmr_reg_cnf.
08/11/04   kwa     Added call to reg_send_cm_stop_mode_cnf in the function
                   process_mmr_reg_cnf to avoid a lock up scenario with CM.
11/12/04   kwa     Added support for FEATURE_GTOW_BACKGROUND_PLMN_SEARCH
                   in process_hplmn_search_timer_expiration.
01/24/05   up      Modified process_hplmn_search_timer_expiration to allow
                   higher priority PLMN selection when the HPLMN timer
                   expires and MS is in LIMITED/LIMITED_REGIONAL service.
01/24/05   up      Updated function process_hplmn_search_timer_expiration to
                   use original preferred mode to determine the dual mode.
02/17/05   up      Modified process_hplmn_search_timer_expiration to handle
                   mode SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION.
02/22/05   up      Updated for Cingular ENS feature.
                   Updated state table to add new function pointer for FCN IND.
                   Added state function process_sim_refresh_fcn_ind.
03/01/05   up      ENS Fix - supported flags are replaced with hash defines.
07/18/05   up      Supporting for two new ENS EFs UIM_USIM_ACT_HPLMN and 
                   UIM_USIM_SPT_TABLE under USIM directory.
09/19/05   up      Added process_cm_hplmn_search_req and updated other
                   to support the on demand HPLMN search from user.
10/20/05   up      Updated for 3G to 2G BPLMN Search.
                   Kept under FEATURE_WTOG_BACKGROUND_PLMN_SEARCH.
11/07/05   ajt     Added handling of MMR_EMERGENCY_NUM_LIST_IND in idle state.                   
01/05/06   sn      Modified process_hplmn_search_timer_expiration. Added check
                   that timer will be started only if MS on VPLMN.
04/06/06   sn      Updated process_sim_refresh_fcn_ind for ehplmn
10/12/06   sn      WCDMA_1X mode preference implementation.
11/23/06   sn      Added feature FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH
02/13/07   sn      Do not start HPLMN search if UE is in Limited Service.
==============================================================================*/

/*==============================================================================

                            INCLUDE FILES FOR MODULE

==============================================================================*/

#include "mmcp_variation.h"
#include "customer.h"
#include "err.h"
#include "msg.h"
#include "reg_mode.h"
#include "reg_send.h"
#include "reg_sim_v.h"
#include "reg_state.h"
#include "ms.h"
#include "reg_timers.h"
#include "reg_log.h"
#include "reg_nv.h"
#include "sys_eplmn_list_v.h"
#include <stringl/stringl.h>
#include "ref_cnt_obj.h"

/*==============================================================================

                                TYPE DEFINITIONS

==============================================================================*/

/*==============================================================================

                            STATIC DATA DECLARATIONS

==============================================================================*/
#if defined(FEATURE_TRIPLE_SIM)  || defined(FEATURE_SGLTE_DUAL_SIM)
#ifdef FEATURE_GW_ON_DEMAND_HPLMN_SRCH
/*
** Set this flag to TRUE if the HPLMN search is initiated in MANUAL mode.
*/
static sys_network_selection_mode_e_type reg_state_idle_prev_nw_sel_mode_sim[MAX_NAS_STACKS] = { SYS_NETWORK_SELECTION_MODE_NONE, SYS_NETWORK_SELECTION_MODE_NONE,SYS_NETWORK_SELECTION_MODE_NONE};
#define reg_state_idle_prev_nw_sel_mode reg_state_idle_prev_nw_sel_mode_sim[reg_as_id]
#endif

#elif defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
#ifdef FEATURE_GW_ON_DEMAND_HPLMN_SRCH
/*
** Set this flag to TRUE if the HPLMN search is initiated in MANUAL mode.
*/
static sys_network_selection_mode_e_type reg_state_idle_prev_nw_sel_mode_sim[MAX_NAS_STACKS] = { SYS_NETWORK_SELECTION_MODE_NONE, SYS_NETWORK_SELECTION_MODE_NONE};
#define reg_state_idle_prev_nw_sel_mode reg_state_idle_prev_nw_sel_mode_sim[reg_as_id]
#endif

#else
#ifdef FEATURE_GW_ON_DEMAND_HPLMN_SRCH
/*
** Set this flag to TRUE if the HPLMN search is initiated in MANUAL mode.
*/
static sys_network_selection_mode_e_type reg_state_idle_prev_nw_sel_mode = SYS_NETWORK_SELECTION_MODE_NONE;
#endif

#endif /* FEATURE_DUAL_SIM*/
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
#define sys_eplmn_list_validate(a) sys_eplmn_per_nas_stacks_list_validate(a, (sys_modem_as_id_e_type)reg_as_id)
#define sys_eplmn_list_rplmn() (sys_eplmn_per_nas_stacks_list_rplmn((sys_modem_as_id_e_type)reg_as_id))
#endif

#ifdef FEATURE_DUAL_SIM
#ifdef FEATURE_ENHANCED_NW_SELECTION
#define reg_sim_read_ens_flag() reg_sim_per_nas_stacks_read_ens_flag((sys_modem_as_id_e_type)reg_sub_id)
#endif

#endif


/*==============================================================================

                            STATIC DATA DECLARATIONS

==============================================================================*/
#if !defined FEATURE_RAT_PRIORITY_LIST && !defined FEATURE_GTOW_BACKGROUND_PLMN_SEARCH
#if defined(FEATURE_TRIPLE_SIM)  || defined(FEATURE_SGLTE_DUAL_SIM)
static boolean                       reg_state_automatic_search_complete_sim[MAX_NAS_STACKS] = { TRUE, TRUE, TRUE};
#define reg_state_automatic_search_complete reg_state_automatic_search_complete_sim[reg_as_id]
static sys_radio_access_tech_e_type  reg_state_automatic_search_rat_sim[MAX_NAS_STACKS];
#define reg_state_automatic_search_rat reg_state_automatic_search_rat_sim[reg_as_id]

#elif defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE/* FEATURE_DUAL_SIM || FEATURE_SGLTE*/
static boolean                       reg_state_automatic_search_complete_sim[MAX_NAS_STACKS] = { TRUE, TRUE};
#define reg_state_automatic_search_complete reg_state_automatic_search_complete_sim[reg_as_id]
static sys_radio_access_tech_e_type  reg_state_automatic_search_rat_sim[MAX_NAS_STACKS];
#define reg_state_automatic_search_rat reg_state_automatic_search_rat_sim[reg_as_id]

#else

static boolean                       reg_state_automatic_search_complete = TRUE;
static sys_radio_access_tech_e_type  reg_state_automatic_search_rat;
#endif /* FEATURE_DUAL_SIM || FEATURE_SGLTE*/
#endif

#ifdef FEATURE_CIOT
/* This array contains the rats that still need to be used in network list request
   The rats are removed from this list after each search
*/
static sys_rat_pri_list_info_s_type  reg_state_rat_pri_list_ciot_manual;
/* This is the RAT which perfoms search*/
static sys_radio_access_tech_e_type  reg_state_manual_search_ciot_rat;
#endif

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE

extern reg_cm_network_list_req_e_type reg_last_recieved_list_req_type_sim[MAX_NAS_STACKS];
#define reg_last_recieved_list_req_type reg_last_recieved_list_req_type_sim[reg_as_id]

#ifndef FEATURE_RAT_PRIORITY_LIST
#ifndef FEATURE_GTOW_BACKGROUND_PLMN_SEARCH

#ifdef FEATURE_TRIPLE_SIM /*FEATURE_TRIPLE_SIM */
static boolean                       reg_state_manual_search_complete_sim[MAX_NAS_STACKS] = { FALSE, FALSE,FALSE };
#define reg_state_manual_search_complete reg_state_manual_search_complete_sim[reg_as_id]
#else
static boolean                       reg_state_manual_search_complete_sim[MAX_NAS_STACKS] = { FALSE, FALSE};
#define reg_state_manual_search_complete reg_state_manual_search_complete_sim[reg_as_id]
#endif

static sys_radio_access_tech_e_type  reg_state_manual_search_rat_sim[MAX_NAS_STACKS];
#define reg_state_manual_search_rat reg_state_manual_search_rat_sim[reg_as_id]
#endif
static sys_mode_pref_e_type          reg_state_manual_search_mode_pref_sim[MAX_NAS_STACKS];
#define reg_state_manual_search_mode_pref   reg_state_manual_search_mode_pref_sim[reg_as_id]
#else
static sys_rat_pri_list_info_s_type  reg_state_manual_rat_pri_list_info_sim[MAX_NAS_STACKS];
#define reg_state_manual_rat_pri_list_info reg_state_manual_rat_pri_list_info_sim[reg_as_id]
#endif

#else
extern reg_cm_network_list_req_e_type reg_last_recieved_list_req_type;
#ifndef FEATURE_RAT_PRIORITY_LIST
#ifndef FEATURE_GTOW_BACKGROUND_PLMN_SEARCH
static boolean                       reg_state_manual_search_complete = FALSE;
static sys_radio_access_tech_e_type  reg_state_manual_search_rat;
#endif
static sys_mode_pref_e_type          reg_state_manual_search_mode_pref;
#else
static sys_rat_pri_list_info_s_type  reg_state_manual_rat_pri_list_info;
#endif

#endif /* FEATURE_DUAL_SIM || FEATURE_SGLTE*/

#ifdef FEATURE_CIOT 
extern boolean mm_check_connection_active 
( 
   sys_modem_as_id_e_type sub_id, 
   sys_modem_as_id_e_type stack_id_nas 
); 
#endif


//#ifdef FEATURE_CIOT
//sys_detailed_plmn_list_s_type detailed_list;
//#endif
/*==============================================================================

                          STATIC FUNCTION DEFINITIONS

==============================================================================*/





/*==============================================================================

FUNCTION NAME

  process_hplmn_search_timer_expiration

==============================================================================*/

reg_state_e_type process_hplmn_search_timer_expiration
(
  void
)
{
  sys_radio_access_tech_e_type rat = SYS_RAT_GSM_RADIO_ACCESS;
  reg_state_e_type             next_state_p = REG_STATE_IDLE;
  boolean                      dual_mode = FALSE;
  uint64 restart_timer = 0;
#ifdef FEATURE_RAT_PRIORITY_LIST
  sys_rat_pri_list_info_s_type *rat_pri_list_info;
#endif
#ifdef FEATURE_CIOT
sys_detailed_plmn_list_s_type detailed_list;
#endif
#if defined(FEATURE_GTOW_BACKGROUND_PLMN_SEARCH ) || defined(FEATURE_RAT_PRIORITY_LIST)
  sys_plmn_rat_list_s_type *higher_priority_plmn_list = NULL;

#ifdef FEATURE_CIOT
  /* Determine if the timer needs to be restarted */
  restart_timer = reg_state_calculate_restart_value();
  if (restart_timer != 0)
  {
      reg_timers_start_hplmn_search_timer_value((dword)restart_timer);
      return next_state_p;
  }
#endif

#ifdef FEATURE_MODEM_HEAP
  higher_priority_plmn_list = (sys_plmn_rat_list_s_type *)modem_mem_alloc(sizeof(sys_plmn_rat_list_s_type), MODEM_MEM_CLIENT_NAS);
#else
  higher_priority_plmn_list = (sys_plmn_rat_list_s_type *)gs_alloc(sizeof(sys_plmn_rat_list_s_type));
#endif
  reg_check_for_null_ptr((void*)higher_priority_plmn_list );
  higher_priority_plmn_list->length = 0;
#endif
  
  memset(&detailed_list, 0, sizeof(sys_detailed_plmn_list_s_type));

#ifdef FEATURE_RAT_PRIORITY_LIST
  rat_pri_list_info = reg_mode_rat_pri_list_get();
#endif

  switch (reg_mode_network_selection_mode_get())
  {
    case SYS_NETWORK_SELECTION_MODE_AUTOMATIC:
    case SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION:
      if ((reg_state_service_status_get() == SYS_SRV_STATUS_SRV) ||
          (reg_state_service_status_get() == SYS_SRV_STATUS_LIMITED) ||
          (reg_state_service_status_get() == SYS_SRV_STATUS_LIMITED_REGIONAL) )
      {
        if (reg_mode_higher_priority_network_available
              (
#if defined(FEATURE_GTOW_BACKGROUND_PLMN_SEARCH ) || defined(FEATURE_RAT_PRIORITY_LIST)
                reg_state_active_rat_get(),
                higher_priority_plmn_list
#endif
              ))
        {
#ifndef FEATURE_RAT_PRIORITY_LIST
          dual_mode = (boolean)((reg_mode_req_mode_pref_get() == SYS_MODE_PREF_GSM_RADIO_ACCESS_PREFERRED) ||
                                (reg_mode_req_mode_pref_get() == SYS_MODE_PREF_UMTS_RADIO_ACCESS_PREFERRED));
#else
          dual_mode = ((reg_mode_is_rat_present_in_rat_pri_list(SYS_RAT_GSM_RADIO_ACCESS, rat_pri_list_info, FALSE))&&
                             ((reg_mode_is_rat_present_in_rat_pri_list(SYS_RAT_UMTS_RADIO_ACCESS, rat_pri_list_info, FALSE) ||
                               (reg_mode_is_rat_present_in_rat_pri_list(SYS_RAT_TDS_RADIO_ACCESS, rat_pri_list_info, FALSE)))));
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST 
          rat = reg_state_active_rat_get();
#else 
#ifdef FEATURE_GTOW_BACKGROUND_PLMN_SEARCH
          /*
          ** Until WCDMA to GSM background PLMN searching is supported all
          ** dual-mode PLMN searches will be initiated on GSM.
          ** WCDMA to GSM background PLMN search is supported so
          ** search is initiated on surrent active RAT.
          */
          switch (reg_mode_mode_pref_get())
          {
            case SYS_MODE_PREF_UMTS_RADIO_ACCESS_ONLY:
#ifdef FEATURE_UMTS_1X_HANDOVER_UMTSMSM 
            #error code not present
#endif
              rat = SYS_RAT_UMTS_RADIO_ACCESS;
              break;

            case SYS_MODE_PREF_GSM_RADIO_ACCESS_ONLY:
              rat = SYS_RAT_GSM_RADIO_ACCESS;
              break;

            case SYS_MODE_PREF_UMTS_RADIO_ACCESS_PREFERRED:
            case SYS_MODE_PREF_GSM_RADIO_ACCESS_PREFERRED:
            default:
#ifdef FEATURE_WTOG_BACKGROUND_PLMN_SEARCH
              rat = reg_state_active_rat_get();
#else
              rat = SYS_RAT_GSM_RADIO_ACCESS;
#endif
              break;
          }
#else // FEATURE_GTOW_BACKGROUND_PLMN_SEARCH
          rat = reg_state_active_rat_get();
#endif //FEATURE_GTOW_BACKGROUND_PLMN_SEARCH
#endif //FEATURE_RAT_PRIORITY_LIST

          reg_state_automatic_search_initiated
          (
            dual_mode,
            rat
          );      
          /* For a single rat, use BPLMN so that UE could remain camped*/
          if (rat_pri_list_info->num_items != 1)
          {         
              boolean selection_started= FALSE; 
              /* Forward searches */
              reg_mode_create_available_plmn_list_ciot(higher_priority_plmn_list, &detailed_list);
              selection_started = reg_mode_start_higher_priority_plmn_selection(&detailed_list);
              if(selection_started == TRUE )	
			  {
				  next_state_p = REG_STATE_REGISTERING;
			  }
			  else
			  {
			      reg_state_reset_psm_alignment();
			  }
          }
          else
          {
              reg_send_mmr_plmn_search_req
                  (
                  SYS_NETWORK_SELECTION_MODE_AUTOMATIC, /* network_selection_mode      */
                  dual_mode,                            /* dual_mode                   */
                  rat                                   /* rat                         */
#ifdef FEATURE_GSM_BAND_PREF
                  ,reg_mode_band_pref_get()             /* band_pref                   */
#endif
#if defined(FEATURE_GTOW_BACKGROUND_PLMN_SEARCH ) || defined(FEATURE_RAT_PRIORITY_LIST)
                  ,higher_priority_plmn_list           /* higher_priority_plmn_list_p */
#endif
#if defined(FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH) || defined(FEATURE_LTE)
                  ,SYS_SERVICE_SEARCH_NONE 
#endif
#ifdef FEATURE_FEMTO_CSG
                  ,SYS_CSG_SEARCH_TYPE_NONE
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
                  ,rat_pri_list_info
                  ,NULL 
#endif
                  ,  SYS_SCAN_SCOPE_FULL_BAND                      
                  , NULL
#ifdef FEATURE_GPS_WWAN_PRIORITY_SEARCH				  
                  , NULL
#endif
                  );
              next_state_p = REG_STATE_AUTOMATIC_SEARCH;
          }

#ifdef FEATURE_GW_ON_DEMAND_HPLMN_SRCH
          if (reg_state_idle_prev_nw_sel_mode != SYS_NETWORK_SELECTION_MODE_NONE)
          {
              reg_send_cm_hplmn_search_cnf(TRUE);
          }
#endif
#if defined(FEATURE_GTOW_BACKGROUND_PLMN_SEARCH ) || defined(FEATURE_RAT_PRIORITY_LIST)
          /*
          ** send LOG packet for starting of higher priority PLMN selecton.
          */
          reg_log_umts_nas_hplmn_search_start(higher_priority_plmn_list);
#endif
        }
        else
        {
#ifdef FEATURE_GW_ON_DEMAND_HPLMN_SRCH
          if (reg_state_idle_prev_nw_sel_mode == SYS_NETWORK_SELECTION_MODE_NONE)
          {
#endif
            /* If MS on HPLMN do not start the timer but reset so that next time timer is 2 min*/ 
            if(reg_state_roaming_ind_get() == SYS_ROAM_STATUS_ON)
            {
              reg_timers_start_hplmn_search_period_timer();
            }

#ifdef FEATURE_GW_ON_DEMAND_HPLMN_SRCH
          }
          else
          {
            MSG_HIGH_DS_0(REG_SUB, "=REG= Ignore: HPLMN Search - NO higher priority list found in PPLMN (SIM/USIM)");
            reg_send_cm_hplmn_search_cnf(FALSE);
          }
#endif

#ifdef FEATURE_CIOT           
           /* No HP network available, clear any PSM alignment flags*/
           reg_state_reset_psm_alignment();
#endif
          next_state_p = REG_STATE_IDLE;
        }
      }
#if defined (FEATURE_GW_ON_DEMAND_HPLMN_SRCH) || defined (FEATURE_REL7_STEERING_OF_ROAMING)
      else
      {
#ifdef FEATURE_GW_ON_DEMAND_HPLMN_SRCH
        if (reg_state_idle_prev_nw_sel_mode != SYS_NETWORK_SELECTION_MODE_NONE)
        {
          MSG_HIGH_DS_1(REG_SUB, "=REG= Ignore: HPLMN Search - Not allowed in NO SERVICE status %d", reg_state_service_status_get());
          reg_send_cm_hplmn_search_cnf(FALSE);
        }
#endif
#ifdef FEATURE_CIOT
       /* No HP search, clear any PSM alignment flags*/
        reg_state_reset_psm_alignment();
#endif
      }
#endif
      break;

    case SYS_NETWORK_SELECTION_MODE_MANUAL:
    case SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION:
      MSG_ERROR_DS_0(REG_SUB, "=REG= HPLMN Search Timer expired - manual mode"); 
      break;

    case SYS_NETWORK_SELECTION_MODE_LIMITED:
      MSG_ERROR_DS_0(REG_SUB, "=REG= HPLMN Search Timer expired - limited mode"); 
      break;

    default:
      MSG_ERROR_DS_1(REG_SUB, "=REG= HPLMN Search Timer expired - mode (%d)", reg_mode_network_selection_mode_get()); 
#ifdef FEATURE_CIOT
       /* No HP search, clear any PSM alignment flags*/
        reg_state_reset_psm_alignment();
#endif
      break;
  }

#ifdef FEATURE_GW_ON_DEMAND_HPLMN_SRCH
  reg_state_idle_prev_nw_sel_mode = SYS_NETWORK_SELECTION_MODE_NONE;
#endif
#if defined(FEATURE_GTOW_BACKGROUND_PLMN_SEARCH ) || defined(FEATURE_RAT_PRIORITY_LIST)
#ifndef FEATURE_MODEM_HEAP
   gs_free(higher_priority_plmn_list);
#else
   modem_mem_free(higher_priority_plmn_list, MODEM_MEM_CLIENT_NAS);
#endif
   higher_priority_plmn_list = NULL;
#endif
  return next_state_p;
}



#if defined (FEATURE_TDSCDMA) || defined (FEATURE_SGLTE)
/*==============================================================================

FUNCTION NAME

  process_hplmn_irat_search_timer_expiration

==============================================================================*/
static reg_state_e_type process_hplmn_irat_search_timer_expiration
(
  void
)
{
  sys_radio_access_tech_e_type rat = SYS_RAT_GSM_RADIO_ACCESS;
  reg_state_e_type             next_state_p = REG_STATE_IDLE;
  sys_rat_pri_list_info_s_type *rat_pri_list_info_ptr, rat_pri_list_info;

  sys_plmn_rat_list_s_type*     list = NULL;
  reg_sim_plmn_list_s_type*     efs_list = NULL;
  uint32 i, j;
   
  boolean cell_not_present_in_lists;

  cell_not_present_in_lists = reg_state_cell_not_present_in_lte_lists();

  reg_state_set_lte_search_requested(FALSE);


  if ( (reg_nv_is_forced_irat_enabled() /* legacy condition */
#ifdef FEATURE_SGLTE
      &&
      (! reg_sglte_mode))
       ||
       ((reg_sglte_mode && (reg_state_active_rat_get() == SYS_RAT_TDS_RADIO_ACCESS))) /* SGLTE state 2 condition */
       ||
       ((IS_UE_IN_SGLTE_STATE_4 && (reg_state_get_plmn_service_capability ((sys_modem_as_id_e_type)reg_cs_stack_id) == SYS_SRV_CAPABILITY_CS_PS_SERVICE)) /* SGLTE state 4 condition */
#endif
     ))  
  {
      /*The below condition checkes the following, be careful before editing it
             1. Return immediately if LTE or TDS is not present in rat priority list(no rat is there to be searched)
             2. Return if cell is present in list ( avaiable or not avaiable) and active rat is TDS(this means that LTE irat timer is running for LTE search)
             3. If cell is present in avaiable list and active rat is GSM then return if mode is CSFB
      */
      rat_pri_list_info_ptr = reg_mode_rat_pri_list_get();

      if((!( reg_mode_is_rat_present_in_rat_pri_list(SYS_RAT_LTE_RADIO_ACCESS, rat_pri_list_info_ptr, FALSE)) &&
           (!reg_mode_is_rat_present_in_rat_pri_list(SYS_RAT_TDS_RADIO_ACCESS, rat_pri_list_info_ptr, FALSE))) ||
             ( cell_not_present_in_lists == FALSE  
#ifdef FEATURE_SGLTE
                && (reg_state_active_rat_get() == SYS_RAT_TDS_RADIO_ACCESS || (!reg_sglte_mode))
#endif
        ))
      {
          return next_state_p;
      }

      switch (reg_mode_network_selection_mode_get())
      {

      case SYS_NETWORK_SELECTION_MODE_LIMITED:
          return next_state_p;
          break;

      default:

          if (reg_state_service_status_get() == SYS_SRV_STATUS_SRV)
          {
              /* Check if UE is camped on a RAT for which further processing
              ** should proceed
              */
              rat = reg_state_active_rat_get();
              if((cell_not_present_in_lists && rat != SYS_RAT_TDS_RADIO_ACCESS) && ( rat != SYS_RAT_GSM_RADIO_ACCESS))
              {
                  return next_state_p;
              } 
              /* At this stage, UE is in full service on TDSCDMA or GSM.
              ** Determine if the PLMN matches one of the PLMNs stored in 
              ** the EFS EHPLMN by checking the roaming ind
              */

              if (reg_state_roaming_ind_get() == SYS_ROAM_STATUS_ON)
              {
                  /* Not camped on EHPLMN, return*/
                  return next_state_p;
              }
              /* All conditions are met. Prepare the rat priority list accordingly */
              memset(&rat_pri_list_info, 0, sizeof(rat_pri_list_info));
              /* Search for LTE is allowed for legacy case and for SGLTE state 2*/
              if ( (reg_nv_is_forced_irat_enabled()
#ifdef FEATURE_SGLTE
                   &&
                   ((!REG_SUB_IS_SGLTE || ! reg_sglte_mode))) 
                  ||
                  (REG_SUB_IS_SGLTE && reg_sglte_mode 
                    && (cell_not_present_in_lists && reg_state_active_rat_get() == SYS_RAT_TDS_RADIO_ACCESS) /* SGLTE state 2 condition */
#endif
              ))              
              {
                /* Copy the LTE related information from the rat priority list*/
                for(i = 0; i < rat_pri_list_info_ptr->num_items; i++)
                {
                    if (rat_pri_list_info_ptr->priority_list_info[i].acq_sys_mode == SYS_SYS_MODE_LTE)
                    {
                        rat_pri_list_info.priority_list_info[0].acq_sys_mode = SYS_SYS_MODE_LTE;
                        rat_pri_list_info.priority_list_info[0].band_cap = 
                            rat_pri_list_info_ptr->priority_list_info[i].band_cap;
                        rat_pri_list_info.priority_list_info[0].bst_band_cap = 
                            rat_pri_list_info_ptr->priority_list_info[i].bst_band_cap;
                        rat_pri_list_info.num_items = 1;
                        break;
                    }
                }
              }
              else
              {
                 /* This must be SGLTE state 4*/
                  /* Copy the LTE and TD related information from the rat priority list*/
                  for(i = 0; i < rat_pri_list_info_ptr->num_items; i++)
                  {
                      if (rat_pri_list_info_ptr->priority_list_info[i].acq_sys_mode == SYS_SYS_MODE_LTE )
                      {
                          rat_pri_list_info.priority_list_info[rat_pri_list_info.num_items].acq_sys_mode = SYS_SYS_MODE_LTE;
                          rat_pri_list_info.priority_list_info[rat_pri_list_info.num_items].band_cap = rat_pri_list_info_ptr->priority_list_info[i].band_cap;
                          rat_pri_list_info.priority_list_info[rat_pri_list_info.num_items].bst_band_cap = rat_pri_list_info_ptr->priority_list_info[i].bst_band_cap;
                          rat_pri_list_info.num_items++;
                      }
                      else if (rat_pri_list_info_ptr->priority_list_info[i].acq_sys_mode == SYS_SYS_MODE_TDS)
                      {
                          rat_pri_list_info.priority_list_info[rat_pri_list_info.num_items].acq_sys_mode = SYS_SYS_MODE_TDS;
                          rat_pri_list_info.priority_list_info[rat_pri_list_info.num_items].band_cap = rat_pri_list_info_ptr->priority_list_info[i].band_cap;
                          rat_pri_list_info.priority_list_info[rat_pri_list_info.num_items].bst_band_cap = rat_pri_list_info_ptr->priority_list_info[i].bst_band_cap;
                          rat_pri_list_info.num_items++;
                      }
                  }
              }
              
#ifdef FEATURE_MODEM_HEAP
              list = (sys_plmn_rat_list_s_type *)modem_mem_alloc(sizeof(sys_plmn_rat_list_s_type), MODEM_MEM_CLIENT_NAS);
#else
              list = (sys_plmn_rat_list_s_type *)gs_alloc(sizeof(sys_plmn_rat_list_s_type));
#endif
              reg_check_for_null_ptr((void*)list );
              list->length = 0;
              /* The TDSCDMA operators are expected to populate the specific list of PLMNs
              ** in the EFS.
              */       
              efs_list = reg_nv_tdscdma_plmn_list_get();
              efs_list->length = MIN (efs_list->length, REG_SIM_PLMN_LIST_LENGTH);
              for(i = 0; (i < efs_list->length && list->length < SYS_PRIORITY_PLMN_LIST_MAX_LENGTH); i++)
              {
                for (j = 0; j < rat_pri_list_info.num_items; j++)
                {
                  list->info[list->length].plmn = efs_list->plmn[i];
                  if (rat_pri_list_info.priority_list_info[j].acq_sys_mode == SYS_SYS_MODE_TDS)
                  {
                    list->info[list->length].rat = SYS_RAT_TDS_RADIO_ACCESS;
                  }
                  else if( rat_pri_list_info.priority_list_info[j].acq_sys_mode == SYS_SYS_MODE_LTE &&
                            (cell_not_present_in_lists
#ifdef FEATURE_SGLTE
                             || (IS_UE_IN_SGLTE_STATE_4 && reg_mode_is_rat_present_in_rat_pri_list(SYS_RAT_LTE_RADIO_ACCESS, rat_pri_list_info_ptr, FALSE))
#endif
                             )
                           )
                  {
                    reg_state_set_lte_search_requested(TRUE);
                    list->info[list->length].rat = SYS_RAT_LTE_RADIO_ACCESS;
                  }
                  list->length++;
                }
              }

              MSG_HIGH_DS_2(REG_SUB, "=REG= HPLMN Search started with RAT length - %d, hpplmn list length - %d", 
                            rat_pri_list_info.num_items, list->length);

              reg_send_mmr_plmn_search_req
              (
                SYS_NETWORK_SELECTION_MODE_AUTOMATIC, /* Background PLMN search      */
                TRUE,                            /* dual_mode                   */
                rat                                   /* rat                         */
  #ifdef FEATURE_GSM_BAND_PREF
                ,reg_mode_band_pref_get()             /* band_pref                   */
  #endif
                ,list
                ,SYS_SERVICE_SEARCH_IRAT_PLMN
#ifdef FEATURE_FEMTO_CSG
                ,SYS_CSG_SEARCH_TYPE_NONE
#endif              
                ,&rat_pri_list_info
                ,NULL                                 
                ,  SYS_SCAN_SCOPE_FULL_BAND                             
                , NULL        
#ifdef FEATURE_GPS_WWAN_PRIORITY_SEARCH				
                , NULL
#endif				
              );
#ifndef FEATURE_MODEM_HEAP
              gs_free(list);
#else
              modem_mem_free(list, MODEM_MEM_CLIENT_NAS);
#endif
              list = NULL;
              /* Move to another REG state*/
              next_state_p = REG_STATE_AUTOMATIC_SEARCH;
          } /* SYS_SRV_STATUS_SRV */
          break;
      }
  }
  return next_state_p;
}

#if defined FEATURE_TDSCDMA || defined FEATURE_SGLTE
/*==============================================================================

FUNCTION NAME

  process_lte_irat_search_timer_expiration

==============================================================================*/
static reg_state_e_type process_lte_irat_search_timer_expiration
(
  void
)
{
  sys_radio_access_tech_e_type rat = SYS_RAT_GSM_RADIO_ACCESS;
  reg_state_e_type             next_state_p = REG_STATE_IDLE;
  sys_rat_pri_list_info_s_type *rat_pri_list_info_ptr, rat_pri_list_info;

  sys_plmn_rat_list_s_type*     list = NULL;
  reg_sim_plmn_list_s_type*     efs_list = NULL;
  uint32 i, j;
  boolean cell_not_present_in_lists = FALSE;

  if(!reg_state_get_force_bplmn_search_start())
  {
    cell_not_present_in_lists = reg_state_cell_not_present_in_lte_lists();
  }
  if(cell_not_present_in_lists && !(reg_state_get_force_irat_search_initiated()))
  {
    MSG_HIGH_DS_0(REG_SUB, "=REG= LTE IRAT BPLMN : cell not present in any of the LTE lists, ignoring expiry");
    return next_state_p;
  }
  else
  {
    if ( (reg_nv_is_forced_irat_enabled() /* legacy condition */
#ifdef FEATURE_SGLTE
      &&
      (! reg_sglte_mode))
       ||
       ((reg_sglte_mode && (reg_state_active_rat_get() == SYS_RAT_TDS_RADIO_ACCESS))) /* SGLTE state 2 condition */
       ||
       ((IS_UE_IN_SGLTE_STATE_4 && (reg_state_get_plmn_service_capability ((sys_modem_as_id_e_type)reg_cs_stack_id) == SYS_SRV_CAPABILITY_CS_PS_SERVICE)) /* SGLTE state 4 condition */
#endif
     ))  
  {
      reg_state_set_lte_search_requested(FALSE);
      /* Return immediately if there is no LTE or TDS in rat priority list */
      rat_pri_list_info_ptr = reg_mode_rat_pri_list_get();

      if (!reg_mode_is_rat_present_in_rat_pri_list(SYS_RAT_LTE_RADIO_ACCESS, rat_pri_list_info_ptr, FALSE))
      {
          return next_state_p;
      }

      switch (reg_mode_network_selection_mode_get())
      {

      case SYS_NETWORK_SELECTION_MODE_LIMITED:
          return next_state_p;
          break;

      default:

          if (reg_state_service_status_get() == SYS_SRV_STATUS_SRV)
          {
              /* Check if UE is camped on a RAT for which further processing
              ** should proceed
              */
              rat = reg_state_active_rat_get();
              if((rat != SYS_RAT_TDS_RADIO_ACCESS) && (rat != SYS_RAT_GSM_RADIO_ACCESS))
              {
                  return next_state_p;
              } 
              /* At this stage, UE is in full service on TDSCDMA or GSM.
              ** Determine if the PLMN matches one of the PLMNs stored in 
              ** the EFS EHPLMN by checking the roaming ind
              */

              if (reg_state_roaming_ind_get() == SYS_ROAM_STATUS_ON)
              {
                  /* Not camped on EHPLMN, return*/
                  return next_state_p;
              }
              /* All conditions are met. Prepare the rat priority list accordingly */
              memset(&rat_pri_list_info, 0, sizeof(rat_pri_list_info));
              /* Search for LTE is allowed for legacy case and for SGLTE state 2*/
              if ( (reg_nv_is_forced_irat_enabled()
#ifdef FEATURE_SGLTE
                   &&
                   ((!REG_SUB_IS_SGLTE || ! reg_sglte_mode))) 
                  ||
                  (REG_SUB_IS_SGLTE && reg_sglte_mode 
                    && (reg_state_active_rat_get() == SYS_RAT_TDS_RADIO_ACCESS) /* SGLTE state 2 condition */
#endif
              ))              
              {
                /* Copy the LTE related information from the rat priority list*/
                for(i = 0; i < rat_pri_list_info_ptr->num_items; i++)
                {
                    if (rat_pri_list_info_ptr->priority_list_info[i].acq_sys_mode == SYS_SYS_MODE_LTE)
                    {
                        rat_pri_list_info.priority_list_info[0].acq_sys_mode = SYS_SYS_MODE_LTE;
                        rat_pri_list_info.priority_list_info[0].band_cap = 
                            rat_pri_list_info_ptr->priority_list_info[i].band_cap;
                        rat_pri_list_info.priority_list_info[0].bst_band_cap = 
                            rat_pri_list_info_ptr->priority_list_info[i].bst_band_cap;
                        rat_pri_list_info.num_items = 1;
                        break;
                    }
                }
              }
              else
              {
                 /* This must be SGLTE state 4*/
                  /* Copy the LTE and TD related information from the rat priority list*/
                  for(i = 0; i < rat_pri_list_info_ptr->num_items; i++)
                  {
                      if (rat_pri_list_info_ptr->priority_list_info[i].acq_sys_mode == SYS_SYS_MODE_LTE)
                      {
                          rat_pri_list_info.priority_list_info[rat_pri_list_info.num_items].acq_sys_mode = SYS_SYS_MODE_LTE;
                          rat_pri_list_info.priority_list_info[rat_pri_list_info.num_items].band_cap = rat_pri_list_info_ptr->priority_list_info[i].band_cap;
                          rat_pri_list_info.priority_list_info[rat_pri_list_info.num_items].bst_band_cap = rat_pri_list_info_ptr->priority_list_info[i].bst_band_cap;
                          rat_pri_list_info.num_items++;
                      }
                  }
              }
              
           
#ifdef FEATURE_MODEM_HEAP
			  list = (sys_plmn_rat_list_s_type *)modem_mem_alloc(sizeof(sys_plmn_rat_list_s_type), MODEM_MEM_CLIENT_NAS);
#else
			  list = (sys_plmn_rat_list_s_type *)gs_alloc(sizeof(sys_plmn_rat_list_s_type));
#endif
			  if(list == NULL)
			  {
			    MSG_FATAL_DS(REG_SUB, "=REG= Error: Not able to Allocate list Buffer", 0, 0, 0);
			  }
			  list->length = 0;          
              /* The TDSCDMA operators are expected to populate the specific list of PLMNs
              ** in the EFS.
              */       
              efs_list = reg_nv_tdscdma_plmn_list_get();
              efs_list->length = MIN (efs_list->length, REG_SIM_PLMN_LIST_LENGTH);
              for(i = 0; (i < efs_list->length && list->length < SYS_PRIORITY_PLMN_LIST_MAX_LENGTH); i++)
              {
                for (j = 0; j < rat_pri_list_info.num_items; j++)
              {
                  list->info[list->length].plmn = efs_list->plmn[i];
                  if (rat_pri_list_info.priority_list_info[j].acq_sys_mode == SYS_SYS_MODE_LTE)
                  {
                    list->info[list->length].rat = SYS_RAT_LTE_RADIO_ACCESS;
                  }
                  list->length++;
                }
              }

              reg_state_set_lte_search_requested(TRUE);
              MSG_HIGH_DS_2(REG_SUB, "=REG= LTE IRAT BPLMN: LTE IRAT Search started with RAT length - %d, hpplmn list length - %d", 
                            rat_pri_list_info.num_items, list->length);

              reg_send_mmr_plmn_search_req
              (
                SYS_NETWORK_SELECTION_MODE_AUTOMATIC, /* Background PLMN search      */
                TRUE,                            /* dual_mode                   */
                rat                                   /* rat                         */
  #ifdef FEATURE_GSM_BAND_PREF
                ,reg_mode_band_pref_get()             /* band_pref                   */
  #endif
                ,list
                ,SYS_SERVICE_SEARCH_IRAT_PLMN
#ifdef FEATURE_FEMTO_CSG
                ,SYS_CSG_SEARCH_TYPE_NONE
#endif              
                ,&rat_pri_list_info
                ,NULL
                ,  SYS_SCAN_SCOPE_FULL_BAND                             
                , NULL
#ifdef FEATURE_GPS_WWAN_PRIORITY_SEARCH				
                , NULL
#endif
              );

              /* Move to another REG state*/
              next_state_p = REG_STATE_AUTOMATIC_SEARCH;
          } /* SYS_SRV_STATUS_SRV */
          break;
      }
  }
}

#ifndef FEATURE_MODEM_HEAP
	gs_free(list);
#else
	modem_mem_free(list, MODEM_MEM_CLIENT_NAS);
#endif
	list = NULL;
  
  return next_state_p;
}

#endif
#endif


#ifdef FEATURE_LTE
/*==============================================================================

FUNCTION NAME

  process_backoff_fplmn_timer_expiration

==============================================================================*/
reg_state_e_type process_backoff_fplmn_timer_expiration
(
  void
)
{
  reg_sim_handle_backoff_fplmn_search_timer_expiry();
  if(reg_mode_reenable_lte())
  {
    MSG_HIGH_DS_0(REG_SUB, "=REG= Initiate PLMN selection to enable LTE RAT capability");
    reg_mode_start_plmn_selection_on_specified_plmn
    (
      reg_state_service_plmn_get(),
      reg_mode_network_selection_mode_get(),
      reg_mode_mode_pref_get(),
      FALSE,
      FALSE,
      reg_mode_req_service_domain_get(),
#ifdef FEATURE_GSM_BAND_PREF
      reg_mode_band_pref_get(),
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
      reg_mode_curr_rat_pri_list_get(),
#endif
      reg_state_active_rat_get(),
#ifdef FEATURE_LTE
      reg_mode_sms_only_get(),
       0,
#endif 
      MMR_REG_REQ_NORMAL
    );
    return REG_STATE_REGISTERING;
  }
  return REG_STATE_IDLE;
}


#endif


#ifdef FEATURE_SGLTE
static reg_state_e_type process_sglte_ps_timer_expiry(void)
{
  reg_as_id_e_type reg_other_as_id;
  reg_other_as_id = ((reg_as_id == reg_sub_id) ? REG_SGLTE_STACK_2: reg_sub_id);
  if( REG_SGLTE_DUAL_STACK &&
      reg_state_get_service_status_per_subs((sys_modem_as_id_e_type)reg_other_as_id) == SYS_SRV_STATUS_SRV && 
      reg_state_get_service_status_per_subs((sys_modem_as_id_e_type)reg_as_id) != SYS_SRV_STATUS_SRV  && 
      reg_state_get_service_domain_per_subs((sys_modem_as_id_e_type)reg_other_as_id) == SYS_SRV_DOMAIN_CS_ONLY &&
      reg_state_get_plmn_service_capability((sys_modem_as_id_e_type)reg_other_as_id) == SYS_SRV_CAPABILITY_CS_PS_SERVICE &&
      reg_state_get_active_rat_per_subs((sys_modem_as_id_e_type)reg_other_as_id) == SYS_RAT_GSM_RADIO_ACCESS &&
      /*Adding CS stack id to get PLMN , becuase during split process finished or dual stack mode
       CS stack PLMN is to check for forbidden PLMN */
#ifdef FEATURE_DUAL_SIM
      (reg_sim_plmn_gprs_forbidden(reg_state_service_plmn_get_per_subs((sys_modem_as_id_e_type)reg_other_as_id), (sys_modem_as_id_e_type)reg_sub_id) == FALSE )
#else
      (reg_sim_plmn_gprs_forbidden(reg_state_service_plmn_get_per_subs(reg_other_as_id))==FALSE )
#endif
    )
  {

#ifdef FEATURE_SGLTE_DUAL_SIM
      if (! mm_get_verify_ps_transfer_conditions((sys_modem_as_id_e_type)reg_other_as_id))
      {
          /* Call must be pending, start the timer instead*/
          MSG_HIGH_DS_0(REG_SUB, "=REG= Call pending, start PS timer"); 
          reg_timers_start_sglte_ps_timer_val (5000);
          return REG_STATE_IDLE;
      }
#endif           

    reg_send_ps_timer_expiry();
  }
  else
  {
    MSG_ERROR_DS_0(REG_SUB, "=REG= SGLTE PS Timer - Unexpected");
  }
  return REG_STATE_IDLE;
}

#endif

#ifdef FEATURE_FEMTO_CSG
/*==============================================================================

FUNCTION NAME

  process_mmr_csg_list_read_complete
==============================================================================*/
static reg_state_e_type process_mmr_csg_list_read_complete
(
 void
)
{
  reg_state_e_type next_state_p = REG_STATE_IDLE;
  sys_csg_info_s_type  csg_info = reg_state_csg_info_get();
  reg_sim_set_csg_list_read_complete(TRUE);

  if((reg_state_power_on_csg_search == TRUE) && 
     (reg_state_service_status_get() == SYS_SRV_STATUS_SRV))
  {
    if((csg_info.csg_id == SYS_CSG_ID_INVALID) || 
       (csg_info.hybrid_cell == TRUE))
    {
      reg_send_cm_csg_search_trigger_ind();
    }
    reg_state_power_on_csg_search = FALSE; 
  }
  return next_state_p;
}
#endif 
#ifdef FEATURE_CIOT
/*==============================================================================

FUNCTION NAME

  process_irat_ciot_search_timer_expiry
==============================================================================*/
reg_state_e_type process_irat_ciot_search_timer_expiry (void)
{
    uint64 restart_timer;
    reg_state_e_type next_state_p = REG_STATE_IDLE;
    
    /* Determine if the timer needs to be restarted */
    restart_timer = reg_state_calculate_restart_value();
    if (restart_timer != 0)
    {
      reg_timers_start_ciot_irat_search_period_timer_value((dword)restart_timer);
      return next_state_p;
    }

    next_state_p = REG_STATE_REGISTERING;
    reg_mode_start_higher_priority_irat_ciot();
    
    return next_state_p;
}
#endif


/*==============================================================================

                              FUNCTION DEFINITIONS

==============================================================================*/


/*==============================================================================

FUNCTION NAME

  process_auto_mmr_plmn_search_cnf

==============================================================================*/

reg_state_e_type process_auto_mmr_plmn_search_cnf
(
  const mmr_plmn_search_cnf_s_type* msg_p
)
{
  reg_state_e_type next_state_p = REG_STATE_IDLE;

  if (msg_p->transaction_id != reg_state_plmn_search_transaction_id_get())
  {
    MSG_HIGH_DS_2(REG_SUB, "=REG= Ignore MMR_PLMN_SEARCH_CNF TI (%d) mismatch with TI (%d)", 
                  msg_p->transaction_id, reg_state_plmn_search_transaction_id_get());
    return REG_STATE_AUTOMATIC_SEARCH; 
  }
  #ifdef FEATURE_TDSCDMA
  if (msg_p->service_search == SYS_SERVICE_SEARCH_IRAT_PLMN)
  {  
     /* Determine if the request for PLMN list search was
     ** for forced IRAT by checking if the current PLMN is EHPLMN
     */
     if (reg_state_roaming_ind_get() == SYS_ROAM_STATUS_OFF)
     {       
       if(msg_p->plmn_search_result != REG_MM_PLMN_SEARCH_SUCCESS)
       {
          reg_state_start_irat_timers_with_10sec();
         next_state_p = REG_STATE_IDLE;
       }
       else
       {
        if (reg_state_get_lte_search_requested())
        {
          reg_state_set_force_irat_search_initiated(FALSE);
        } 
        if (reg_mode_start_higher_priority_irat_plmn_selection(&msg_p->available_plmn_list))  
        {    
             next_state_p = REG_STATE_REGISTERING;  
        }
      }
    }
  }
  else 
  {
#endif
    
    /*
    ** Abort the higher priority search if MM failed to initiate the search
    ** because the phone was in a call.
    */
    if (msg_p->plmn_search_result != REG_MM_PLMN_SEARCH_SUCCESS)
    {
      /*
      ** Calling the stop function insures the timer is set to its minimum
      ** value (2 minutes) when it is started.
      */
      reg_timers_stop_hplmn_search_period_timer();
  
      /*
      ** Start 30 sec timer to initiate the search ASAP.
      reg_timers_start_hplmn_search_period_timer();
      */
      reg_timers_start_hplmn_search_timer_10sec();
  
      next_state_p = REG_STATE_IDLE;
    }
    /*
    ** If a higher priority network was found then PLMN selection is started.
    */
    else if (reg_mode_start_higher_priority_plmn_selection(&msg_p->found_plmn_list.plmn_list))
    {
  #ifdef FEATURE_ENHANCED_NW_SELECTION
      reg_state_delay_fcn_act_hplmn_set(FALSE);
  #endif
      next_state_p = REG_STATE_REGISTERING;
    }
  #ifndef FEATURE_RAT_PRIORITY_LIST
  #ifndef FEATURE_GTOW_BACKGROUND_PLMN_SEARCH
    /*
    ** Initiate a search on the second RAT.
    */
    else if (!reg_state_automatic_search_complete)
    {
      if (reg_state_automatic_search_rat == SYS_RAT_GSM_RADIO_ACCESS)
      {
        reg_state_automatic_search_rat = SYS_RAT_UMTS_RADIO_ACCESS;
      }
      else
      {
        reg_state_automatic_search_rat = SYS_RAT_GSM_RADIO_ACCESS;
      }
  
      reg_send_mmr_plmn_search_req
      (
        SYS_NETWORK_SELECTION_MODE_AUTOMATIC, /* network_selection_mode */
        FALSE,                                /* dual_mode              */
        reg_state_automatic_search_rat        /* rat                    */
  #ifdef FEATURE_GSM_BAND_PREF
        ,reg_mode_band_pref_get()             /* band_pref              */
  #endif
  #ifdef FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH
        ,SYS_SERVICE_SEARCH_NONE 
  #endif
#ifdef FEATURE_GPS_WWAN_PRIORITY_SEARCH  
        ,NULL
#endif
      );
  
      next_state_p = REG_STATE_AUTOMATIC_SEARCH;
    }
  #endif
  #endif/*FEATURE_LTE*/
  
  
  #ifndef FEATURE_RAT_PRIORITY_LIST
  #ifndef FEATURE_GTOW_BACKGROUND_PLMN_SEARCH
    reg_state_automatic_search_complete = TRUE;
  #endif
  #endif /*FEATURE_LTE*/
  if (reg_state_current_state_get() != REG_STATE_REGISTERING && msg_p->plmn_search_result == REG_MM_PLMN_SEARCH_SUCCESS)
  {
#ifdef FEATURE_CIOT    
          /* Indicate that the REG related procedures are complete now*/
          reg_state_set_timer_expired(FALSE);
          if (reg_state_get_psm_sleep_blocked())
          {
              reg_state_set_psm_sleep_blocked(FALSE);
              reg_send_cm_psm_unblock();
          }
#endif
  }
#ifdef FEATURE_TDSCDMA
  }
#endif 

  return next_state_p;
}

/*==============================================================================

FUNCTION NAME

  process_mmr_plmn_search_cnf

==============================================================================*/

reg_state_e_type process_manual_mmr_plmn_search_cnf
(
  const mmr_plmn_search_cnf_s_type* msg_p
)
{

#ifdef FEATURE_EQUIVALENT_HPLMN
  uint32                             i,j;
#endif

  sys_detailed_plmn_list_s_type     *available_plmn_list =  NULL;
  sys_cell_search_list_s_type       *local_cell_info_list = NULL;
  sys_found_plmn_list_u_type        *available_plmn_u_list =  NULL;
#ifdef FEATURE_GPS_WWAN_PRIORITY_SEARCH  
  sys_loc_wwan_scan_result_s_type* gps_wwan_search_result = NULL;
#endif
 // const sys_detailed_plmn_list_s_type  *available_list = NULL;
  uint32                             list_size = 0;

  reg_state_e_type next_state_p = REG_STATE_MANUAL_SEARCH;


#if defined (FEATURE_WTOG_BACKGROUND_PLMN_SEARCH) || defined (FEATURE_WTOW_BACKGROUND_PLMN_SEARCH) \
    || defined (FEATURE_RAT_PRIORITY_LIST)
#if defined(FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH) || defined(FEATURE_LTE)
  if (msg_p->network_selection_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC &&
      msg_p->service_search == SYS_SERVICE_SEARCH_NONE) 
#else
  if (msg_p->network_selection_mode != SYS_NETWORK_SELECTION_MODE_MANUAL)
#endif
  {
    MSG_HIGH_DS_0(REG_SUB, "=REG= Ignore MMR_PLMN_SEARCH_CNF of Automatic Search");
    /*
    ** Start 30 sec timer to initiate the search ASAP.
    ** Starting the timer as the previous automatic search is ignored.
    */
    if(reg_state_roaming_ind_get()== SYS_ROAM_STATUS_ON)
    {
       reg_timers_start_hplmn_search_timer_10sec();
    }
#ifdef FEATURE_TDSCDMA
    else
    { 
       /* Roaming is off if the IRAT HPLMN search is ongoing*/
      reg_state_start_irat_timers_with_10sec();

    }
#endif
    return next_state_p; 
  }
#endif

  if (msg_p->transaction_id != reg_state_plmn_search_transaction_id_get())
  {
    MSG_HIGH_DS_2(REG_SUB, "=REG= Ignore MMR_PLMN_SEARCH_CNF TI (%d) mismatch with TI (%d)", 
                  msg_p->transaction_id, reg_state_plmn_search_transaction_id_get());
    return next_state_p; 
  }
#ifdef FEATURE_MODEM_HEAP
	available_plmn_u_list = (sys_found_plmn_list_u_type *)modem_mem_calloc(1,sizeof(sys_found_plmn_list_u_type), MODEM_MEM_CLIENT_NAS);
#else
	available_plmn_u_list = (sys_found_plmn_list_u_type *)gs_alloc(sizeof(sys_found_plmn_list_u_type));
#endif
	reg_check_for_null_ptr((void*)available_plmn_u_list );
    available_plmn_list = &available_plmn_u_list->plmn_list; //extract PLMNs from received list into this
    local_cell_info_list = &available_plmn_u_list->cell_info_list; //extract cells from received list into this
	
#ifdef FEATURE_FEMTO_CSG
  if(msg_p->service_search == SYS_SERVICE_SEARCH_CSG)
  {

    reg_mode_update_available_csg_list
    (
      &reg_state_manual_rat_pri_list_info,
      &(msg_p->found_plmn_list.plmn_list)
    );
 
    list_size = sizeof(uint32) + ((reg_mode_available_plmn_list_manual_get())->length) * sizeof(sys_detailed_plmn_list_info_s_type);
    memscpy(available_plmn_list,list_size,(reg_mode_available_plmn_list_manual_get()),list_size);
    reg_send_cm_network_list_cnf
    (
      reg_state_cm_transaction_id_get(),
      reg_sim_read_rplmn(reg_mode_req_service_domain_get()),
      available_plmn_u_list,
      msg_p->plmn_search_result
    );
    next_state_p = REG_STATE_IDLE;
	reg_last_recieved_list_req_type = REG_CM_NETWORK_LIST_NONE;
  }
  else
#endif /*FEATURE_FEMTO_CSG*/
  {
   if(msg_p->service_search == SYS_SERVICE_SEARCH_CELL_SEARCH)
   	{  	  
	   reg_mode_consolidate_cell_list 
       (
          msg_p->found_plmn_list.cell_info_list
       );
   	}
#ifdef FEATURE_GPS_WWAN_PRIORITY_SEARCH
   else if(msg_p->service_search == SYS_SERVICE_SEARCH_GPS_ADVANCE_SEARCH)
    {
		MSG_HIGH_DS_0(REG_SUB, "=REG= Consolidate available plmn is not required");
   	}
#endif
    else
    {
       reg_mode_update_available_plmn_list_manual
    (
  #if defined(FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH) || defined(FEATURE_LTE)
      (msg_p->network_selection_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC &&
       msg_p->service_search == SYS_SERVICE_SEARCH_BACKGROUND_AUTOMATIC)?
         REG_MODE_AUTOMATIC_MODE_NORMAL:REG_MODE_AUTOMATIC_MODE_MANUAL,
  #else    
      REG_MODE_AUTOMATIC_MODE_MANUAL,
  #endif
  #ifndef FEATURE_RAT_PRIORITY_LIST
      reg_state_manual_search_mode_pref,
  #else
      &reg_state_manual_rat_pri_list_info,
  #endif 
      reg_mode_req_service_domain_get(),
          &(msg_p->found_plmn_list.plmn_list),  
        msg_p->service_search
       #ifdef FEATURE_CIOT
      ,reg_state_manual_search_ciot_rat
       #endif
    );
    }

#ifdef FEATURE_CIOT
  /* MPLMN scans are handled here */
  if((msg_p->service_search == SYS_SERVICE_SEARCH_NONE 
       || msg_p->service_search == SYS_SERVICE_SEARCH_CELL_SEARCH
#ifdef FEATURE_GPS_WWAN_PRIORITY_SEARCH      
	   || msg_p->service_search == SYS_SERVICE_SEARCH_GPS_ADVANCE_SEARCH
#endif
      )
	  && (msg_p->plmn_search_result != REG_MM_PLMN_SEARCH_PARTIAL))
  {
      
      /* The searches are run separately for each rat in the list. Once done,
      update the combined list      
      */
      reg_mode_remove_rat_from_rat_pri_list(&reg_state_rat_pri_list_ciot_manual, 
          reg_mode_convert_access_tech_to_acq_mode(reg_state_manual_search_ciot_rat));

      if (reg_state_rat_pri_list_ciot_manual.num_items != 0)
      {
#if defined(FEATURE_GTOW_BACKGROUND_PLMN_SEARCH ) || defined(FEATURE_RAT_PRIORITY_LIST)
          sys_plmn_rat_list_s_type *higher_priority_plmn_list = NULL;
#ifdef FEATURE_MODEM_HEAP
          higher_priority_plmn_list = (sys_plmn_rat_list_s_type *)modem_mem_alloc(sizeof(sys_plmn_rat_list_s_type), MODEM_MEM_CLIENT_NAS);
#else
          higher_priority_plmn_list = (sys_plmn_rat_list_s_type *)gs_alloc(sizeof(sys_plmn_rat_list_s_type));
#endif
          reg_check_for_null_ptr((void*)higher_priority_plmn_list );
          higher_priority_plmn_list->length = 0;
#endif

          /* Pick another RAT */
          reg_state_manual_search_ciot_rat = reg_mode_convert_acq_mode_to_access_tech(
              reg_state_rat_pri_list_ciot_manual.
              priority_list_info[0].acq_sys_mode);
          reg_send_mmr_plmn_search_req
              (
#ifdef FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH
              ((msg_p->network_selection_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC)&&
              (msg_p->service_search == SYS_SERVICE_SEARCH_BACKGROUND_AUTOMATIC ||
              msg_p->service_search == SYS_SERVICE_SEARCH_BACKGROUND_MANUAL))?
SYS_NETWORK_SELECTION_MODE_AUTOMATIC:SYS_NETWORK_SELECTION_MODE_MANUAL,
#else
              SYS_NETWORK_SELECTION_MODE_MANUAL, /* network_selection_mode */
#endif
              FALSE,                         /* dual_mode                   */
              reg_state_manual_search_ciot_rat
#ifdef FEATURE_GSM_BAND_PREF
              ,reg_mode_req_mode_pref_get()  /* band_pref                   */
#endif
#if defined(FEATURE_GTOW_BACKGROUND_PLMN_SEARCH ) || defined(FEATURE_RAT_PRIORITY_LIST)
              ,higher_priority_plmn_list        /* higher_priority_plmn_list_p */
#endif
#if defined(FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH) || defined(FEATURE_LTE)
              ,msg_p->service_search
#endif
#ifdef FEATURE_FEMTO_CSG
              ,SYS_CSG_SEARCH_TYPE_NONE
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
              ,&reg_state_rat_pri_list_ciot_manual            
              ,reg_mode_mcc_mode_band_filter_get()
#endif
              ,reg_mode_scan_scope_get()
              , NULL
#ifdef FEATURE_GPS_WWAN_PRIORITY_SEARCH
              , NULL
#endif
 );
#if defined(FEATURE_GTOW_BACKGROUND_PLMN_SEARCH ) || defined(FEATURE_RAT_PRIORITY_LIST)
#ifndef FEATURE_MODEM_HEAP
          gs_free(higher_priority_plmn_list);
#else
          modem_mem_free(higher_priority_plmn_list, MODEM_MEM_CLIENT_NAS);
#endif
          higher_priority_plmn_list = NULL;
#endif

#ifndef FEATURE_MODEM_HEAP
          gs_free(available_plmn_u_list);
#else
          modem_mem_free(available_plmn_u_list, MODEM_MEM_CLIENT_NAS);
#endif
          available_plmn_u_list =  NULL;

          return next_state_p;
      }       
  }
#endif /* CIOT*/
    
  #ifndef FEATURE_RAT_PRIORITY_LIST
  #ifndef FEATURE_GTOW_BACKGROUND_PLMN_SEARCH
    /*
    ** Initiate the search on the second RAT.
    */
    if (!reg_state_manual_search_complete)
    {
      reg_state_manual_search_complete = TRUE;
  
      if (reg_state_manual_search_rat == SYS_RAT_GSM_RADIO_ACCESS)
      {
        reg_state_manual_search_rat = SYS_RAT_UMTS_RADIO_ACCESS;
      }
      else
      {
        reg_state_manual_search_rat = SYS_RAT_GSM_RADIO_ACCESS;
      }
  
      reg_send_mmr_plmn_search_req
      (
  #ifdef FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH
        ((msg_p->network_selection_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC)&&
         (msg_p->service_search == SYS_SERVICE_SEARCH_BACKGROUND_AUTOMATIC ||
          msg_p->service_search == SYS_SERVICE_SEARCH_BACKGROUND_MANUAL))?
           SYS_NETWORK_SELECTION_MODE_AUTOMATIC:SYS_NETWORK_SELECTION_MODE_MANUAL,
  #else
        SYS_NETWORK_SELECTION_MODE_MANUAL, /* network_selection_mode */
  #endif
        FALSE,                             /* dual_mode              */
        reg_state_manual_search_rat        /* rat                    */
  #ifdef FEATURE_GSM_BAND_PREF
        ,reg_mode_band_pref_get()          /* band_pref              */
  #endif
  #ifdef FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH
        ,msg_p->service_search
  #endif
#ifdef FEATURE_GPS_WWAN_PRIORITY_SEARCH
        ,NULL
#endif
      );
    }
    /*
    ** Prioritize the complete PLMN list and send it to CM.
    */
    else
  #endif
  #endif /*FEATURE_RAT_PRIORITY_LIST*/
    {
       if( msg_p->service_search == SYS_SERVICE_SEARCH_BACKGROUND_MANUAL ||
           msg_p->service_search == SYS_SERVICE_SEARCH_BACKGROUND_AUTOMATIC )
       {
         list_size = sizeof(uint32) + ((reg_mode_get_available_backgnd_list())->length) * sizeof(sys_detailed_plmn_list_info_s_type);
         memscpy(available_plmn_list,list_size,(reg_mode_get_available_backgnd_list()),list_size);
       }
       else if(msg_p->service_search == SYS_SERVICE_SEARCH_CELL_SEARCH)
       {
		 memscpy(local_cell_info_list,sizeof(sys_cell_search_list_s_type),reg_mode_consolidated_cell_list_get(),sizeof(sys_cell_search_list_s_type));
       }
#ifdef FEATURE_GPS_WWAN_PRIORITY_SEARCH
       else if(msg_p->service_search == SYS_SERVICE_SEARCH_GPS_ADVANCE_SEARCH)
       {

		 ref_cnt_obj_add_ref(msg_p->found_plmn_list.gps_wwan_scan_result);

         available_plmn_u_list->gps_wwan_scan_result = msg_p->found_plmn_list.gps_wwan_scan_result;
		 memscpy(available_plmn_u_list->gps_wwan_scan_result,sizeof(sys_loc_wwan_scan_result_s_type),
		   msg_p->found_plmn_list.gps_wwan_scan_result,sizeof(sys_loc_wwan_scan_result_s_type));
		   
		 if(available_plmn_u_list->gps_wwan_scan_result->wwan_scan_result_per_rat.rat == SYS_RAT_LTE_M1_RADIO_ACCESS)
		 {
		   reg_log_umts_wwan_m1_scan_result(&(available_plmn_u_list->gps_wwan_scan_result->wwan_scan_result_per_rat), (sys_modem_as_id_e_type)reg_as_id);
		 }
		 else if(available_plmn_u_list->gps_wwan_scan_result->wwan_scan_result_per_rat.rat == SYS_RAT_LTE_NB1_RADIO_ACCESS)
		 {
		   reg_log_umts_wwan_nb1_scan_result(&(available_plmn_u_list->gps_wwan_scan_result->wwan_scan_result_per_rat), (sys_modem_as_id_e_type)reg_as_id);
		 }
		 else if(available_plmn_u_list->gps_wwan_scan_result->wwan_scan_result_per_rat.rat == SYS_RAT_GSM_RADIO_ACCESS)
		 {
		   reg_log_umts_wwan_gsm_scan_result(&(available_plmn_u_list->gps_wwan_scan_result->wwan_scan_result_per_rat), (sys_modem_as_id_e_type)reg_as_id);
		 }        
		 ref_cnt_obj_release(msg_p->found_plmn_list.gps_wwan_scan_result);
       }      
#endif
       else
       {
         list_size = sizeof(uint32) + ((reg_mode_available_plmn_list_manual_get())->length) * sizeof(sys_detailed_plmn_list_info_s_type);
         memscpy(available_plmn_list,list_size,(reg_mode_available_plmn_list_manual_get()),list_size);
       }
  
  #ifdef FEATURE_EQUIVALENT_HPLMN
  
      if(reg_sim_disp_only_highest_priority_ehplmn() && 
          msg_p->service_search != SYS_SERVICE_SEARCH_BACKGROUND_MANUAL &&
          msg_p->service_search != SYS_SERVICE_SEARCH_BACKGROUND_AUTOMATIC &&
          msg_p->service_search != SYS_SERVICE_SEARCH_CELL_SEARCH
#ifdef FEATURE_GPS_WWAN_PRIORITY_SEARCH
           && msg_p->service_search != SYS_SERVICE_SEARCH_GPS_ADVANCE_SEARCH
#endif
      )
      {
        i=0;
  
        if (i<available_plmn_list->length && reg_sim_is_ehplmn(available_plmn_list->info[i].plmn))
        {
          i++;
          j = i;
          while (j<available_plmn_list->length && reg_sim_is_ehplmn(available_plmn_list->info[j].plmn))
          {
            /* If found another EHPLMN with other RAT of the first EHPLMN, insert
            ** highest priority EHPLMN with this RAT too. Keep other parameters
            ** same as jth this EHPLMN.
            */
            if (sys_plmn_match(available_plmn_list->info[0].plmn, available_plmn_list->info[j].plmn))
            {
              i++;
            }
            j++;
          }
  
          if (j!= i)
          {
            while (j<available_plmn_list->length)
            {
              available_plmn_list->info[i++]=available_plmn_list->info[j++];
            }
            available_plmn_list->length = i;
          } 
        }
      }
  #endif
#ifdef FEATURE_SGLTE
     if(REG_SGLTE_DUAL_STACK && 
        (msg_p->service_search == SYS_SERVICE_SEARCH_NONE|| 
         msg_p->service_search == SYS_SERVICE_SEARCH_BACKGROUND_AUTOMATIC ||
         msg_p->service_search == SYS_SERVICE_SEARCH_BACKGROUND_MANUAL) &&
        (reg_as_id == reg_ps_stack_id))
     {
         reg_mode_copy_available_plmn_lists(msg_p->service_search);
     }
#endif
  
      /*
      ** Send the prioritized PLMN list to CM.
      */
      reg_send_cm_network_list_cnf
      (
        reg_state_cm_transaction_id_get(),
        reg_sim_read_rplmn(reg_mode_req_service_domain_get()),
        available_plmn_u_list,
        msg_p->plmn_search_result
      );

      if(msg_p->plmn_search_result == REG_MM_PLMN_SEARCH_PARTIAL)    
      {

	
	  
#ifndef FEATURE_MODEM_HEAP
		gs_free(available_plmn_u_list);
#else
		modem_mem_free(available_plmn_u_list, MODEM_MEM_CLIENT_NAS);
#endif
		available_plmn_u_list =  NULL;



        return REG_STATE_MANUAL_SEARCH;
      }
  #ifdef FEATURE_ENHANCED_NW_SELECTION
      /*
      ** If FCN - Acting HPLMN is delayed, i.e. PLMN rescan should be done.
      ** Ignore MMR_REG_CNF if PLMN rescan is started.
      */
      if (reg_state_delay_fcn_act_hplmn_get()== TRUE) 
      {
        reg_state_delay_fcn_act_hplmn_set(FALSE);
        if (reg_mode_restart_plmn_selection() == TRUE)
        {
          MSG_HIGH_DS_0(REG_SUB, "=REG= PLMN rescan - ACT HPLMN Changed");
          reg_state_service_status_set(SYS_SRV_STATUS_NO_SRV);
#ifndef FEATURE_MODEM_HEAP
          gs_free(available_plmn_u_list);
#else
          modem_mem_free(available_plmn_u_list, MODEM_MEM_CLIENT_NAS);
#endif
          available_plmn_u_list =  NULL;
          return REG_STATE_REGISTERING;
          reg_last_recieved_list_req_type = REG_CM_NETWORK_LIST_NONE;
        }
      }
  #endif
  
#ifdef DISABLE_9x05_OPTIMIZATIONS
  #ifdef FEATURE_GSTK
      if ((reg_nv_is_imsi_switch_enabled()) &&
         (reg_state_is_gstk_event_to_be_send() ||
           (reg_state_service_status_get() != reg_state_get_last_gstk_sent_svc())) &&
           (!sys_plmn_id_is_undefined(reg_state_service_plmn_get())) &&
           (msg_p->plmn_search_result == REG_MM_PLMN_SEARCH_SUCCESS)&&
           (((msg_p->service_search == SYS_SERVICE_SEARCH_BACKGROUND_AUTOMATIC) &&
             (available_plmn_list->length  == 0))||
             (msg_p->service_search == SYS_SERVICE_SEARCH_BACKGROUND_MANUAL)))
      {
        reg_send_mmr_gstk_service( TRUE); 
        reg_state_update_last_gstk_sent_plmn(reg_state_service_plmn_get());
        reg_state_update_last_gstk_sent_svc(reg_state_service_status_get());
        reg_state_update_last_gstk_sent_rat(reg_state_active_rat_get());
        reg_state_set_gstk_event_flag(FALSE);
      }
  #endif
  #endif
     }
#ifdef FEATURE_GPS_WWAN_PRIORITY_SEARCH
      if(reg_state_is_stack_suspended)
      {
        next_state_p = REG_STATE_INACTIVE;
      }
	  else
#endif	  	
	  {
      next_state_p = REG_STATE_IDLE;
	  }
#ifdef FEATURE_GPS_WWAN_PRIORITY_SEARCH
      reg_state_is_stack_suspended = FALSE;
#endif
      reg_last_recieved_list_req_type = REG_CM_NETWORK_LIST_NONE;  
  }
#ifndef FEATURE_MODEM_HEAP
   gs_free(available_plmn_u_list);
#else
   modem_mem_free(available_plmn_u_list, MODEM_MEM_CLIENT_NAS);
#endif
  available_plmn_u_list =  NULL;
  return next_state_p;
}



/*==============================================================================

FUNCTION NAME

  process_cm_network_list_req

==============================================================================*/
reg_state_e_type process_cm_network_list_req
(
  const cm_network_list_req_s_type* msg_p
)
{

  switch (reg_state_curr_p)
  {
    case REG_STATE_INACTIVE:
    case REG_STATE_IDLE:
      return reg_state_process_cm_network_list_req(msg_p);
      break;

    case REG_STATE_DEREGISTERING:
    case REG_STATE_OFFLINE:
      return reg_state_unexpected_cm_network_list_req(msg_p);
      break;

    case REG_STATE_REGISTERING:
    case REG_STATE_MANUAL_SEARCH :
    case REG_STATE_AUTOMATIC_SEARCH:
{
#if defined(FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH) || defined(FEATURE_LTE)
  if(msg_p->list_search_type == REG_CM_NETWORK_LIST_REQ_SRV_SEARCH_BGND)
  {
    /* Immediately respond CM_NETWORK_LIST_CNF with SEARCH FAILED */
	
	reg_send_empty_cm_network_list_cnf
	(
	 reg_state_cm_transaction_id_get(),
	 reg_sim_read_rplmn(reg_mode_req_service_domain_get()),
	 FALSE 
	 );

	
    return reg_state_curr_p;
  }
  else
#endif
  {
    return reg_state_process_cm_network_list_req(msg_p);
  }
}
  break;

  default:
    MSG_FATAL_DS(REG_SUB, "=REG= CM_NETWORK_LIST_REQ received in unknown state %d ", reg_state_curr_p,0,0);
    return reg_state_curr_p;
  break;
  
  }
  
}

/*==============================================================================

FUNCTION NAME

  process_cm_service_req

==============================================================================*/

reg_state_e_type process_cm_service_req
(
  const cm_service_req_s_type* msg_p
)
{
//  (void) msg_p;

  switch (reg_state_curr_p)
  {
    case REG_STATE_OFFLINE:
    case REG_STATE_DEREGISTERING:
		
        MSG_ERROR_DS_0(REG_SUB, "=REG= CM_SERVICE_REQ - Unexpected");
        return reg_state_curr_p;
      break;

    default:
        return reg_state_process_cm_service_req(msg_p);
    break;
  }
}

/*==============================================================================*/

reg_state_e_type process_cm_sim_available_req
(
  cm_sim_available_req_s_type* msg_p
)
{
//  (void) msg_p;

  switch (reg_state_curr_p)
  {
    case REG_STATE_OFFLINE:
    case REG_STATE_DEREGISTERING:
        MSG_ERROR_DS_0(REG_SUB, "=REG= CM_SIM_AVAILABLE_REQ - Unexpected");
        return reg_state_curr_p;
      break;

    case REG_STATE_INACTIVE:
	   reg_read_sim_available_nv();
       reg_state_process_cm_sim_available_req(msg_p);
       return REG_STATE_INACTIVE;
      break;

    case REG_STATE_IDLE:
    case REG_STATE_REGISTERING:
    case REG_STATE_MANUAL_SEARCH :
    case REG_STATE_AUTOMATIC_SEARCH:
        return reg_state_process_cm_sim_available_req(msg_p);
      break;

    default:
       MSG_FATAL_DS(REG_SUB, "=REG= CM_SIM_AVAILABLE_REQ received in unknown state %d ", reg_state_curr_p,0,0);
       return reg_state_curr_p;
       break;
  }
}


/*==============================================================================*/

reg_state_e_type process_cm_sim_not_available_req
(
  cm_sim_not_available_req_s_type* msg_p
)
{
//  (void) msg_p;

  switch (reg_state_curr_p)
  {
    case REG_STATE_OFFLINE:
    case REG_STATE_DEREGISTERING:
        MSG_ERROR_DS_0(REG_SUB, "=REG= CM_SIM_NOT_AVAILABLE_REQ - Unexpected");
        return reg_state_curr_p;
      break;

    case REG_STATE_INACTIVE:
        reg_sim_reset_card();
		reg_sim_reset_nasconfig();
        reg_send_mmr_sim_not_available_req();
        return (REG_STATE_INACTIVE);
      break;

    case REG_STATE_IDLE:
    case REG_STATE_REGISTERING:
    case REG_STATE_MANUAL_SEARCH :
    case REG_STATE_AUTOMATIC_SEARCH:
        return reg_state_process_cm_sim_not_available_req(msg_p);
      break;

    default:
       MSG_FATAL_DS(REG_SUB, "=REG= CM_SIM_NOT_AVAILABLE_REQ received in unknown state %d ", reg_state_curr_p,0,0);
       return reg_state_curr_p;
       break;
  }
}


/*==============================================================================

FUNCTION NAME

  process_cm_stop_mode_req

==============================================================================*/

reg_state_e_type process_cm_stop_mode_req
(
  const cm_stop_mode_req_s_type* msg_p
)
{
//  (void) msg_p;

  switch (reg_state_curr_p)
  {
    case REG_STATE_OFFLINE:
    case REG_STATE_DEREGISTERING:
        MSG_ERROR_DS_0(REG_SUB, "=REG= CM_STOP_MODE_REQ - Unexpected");
        return reg_state_curr_p;
      break;

    default:
        return reg_state_process_cm_stop_mode_req(msg_p);
    break;
  }
}

/*==============================================================================

FUNCTION NAME

  process_cm_ph_status_chgd_req

==============================================================================*/

reg_state_e_type process_cm_ph_status_chgd_req
(
  const cm_ph_status_chgd_req_s_type* msg_p
)
{
//  (void) msg_p;

  switch (reg_state_curr_p)
  {
    case REG_STATE_IDLE:
    case REG_STATE_REGISTERING:
    case REG_STATE_MANUAL_SEARCH :
    case REG_STATE_AUTOMATIC_SEARCH:
        MSG_ERROR_DS_0(REG_SUB, "=REG= CM_PH_STATUS_CHGD_REQ - Unexpected");
        return reg_state_curr_p;
      break;

    case REG_STATE_OFFLINE:
    case REG_STATE_DEREGISTERING:
    case REG_STATE_INACTIVE:
        return reg_state_process_cm_ph_status_chgd_req(msg_p);
    break;

    default:       
      MSG_FATAL_DS(REG_SUB, "=REG= CM_PH_STATUS_CHGD_REQ received in unknown state %d ", reg_state_curr_p,0,0);
      break;
  }
}

#ifdef DISABLE_9x05_OPTIMIZATIONS
#ifdef FEATURE_LTE
/*==============================================================================

FUNCTION NAME

  process_cm_set_drx_req

==============================================================================*/

reg_state_e_type process_cm_set_drx_req
(
     const cm_set_drx_req_s_type* msg_p
)
{
//  (void) msg_p;

  switch (reg_state_curr_p)
  {
    case REG_STATE_IDLE:
    case REG_STATE_REGISTERING:
    case REG_STATE_MANUAL_SEARCH :
    case REG_STATE_AUTOMATIC_SEARCH:
         return reg_state_process_cm_set_drx_req(msg_p);
       break;

    case REG_STATE_OFFLINE:
    case REG_STATE_DEREGISTERING:
    case REG_STATE_INACTIVE:
         MSG_ERROR_DS_0(REG_SUB, "=REG= CM_SET_DRX_REQ - Unexpected");
         return reg_state_curr_p;
       break;

    default:       
      MSG_FATAL_DS(REG_SUB, "=REG= CM_SET_DRX_REQ received in unknown state %d ", reg_state_curr_p,0,0);
      break;
  }
}

#endif
#endif
#ifdef FEATURE_TDSCDMA

/*==============================================================================

FUNCTION NAME

  process_cm_set_hplmn_irat_timer

==============================================================================*/

reg_state_e_type process_cm_set_hplmn_irat_timer
(
    const cm_set_hplmn_irat_search_timer_req_s_type *msg_p
)
{
//  (void) msg_p;

  switch (reg_state_curr_p)
  {
    case REG_STATE_IDLE:
    case REG_STATE_MANUAL_SEARCH :
       {
        uint32 timer_val = msg_p->timer_value;

        reg_state_e_type             next_state_p = reg_state_curr_p;

        boolean     irat_timer_allowed = FALSE;

        MSG_HIGH_DS_1(REG_SUB, "=REG= CM sent HPLMN IRAT timer  - %lu(milli-sec)", timer_val);   

        if (reg_nv_is_forced_irat_enabled() &&        
            (reg_state_roaming_ind_get()== SYS_ROAM_STATUS_OFF)&&
            (reg_state_service_status_get() != SYS_SRV_STATUS_NO_SRV)&&
            ((reg_state_active_rat_get() == SYS_RAT_TDS_RADIO_ACCESS) ||
             (reg_state_active_rat_get()  == SYS_RAT_GSM_RADIO_ACCESS)) &&
            reg_mode_is_usim_compliant() &&
            reg_mode_is_rat_present_in_rat_pri_list(SYS_RAT_LTE_RADIO_ACCESS, reg_mode_rat_pri_list_get(), FALSE) &&
           (reg_mode_network_selection_mode_get() != SYS_NETWORK_SELECTION_MODE_LIMITED))
        {
          irat_timer_allowed = TRUE;
        }

        /* Stop the timer */
        reg_timers_stop_hplmn_irat_search_period_timer();

        if(timer_val == 0)
        {
          /* Note that setting the timer to this value will prevent starting it */
          timer_val = REG_TIMERS_MAX_TIMER_VALUE;    
          /* Update the timer NV */
          reg_nv_hplmn_irat_search_timer_set(timer_val);
          if (irat_timer_allowed)
          {
              /* Issue an immediate search request.Note that in the case of falling 
                   ** back to T or G rat, the timer will not be restarted
                   */
            next_state_p = process_hplmn_irat_search_timer_expiration();
           }
         }
         else
         {
           /* Update the timer NV */
           reg_nv_hplmn_irat_search_timer_set(timer_val);
           if (irat_timer_allowed)
           {
                 /* Issue an immediate search request. Note that in the case of falling 
                      ** back to T or G rat, the timer will be restarted with the new value
                      */
              next_state_p = process_hplmn_irat_search_timer_expiration();
           }
         }

         return next_state_p;
       }
       break;

    case REG_STATE_OFFLINE:
    case REG_STATE_DEREGISTERING:
    case REG_STATE_INACTIVE:
    case REG_STATE_REGISTERING:
    case REG_STATE_AUTOMATIC_SEARCH:
      uint32 timer_val = msg_p->timer_value;

      MSG_HIGH_DS_1(REG_SUB, "=REG= CM sent HPLMN IRAT timer  - %lu(milli-sec)", timer_val);   
      if(timer_val == 0)
      {
        timer_val = REG_TIMERS_MAX_TIMER_VALUE;    
      }

      reg_nv_hplmn_irat_search_timer_set(timer_val);
      return reg_state_curr_p;
      break;

    default:       
      MSG_FATAL_DS(REG_SUB, "=REG= CM_SET_HPLMN_IRAT_TIMER_REQ received in unknown state %d ", reg_state_curr_p,0,0);
      break;
  }
}

#endif

/*==============================================================================

FUNCTION NAME

  process_cm_enter_psm_mode_req

==============================================================================*/

reg_state_e_type process_cm_enter_psm_mode_req
(
     const cm_enter_psm_mode_req_s_type* msg_p
)
{
//  (void) msg_p;

  switch (reg_state_curr_p)
  {
    case REG_STATE_IDLE:
    case REG_STATE_MANUAL_SEARCH :
    case REG_STATE_AUTOMATIC_SEARCH:
         return reg_state_process_cm_enter_psm_mode_req(msg_p);
       break;
    case REG_STATE_OFFLINE:
    case REG_STATE_DEREGISTERING:
    case REG_STATE_INACTIVE:
#ifdef FEATURE_FMD_SPEED_INFO
	if(reg_state_curr_p == REG_STATE_INACTIVE)
	{
	return reg_state_process_cm_enter_psm_mode_req(msg_p);
	}
	else
#endif
	{
	MSG_ERROR_DS_0(REG_SUB, "=REG= CM_ENTER_PSM_MODE_REQ - Unexpected");
	return reg_state_curr_p;
	}
    case REG_STATE_REGISTERING:
         MSG_ERROR_DS_0(REG_SUB, "=REG= CM_ENTER_PSM_MODE_REQ - Unexpected");
         return reg_state_curr_p;
       break;

    default:       
      MSG_FATAL_DS(REG_SUB, "=REG= CM_ENTER_PSM_MODE_REQ received in unknown state %d ", reg_state_curr_p,0,0);
      break;
  }
}


 /*==============================================================================
 
 FUNCTION NAME
 
   process_mmr_camped_ind
 
 ==============================================================================*/
 
 reg_state_e_type process_mmr_camped_ind
 (
     const mmr_camped_ind_s_type* msg_p
 )
 {
 //  (void) msg_p;
 
   switch (reg_state_curr_p)
   {
     case REG_STATE_IDLE:
     case REG_STATE_MANUAL_SEARCH :
     case REG_STATE_AUTOMATIC_SEARCH:
     case REG_STATE_OFFLINE:
     case REG_STATE_DEREGISTERING:
     case REG_STATE_INACTIVE:
          MSG_ERROR_DS_0(REG_SUB, "=REG= MMR_CAMPED_IND  - Unexpected");
          return reg_state_curr_p;
       break;

     case REG_STATE_REGISTERING:
 
       reg_send_cm_camped_ind
       (
#ifndef FEATURE_GSM_BAND_PREF
         msg_p->active_rat
#else
         msg_p->active_rat,
         msg_p->active_band
#endif
        ,msg_p->plmn
        ,msg_p->lac_id
       );
 
       reg_state_active_rat_set(msg_p->active_rat);

       reg_state_camped_ind_received_set(TRUE);
       reg_mode_scan_scope_set(SYS_SCAN_SCOPE_FULL_BAND);

#ifdef FEATURE_CIOT  
       reg_state_set_current_MCC(msg_p->plmn);
#endif
	   return reg_state_curr_p;
       break;

     default:
       MSG_FATAL_DS(REG_SUB, "=REG= MMR_CAMPED_IND received in unknown state %d ", reg_state_curr_p,0,0);
       break;
   }
 }

  
  
   /*==============================================================================
   
   FUNCTION NAME
   
	 process_mmr_plmn_search_cnf
   
   ==============================================================================*/
   
   reg_state_e_type process_mmr_plmn_search_cnf
   (
  const mmr_plmn_search_cnf_s_type* msg_p
)
   {
   //  (void) msg_p;
   
	 switch (reg_state_curr_p)
	 {
	   case REG_STATE_IDLE:
	   case REG_STATE_OFFLINE:
	   case REG_STATE_DEREGISTERING:
       case REG_STATE_REGISTERING:
	   case REG_STATE_INACTIVE:
			MSG_ERROR_DS_0(REG_SUB, "=REG= MMR_PLMN_SEARCH_CNF  - Unexpected");
#ifdef FEATURE_GPS_WWAN_PRIORITY_SEARCH
    if(msg_p->service_search == SYS_SERVICE_SEARCH_GPS_ADVANCE_SEARCH)
    {
       if(msg_p->found_plmn_list.gps_wwan_scan_result != NULL)
       	{
          ref_cnt_obj_release(msg_p->found_plmn_list.gps_wwan_scan_result);
       	}
   	}
#endif
			return reg_state_curr_p;
		 break;
  
	   case REG_STATE_MANUAL_SEARCH :
	   	return process_manual_mmr_plmn_search_cnf(msg_p);
			break;
	   case REG_STATE_AUTOMATIC_SEARCH:
	   	return process_auto_mmr_plmn_search_cnf(msg_p);
			break;

	   default:
		 MSG_FATAL_DS(REG_SUB, "=REG= MMR_PLMN_SEARCH_CNF received in unknown state %d ", reg_state_curr_p,0,0);
		 break;
	 }
   }

#ifdef FEATURE_GW_ON_DEMAND_HPLMN_SRCH
/*==============================================================================

FUNCTION NAME

  process_cm_hplmn_search_req

==============================================================================*/

reg_state_e_type process_cm_hplmn_search_req
(
  const cm_hplmn_search_req_s_type* msg_p
)
{
  reg_cmd_type* reg_cmd_p;

  (void) msg_p;

  switch (reg_state_curr_p)
  {
    case REG_STATE_REGISTERING:
    case REG_STATE_MANUAL_SEARCH :
    case REG_STATE_AUTOMATIC_SEARCH:
    case REG_STATE_OFFLINE:
    case REG_STATE_DEREGISTERING:
    case REG_STATE_INACTIVE:
        MSG_HIGH_DS_1(REG_SUB, "=REG= Ignore: HPLMN Search Request - REG is in Non-IDLE state %d", reg_state_curr_p->number());
        reg_send_cm_hplmn_search_cnf(FALSE);
        return reg_state_curr_p;
      break;

    case REG_STATE_IDLE:
        reg_state_idle_prev_nw_sel_mode = SYS_NETWORK_SELECTION_MODE_NONE;

        /*
          ** Ignore the HPLMN search if initiated in LIMITED mode.
          */
       if ((reg_mode_network_selection_mode_get() != SYS_NETWORK_SELECTION_MODE_AUTOMATIC) &&
           (reg_mode_network_selection_mode_get() != SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION))
       {
         MSG_HIGH_DS_0(REG_SUB, "=REG= Ignore: HPLMN Search in LIMITED/MANUAL MODE");
         reg_send_cm_hplmn_search_cnf(FALSE);
         return REG_STATE_IDLE;
       }

        /*
           ** N/W selection mode will be changed to AUTOMATIC
           ** if HPLMN search is initiated in MANUAL mode.
           ** This requirement is droped. Saved to indicate the hplmn search initiated by user.
           */
  reg_state_idle_prev_nw_sel_mode = reg_mode_network_selection_mode_get();
  
  /*
  ** Enqueue HPLMN Search timer to initiate the HPLMN search.
  ** Get a command buffer for the REG task.
  */
  reg_cmd_p = reg_get_cmd_buf();

  if(reg_cmd_p == NULL)
  {
    MSG_FATAL_DS(REG_SUB, "=REG= ERR:Unable to allocate the meomory", 0, 0, 0);
  }

  /*
  ** Construct the timer command.
  */
  reg_cmd_p->cmd.timer_expiry.IMH.message_set = MS_TIMER;
  reg_cmd_p->cmd.timer_expiry.IMH.message_id  = MID_TIMER_EXPIRY;

  reg_cmd_p->cmd.timer_expiry.IMH.message_len_lsb =
    (sizeof(timer_expiry_T) - sizeof(IMH_T)) % 256;
  reg_cmd_p->cmd.timer_expiry.IMH.message_len_msb =
    (sizeof(timer_expiry_T) - sizeof(IMH_T)) / 256;

  reg_cmd_p->cmd.timer_expiry.data     = 0;
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
  if(reg_as_id == SYS_MODEM_AS_ID_1)
  {
    reg_cmd_p->cmd.timer_expiry.timer_id = REG_HPLMN_TIMER;
  }
  else if(reg_as_id == SYS_MODEM_AS_ID_2)
  {
    reg_cmd_p->cmd.timer_expiry.timer_id = REG_HPLMN_TIMER_SIM_ID_2;
  }
#if defined FEATURE_TRIPLE_SIM || defined FEATURE_SGLTE_DUAL_SIM
  else if(reg_as_id == SYS_MODEM_AS_ID_3)
  {
    reg_cmd_p->cmd.timer_expiry.timer_id = REG_HPLMN_TIMER_SIM_ID_3; 
  }
#endif
#else
  reg_cmd_p->cmd.timer_expiry.timer_id = REG_HPLMN_TIMER;
#endif

       /*
           ** Put on REG command queue
           */
       reg_put_cmd(reg_cmd_p);
       return REG_STATE_IDLE;
     break;

    default:       
      MSG_FATAL_DS(REG_SUB, "=REG= CM_HPLMN_SEARCH_REQ received in unknown state %d ", reg_state_curr_p,0,0);
      break;
}
#endif

/*==============================================================================

FUNCTION NAME

  process_mmr_mm_information_ind

==============================================================================*/

reg_state_e_type process_mmr_mm_information_ind
(
  const mmr_mm_information_ind_s_type* msg_p
)

{
//  (void) msg_p;

  switch (reg_state_curr_p)
  {
    case REG_STATE_IDLE:
    case REG_STATE_MANUAL_SEARCH:
    case REG_STATE_AUTOMATIC_SEARCH:
    case REG_STATE_REGISTERING:
         reg_send_cm_mm_information_ind(&(msg_p->info));
         return (reg_state_curr_p);
       break;

    case REG_STATE_OFFLINE:
    case REG_STATE_DEREGISTERING:
    case REG_STATE_INACTIVE:
         MSG_ERROR_DS_0(REG_SUB, "=REG= MMR_MM_INFORMATION_IND - Unexpected");
         return reg_state_curr_p;
       break;

    default:       
      MSG_FATAL_DS(REG_SUB, "=REG= MMR_MM_INFORMATION_IND received in unknown state %d ", reg_state_curr_p,0,0);
      break;
  }
}


/*==============================================================================

FUNCTION NAME

  process_mmr_reg_cnf

==============================================================================*/

reg_state_e_type process_mmr_reg_cnf
(
  mmr_reg_cnf_s_type* msg_p
)

{
//  (void) msg_p;

  switch (reg_state_curr_p)
  {
    case REG_STATE_IDLE:
    case REG_STATE_REGISTERING:
         return reg_state_process_mmr_reg_cnf(msg_p);
       break;

    case REG_STATE_MANUAL_SEARCH:
          /*Update the EPLMN List */
         if (msg_p->update_equivalent_plmn_list)
         {
            boolean list_matched = reg_mode_save_equivalent_plmn_list
                                   (
                                     msg_p->service_state.plmn,
                                     &(msg_p->equivalent_plmn_list)
                                   );
            boolean eplmn_list_validated = sys_eplmn_list_validate(TRUE);
            if ((!list_matched) || eplmn_list_validated)
            {
              MSG_HIGH_DS_0(REG_SUB, "=REG= EPLMN list is either validated or eplmn_list changed");
              reg_send_eq_plmn_change_ind();
            }
         }
         return reg_state_curr_p;
         break;

    case REG_STATE_AUTOMATIC_SEARCH:
        if((msg_p->service_state.active_rat == reg_mode_req_rat_get()) && 
            (msg_p->service_state.service_status == SYS_SRV_STATUS_SRV)&&
            (msg_p->cause == MMR_CAUSE_NOT_SPECIFIED ))
        {
          (void) reg_state_process_mmr_reg_cnf(msg_p);
        }
        else
        {
          MSG_ERROR_DS_0(REG_SUB, "=REG= MMR_REG_CNF - Unexpected");
        }
        return reg_state_curr_p;
       break;

    case REG_STATE_OFFLINE:
    case REG_STATE_DEREGISTERING:
    case REG_STATE_INACTIVE:
         MSG_ERROR_DS_0(REG_SUB, "=REG= MMR_REG_CNF - Unexpected");
         return reg_state_curr_p;
       break;

    default:       
      MSG_FATAL_DS(REG_SUB, "=REG= MMR_REG_CNF received in unknown state %d ", reg_state_curr_p,0,0);
      break;
  }
}




/*==============================================================================

FUNCTION NAME

  process_mmr_service_ind

==============================================================================*/
reg_state_e_type process_automatic_search_mmr_service_ind
(
  mmr_service_ind_s_type* msg_p
)
{

  if ((reg_state_is_active_rat_lte(msg_p->service_state.active_rat)&& 
      (msg_p->cause == MMR_CAUSE_ABORT_PLMN_SELECTION)))
  {
     MSG_HIGH_DS_0(REG_SUB, "=REG= abort PLMN search and process service ind");
     reg_send_mmr_plmn_search_abort_req();
     (void) reg_state_process_mmr_service_ind(msg_p);
     return REG_STATE_IDLE;
  } 
  else if((reg_state_is_active_rat_lte(msg_p->service_state.active_rat)&& 
  	 (msg_p->cause == MMR_CAUSE_INITIATE_PLMN_SELECTION_ON_GW)))
  {
     return reg_state_process_mmr_service_ind(msg_p);     
  }	
  else if((msg_p->service_state.service_status == SYS_SRV_STATUS_NO_SRV) &&
          (msg_p->cause == MMR_CAUSE_NOT_SPECIFIED ))
  {
    MSG_HIGH_DS_0(REG_SUB, "=REG= abort PLMN search and Later on PLMN search CNF No service ind will be sent to CM");
    reg_send_mmr_plmn_search_abort_req();
  }
  else if((msg_p->service_state.service_status == SYS_SRV_STATUS_SRV) &&
          (msg_p->cause == MMR_CAUSE_NOT_SPECIFIED ))
  {
    (void) reg_state_process_mmr_service_ind(msg_p);
  }
  else
  {
     MSG_ERROR_DS_0(REG_SUB, "=REG= MMR_SERVICE_IND - Unexpected");
  }

   return REG_STATE_AUTOMATIC_SEARCH;
}

/*==============================================================================

FUNCTION NAME

  reg_state_manual_search_process_mmr_service_ind

==============================================================================*/

reg_state_e_type reg_state_manual_search_process_mmr_service_ind
(
  mmr_service_ind_s_type* msg_p
)
{
  /*No change of state in case of MMR_SERVICE_IND*/
  reg_state_e_type next_state_p = REG_STATE_MANUAL_SEARCH;

  if ((reg_state_is_active_rat_lte(msg_p->service_state.active_rat)&& 	 
     (msg_p->cause == MMR_CAUSE_INITIATE_PLMN_SELECTION_ON_GW)))
  {
     MSG_HIGH_DS_0(REG_SUB, "=REG= abort PLMN search and process service ind");
     reg_send_mmr_plmn_search_abort_req();
	 reg_send_empty_cm_network_list_cnf
	 (
	  reg_state_cm_transaction_id_get(),
	  reg_sim_read_rplmn(reg_mode_req_service_domain_get()),
	  REG_MM_PLMN_SEARCH_ABORTED
	 );

     return reg_state_process_mmr_service_ind(msg_p);     
  }

  /*Update the EPLMN List */
  if (msg_p->update_equivalent_plmn_list)
  {
    boolean list_matched = reg_mode_save_equivalent_plmn_list
    (
      msg_p->service_state.plmn,
      &(msg_p->equivalent_plmn_list)
    );
    boolean eplmn_list_validated = sys_eplmn_list_validate(TRUE);
    if ((!list_matched) || eplmn_list_validated)
    {
      MSG_HIGH_DS_0(REG_SUB, "=REG= EPLMN list is either validated or eplmn_list changed");
      reg_send_eq_plmn_change_ind();
    }
  }
  return next_state_p;
}


/*==============================================================================

FUNCTION NAME

  process_mmr_service_ind

==============================================================================*/

reg_state_e_type process_mmr_service_ind
(
  mmr_service_ind_s_type* msg_p
)
{
//  (void) msg_p;
  sys_plmn_id_s_type hplmn;

  switch (reg_state_curr_p)
  {
    case REG_STATE_IDLE:
        return  reg_state_process_mmr_service_ind(msg_p);
       break;

    case REG_STATE_MANUAL_SEARCH:
         return reg_state_manual_search_process_mmr_service_ind(msg_p);
         break;

    case REG_STATE_AUTOMATIC_SEARCH:
        return process_automatic_search_mmr_service_ind(msg_p);
       break;

	case REG_STATE_REGISTERING:
    case REG_STATE_OFFLINE:
    case REG_STATE_INACTIVE:
         MSG_ERROR_DS_0(REG_SUB, "=REG= MMR_SERVICE_IND - Unexpected");
         if(msg_p->update_equivalent_plmn_list == TRUE)
         {
           boolean list_matched = reg_mode_save_equivalent_plmn_list
                                  (
                                    msg_p->service_state.plmn,
                                    &(msg_p->equivalent_plmn_list)
                                  );
           boolean eplmn_list_validated = sys_eplmn_list_validate(TRUE);
           if ((!list_matched) || eplmn_list_validated)
           {
             MSG_HIGH_DS_0(REG_SUB, "=REG= EPLMN list is either validated or eplmn_list changed");
             reg_send_eq_plmn_change_ind();
           }
         }
         return reg_state_curr_p;
       break;

    case REG_STATE_DEREGISTERING:

         hplmn = reg_sim_read_hplmn();

         if ( sys_plmn_match(hplmn, msg_p->service_state.plmn) )
         {
            msg_p->service_state.roaming_ind = SYS_ROAM_STATUS_OFF;
         }
         else
         {
           msg_p->service_state.roaming_ind = SYS_ROAM_STATUS_ON;
         }
         reg_state_print_service_state(msg_p->service_state);
         reg_send_cm_service_ind(msg_p->service_state);
         return reg_state_curr_p;
         break;

    default:       
      MSG_FATAL_DS(REG_SUB, "=REG= MMR_SERVICE_IND received in unknown state %d ", reg_state_curr_p,0,0);
      break;
  }
}

/*==============================================================================

FUNCTION NAME

  process_mmr_sim_available_cnf

==============================================================================*/


reg_state_e_type process_mmr_sim_available_cnf
(
  mmr_sim_available_cnf_s_type* msg_p
)
{
//  (void) msg_p;

  switch (reg_state_curr_p)
  {
    case REG_STATE_REGISTERING:
           reg_send_cm_sim_available_cnf
           (
              reg_state_cm_transaction_id_get()
           );

           reg_mode_start_plmn_selection
           (
              reg_mode_network_selection_mode_get(),
              reg_mode_mode_pref_get(),
              reg_mode_cs_for_request_get(),
              reg_mode_ps_for_request_get(),
              reg_mode_req_service_domain_get()
#ifdef FEATURE_GSM_BAND_PREF
              ,reg_mode_band_pref_get()
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
              ,reg_mode_rat_pri_list_get()
#endif
              ,REG_CM_SERVICE_REQ_NORMAL
#ifdef FEATURE_LTE
              ,reg_mode_sms_only_get(),
              0
#endif 
           );

          return reg_state_curr_p;
       break;

      case REG_STATE_INACTIVE:
        reg_send_cm_sim_available_cnf
        (
          reg_state_cm_transaction_id_get()
        );

        if (reg_nv_psm_restore_pending == TRUE)
        {
          reg_nv_psm_restore_pending = FALSE;
          reg_nv_psm_free_cache();
        }
        return reg_state_curr_p;
        break;

      case REG_STATE_OFFLINE:
      case REG_STATE_DEREGISTERING:
      case REG_STATE_IDLE:
      case REG_STATE_MANUAL_SEARCH:
      case REG_STATE_AUTOMATIC_SEARCH:
         MSG_ERROR_DS_0(REG_SUB, "=REG= MMR_SIM_AVAILABLE_CNF - Unexpected");
         return reg_state_curr_p;
       break;

    default:       
      MSG_FATAL_DS(REG_SUB, "=REG= MMR_SIM_AVAILABLE_CNF received in unknown state %d ", reg_state_curr_p,0,0);
      break;
  }
}


/*==============================================================================

FUNCTION NAME

  process_mmr_sim_not_available_cnf

==============================================================================*/

reg_state_e_type process_mmr_sim_not_available_cnf
(
  mmr_sim_not_available_cnf_s_type* msg_p
)
{
//  (void) msg_p;

  switch (reg_state_curr_p)
  {
    case REG_STATE_IDLE:
    case REG_STATE_MANUAL_SEARCH:
    case REG_STATE_AUTOMATIC_SEARCH:
    case REG_STATE_REGISTERING:
    case REG_STATE_OFFLINE:
      MSG_ERROR_DS_0(REG_SUB, "=REG= MMR_SIM_NOT_AVAILABLE_CNF - Unexpected");
		  return reg_state_curr_p;
		break;

    case REG_STATE_DEREGISTERING:
       reg_send_cm_sim_not_available_cnf
       (
         reg_state_cm_transaction_id_get()
       );
       return REG_STATE_IDLE;
       break;


    case REG_STATE_INACTIVE:
      reg_send_cm_sim_not_available_cnf
      (
        reg_state_cm_transaction_id_get()
      );
      return (REG_STATE_INACTIVE);
      break;

    default:       
      MSG_FATAL_DS(REG_SUB, "=REG= MMR_SIM_NOT_AVAILABLE_CNF received in unknown state %d ", reg_state_curr_p,0,0);
      break;
  }
}

/*==============================================================================

FUNCTION NAME

  process_mmr_stop_mode_cnf

==============================================================================*/

reg_state_e_type process_mmr_stop_mode_cnf
(
  mmr_stop_mode_cnf_s_type* msg_p
)
{
//  (void) msg_p;

  switch (reg_state_curr_p)
  {

    case REG_STATE_MANUAL_SEARCH:
    case REG_STATE_AUTOMATIC_SEARCH:
    case REG_STATE_REGISTERING:
      MSG_ERROR_DS_0(REG_SUB, "=REG= MMR_STOP_MODE_CNF - Unexpected");
      return reg_state_curr_p;
      break;

    case REG_STATE_IDLE:
    case REG_STATE_DEREGISTERING:
    case REG_STATE_INACTIVE:
    case REG_STATE_OFFLINE:
       reg_send_cm_stop_mode_cnf
       (
         reg_state_cm_transaction_id_get()
       );
       return (REG_STATE_INACTIVE);
       break;

    default:       
      MSG_FATAL_DS(REG_SUB, "=REG= MMR_STOP_MODE_CNF received in unknown state %d ", reg_state_curr_p,0,0);
      break;
  }
}

/*==============================================================================

FUNCTION NAME

  process_mmr_ph_status_change_cnf

==============================================================================*/

reg_state_e_type process_mmr_ph_status_change_cnf
(
  mmr_ph_status_change_cnf_s_type* msg_p
)
{
//  (void) msg_p;

  switch (reg_state_curr_p)
  {
    case REG_STATE_MANUAL_SEARCH:
    case REG_STATE_AUTOMATIC_SEARCH:
    case REG_STATE_REGISTERING:
    case REG_STATE_IDLE:
    case REG_STATE_DEREGISTERING:
      MSG_ERROR_DS_0(REG_SUB, "=REG= MMR_PH_STATUS_CHANGE_CNF - Unexpected");
      return reg_state_curr_p;
      break;

    case REG_STATE_INACTIVE:
    case REG_STATE_OFFLINE:
        reg_send_cm_ph_status_chgd_cnf
        (
          reg_state_cm_transaction_id_get(),
          SYS_PH_STATUS_CHGD_SUCCESS
        );
       return reg_state_curr_p;
      break;

    default:       
      MSG_FATAL_DS(REG_SUB, "=REG= MMR_PH_STATUS_CHANGE_CNF received in unknown state %d ", reg_state_curr_p,0,0);
      break;
  }
}

/*==============================================================================

FUNCTION NAME

process_mmr_reg_reject_ind

==============================================================================*/

reg_state_e_type process_mmr_reg_reject_ind
(
  mmr_reg_reject_ind_s_type* msg_p
)
{
//  (void) msg_p;

  switch (reg_state_curr_p)
  {
    case REG_STATE_OFFLINE:
    case REG_STATE_DEREGISTERING:
    case REG_STATE_INACTIVE:
        MSG_ERROR_DS_0(REG_SUB, "=REG= MMR_REG_REJECT_IND - Unexpected");
        return reg_state_curr_p;
      break;

    case REG_STATE_IDLE:
    case REG_STATE_REGISTERING:
    case REG_STATE_AUTOMATIC_SEARCH:
        return reg_state_process_mmr_reg_reject_ind(msg_p);
      break;

    case REG_STATE_MANUAL_SEARCH :
      return reg_state_handle_mmr_reg_reject_ind(msg_p);
      break;

    default:
       MSG_FATAL_DS(REG_SUB, "=REG= MMR_REG_REJECT_IND received in unknown state %d ", reg_state_curr_p,0,0);
       return reg_state_curr_p;
       break;
  }
}

/*==============================================================================

FUNCTION NAME

  process_mmr_enter_psm_mode_rsp

==============================================================================*/

reg_state_e_type process_mmr_enter_psm_mode_rsp
(
     const mmr_enter_psm_mode_rsp_s_type* msg_p
)
{
//  (void) msg_p;

  switch (reg_state_curr_p)
  {
    case REG_STATE_IDLE:
    case REG_STATE_MANUAL_SEARCH :
    case REG_STATE_AUTOMATIC_SEARCH:
         return reg_state_process_mmr_enter_psm_mode_rsp(msg_p);
       break;

    case REG_STATE_OFFLINE:
    case REG_STATE_DEREGISTERING:
    case REG_STATE_INACTIVE:
    case REG_STATE_REGISTERING:
#ifdef FEATURE_FMD_SPEED_INFO
			 if(reg_state_curr_p == REG_STATE_INACTIVE)
			 {
			 return reg_state_process_mmr_enter_psm_mode_rsp(msg_p);
			 }
			 else
#endif
			 {
			 MSG_ERROR_DS_0(REG_SUB, "=REG= CM_ENTER_PSM_MODE_REQ - Unexpected");
			 return reg_state_curr_p ;
             }
    default:       
      MSG_FATAL_DS(REG_SUB, "=REG= MMR_ENTER_PSM_MODE_RSP received in unknown state %d ", reg_state_curr_p,0,0);
      break;
  }
}

/*==============================================================================

FUNCTION NAME

  process_idle_sim_refresh_fcn_ind

==============================================================================*/
reg_state_e_type process_idle_sim_refresh_fcn_ind
(
  sim_refresh_fcn_ind_s_type* msg_p
)
{
  reg_state_e_type next_state_p = REG_STATE_IDLE;
#ifdef DISABLE_9x05_OPTIMIZATIONS
  sys_plmn_list_s_type eq_plmn_list;
  boolean              list_matched = FALSE;
  boolean eplmn_list_validated = FALSE;
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
  sys_rat_pri_list_info_s_type *rat_pri_list_info = reg_mode_rat_pri_list_get();
  reg_mode_3gpp_capability_s_type rat_priority_list;
#endif 
#ifdef FEATURE_ENHANCED_NW_SELECTION
  uint8            ens_flag;
  ens_flag = reg_sim_read_ens_flag();
#endif

#ifdef FEATURE_RAT_PRIORITY_LIST  
  reg_mode_get_3gpp_rat_capabilty(&rat_priority_list,rat_pri_list_info
#ifdef FEATURE_SGLTE
      ,TRUE
#endif                                  
     );
#endif
    switch(msg_p->file_id)
    {
#ifdef DISABLE_9x05_OPTIMIZATIONS
#ifdef FEATURE_ENHANCED_NW_SELECTION
      case MMGSDI_SIM_7F66_PROP1_ACT_HPLMN:
      case MMGSDI_USIM_7F66_PROP1_ACT_HPLMN:
        if (ens_flag & GSDI_SUPPORT_ACTING_HPLMN)
        {
#ifdef FEATURE_EQUIVALENT_HPLMN
          (void)reg_sim_read_ehplmn_list();
#endif
          reg_state_process_fcn_act_hplmn(FALSE);
          if (reg_mode_restart_plmn_selection() == TRUE)
          {
            reg_state_service_status_set(SYS_SRV_STATUS_NO_SRV);
            next_state_p = REG_STATE_REGISTERING;
          }
        }
        break;
#endif
#endif
      case MMGSDI_USIM_HPLMN:
      case MMGSDI_GSM_HPLMN:
         /*
        This may be needed to satisfy ENS test cases along with general SIM refresh implementation. 
#ifdef FEATURE_ENHANCED_NW_SELECTION
         if (ens_flag & GSDI_SUPPORT_HPLMN_TIMER)
        {
#endif
        */
        reg_state_process_fcn_hplmn();
        break;

#ifdef DISABLE_9x05_OPTIMIZATIONS
      case MMGSDI_GSM_FPLMN:
      case MMGSDI_USIM_FPLMN:
        
        MSG_HIGH_DS_0(REG_SUB, "=REG= FPLMN List Updated");

        reg_nv_sys_equivalent_plmn_list_get(&eq_plmn_list);
        list_matched = reg_mode_save_equivalent_plmn_list(sys_eplmn_list_rplmn(), &eq_plmn_list);
        eplmn_list_validated = sys_eplmn_list_validate(TRUE);

        if((!list_matched) || eplmn_list_validated)
        {
          MSG_HIGH_DS_0(REG_SUB, "=REG= EPLMN list is either validated or eplmn_list changed");
          reg_send_eq_plmn_change_ind();
        }
        break;

      case MMGSDI_GSM_HPLMNACT:
      case MMGSDI_USIM_HPLMNWACT:

        if((sys_plmn_match(reg_sim_read_hplmn(), reg_sim_read_rplmn(reg_mode_req_service_domain_get()))) &&
           (reg_state_service_status_get() == SYS_SRV_STATUS_SRV))
        {
#ifndef FEATURE_RAT_PRIORITY_LIST        
          if(((reg_state_active_rat_get() == SYS_RAT_GSM_RADIO_ACCESS) &&
              (reg_sim_read_hplmn_rat_search_order(reg_mode_req_mode_pref_get()) == 
               SYS_MODE_PREF_UMTS_RADIO_ACCESS_PREFERRED)) ||
             ((reg_state_active_rat_get() == SYS_RAT_UMTS_RADIO_ACCESS) &&
              (reg_sim_read_hplmn_rat_search_order(reg_mode_req_mode_pref_get()) == 
               SYS_MODE_PREF_GSM_RADIO_ACCESS_PREFERRED)))
#else
#ifdef FEATURE_NAS_GW
          reg_sim_read_hplmn_rat_search_order(&rat_priority_list);
#endif 
          if(rat_priority_list.rat_list[0] != reg_state_active_rat_get())
#endif /*FEATURE_RAT_PRIORITY_LIST*/
          {
            reg_mode_start_automatic_plmn_selection
            (
              SYS_NETWORK_SELECTION_MODE_AUTOMATIC,
              REG_MODE_AUTOMATIC_MODE_NORMAL,
              reg_mode_req_mode_pref_get(),
              reg_mode_cs_for_request_get(),
              reg_mode_ps_for_request_get(),
              reg_mode_req_service_domain_get()
#ifdef FEATURE_GSM_BAND_PREF
              ,reg_mode_band_pref_get()
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
              ,reg_mode_rat_pri_list_get()
#endif
#ifdef FEATURE_LTE
              ,reg_mode_sms_only_get(),
               0
#endif 
            );
            next_state_p = REG_STATE_REGISTERING;
          }
        }
        break;

      case MMGSDI_USIM_RPLMNACT:

        if(reg_state_service_status_get() == SYS_SRV_STATUS_SRV)
        {
#ifndef FEATURE_RAT_PRIORITY_LIST        
          if(((reg_state_active_rat_get() == SYS_RAT_GSM_RADIO_ACCESS) &&
              (reg_sim_read_rplmn_rat_search_order(reg_mode_req_mode_pref_get()) == 
               SYS_MODE_PREF_UMTS_RADIO_ACCESS_PREFERRED)) ||
             ((reg_state_active_rat_get() == SYS_RAT_UMTS_RADIO_ACCESS) &&
              (reg_sim_read_rplmn_rat_search_order(reg_mode_req_mode_pref_get()) == 
               SYS_MODE_PREF_GSM_RADIO_ACCESS_PREFERRED)))
#else
#ifdef FEATURE_NAS_GW
          reg_sim_read_rplmn_rat_search_order(&rat_priority_list);
#endif
          if(rat_priority_list.rat_list[0] != reg_state_active_rat_get())
#endif /*FEATURE_RAT_PRIORITY_LIST*/
          {
            reg_mode_start_plmn_selection
            (
              reg_mode_network_selection_mode_get(),
              reg_mode_req_mode_pref_get(),
              reg_mode_cs_for_request_get(),
              reg_mode_ps_for_request_get(),
              reg_mode_req_service_domain_get()
#ifdef FEATURE_GSM_BAND_PREF
              ,reg_mode_band_pref_get()
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
              ,reg_mode_rat_pri_list_get()
#endif
              ,REG_CM_SERVICE_REQ_NORMAL
#ifdef FEATURE_LTE
              ,reg_mode_sms_only_get(),
               0
#endif 
            );
            next_state_p = REG_STATE_REGISTERING;
          }
        }
        break;
#endif

        case MMGSDI_USIM_LOCI:
        case MMGSDI_USIM_PSLOCI:
#ifdef FEATURE_GSTK
          reg_state_set_gstk_event_flag (TRUE);
          MSG_HIGH_DS_0(REG_SUB, "=REG= Setting reg need to send gstk event flag, due to RPLMN Refresh");
#endif
          break;
      default:
        MSG_HIGH_DS_1(REG_SUB, "=REG= Wrong FCN Received file - %d", msg_p->file_id);
        break;
    }

  return next_state_p;
}

 /*==============================================================================

FUNCTION NAME

  process_sim_refresh_fcn_ind

==============================================================================*/
reg_state_e_type process_sim_refresh_fcn_ind
(
  sim_refresh_fcn_ind_s_type* msg_p
)
{
//  (void) msg_p;

  switch (reg_state_curr_p)
  {
    case REG_STATE_MANUAL_SEARCH :
    case REG_STATE_AUTOMATIC_SEARCH:
    case REG_STATE_REGISTERING:
         return reg_state_process_sim_refresh_fcn_ind(msg_p);
       break;

    case REG_STATE_IDLE:
         return process_idle_sim_refresh_fcn_ind(msg_p);
        break;

    case REG_STATE_OFFLINE:
    case REG_STATE_DEREGISTERING:
    case REG_STATE_INACTIVE:
         MSG_ERROR_DS_0(REG_SUB, "=REG= SIM_REFRESH_FCN - Unexpected");
         return reg_state_curr_p;
       break;

    default:       
      MSG_FATAL_DS(REG_SUB, "=REG= SIM_REFRESH_FCN received in unknown state %d ", reg_state_curr_p,0,0);
      break;
  }
}

/*==============================================================================

FUNCTION NAME

  reg_state_process_hplmn_search_timer_expiration

==============================================================================*/

reg_state_e_type reg_state_process_hplmn_search_timer_expiration
(
     void
)
{
//  (void) msg_p;

  switch (reg_state_curr_p)
  {
    case REG_STATE_REGISTERING:
    case REG_STATE_MANUAL_SEARCH :
    case REG_STATE_INACTIVE:
          /* Timer running in Inactive state means Deep Sleep Alt Scan. Restart */
          switch(reg_mode_network_selection_mode_get())
          {
            case SYS_NETWORK_SELECTION_MODE_AUTOMATIC:
            case SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION:
            reg_timers_start_hplmn_search_timer_10sec();
              break;

            default:
             break;
          }
          return reg_state_curr_p;
       break;

    case REG_STATE_IDLE:
         return process_hplmn_search_timer_expiration();

    case REG_STATE_DEREGISTERING:
    case REG_STATE_OFFLINE:
    case REG_STATE_AUTOMATIC_SEARCH:
         MSG_ERROR_DS_0(REG_SUB, "=REG= HPLMN Search Timer - Unexpected");
         return reg_state_curr_p;
       break;

    default:       
      MSG_FATAL_DS(REG_SUB, "=REG= HPLMN Search Timer expiry received in unknown state %d ", reg_state_curr_p,0,0);
      break;
  }
}


reg_state_e_type reg_state_process_backoff_fplmn_timer_expiration
(
     void
)
{
//  (void) msg_p;

  switch (reg_state_curr_p)
  {
    case REG_STATE_REGISTERING:
    case REG_STATE_MANUAL_SEARCH :
    case REG_STATE_AUTOMATIC_SEARCH:
    case REG_STATE_INACTIVE:
         reg_sim_handle_backoff_fplmn_search_timer_expiry();
         return reg_state_curr_p;
       break;

    case REG_STATE_IDLE:
         return process_backoff_fplmn_timer_expiration();
       break;

    case REG_STATE_DEREGISTERING:
    case REG_STATE_OFFLINE:
         MSG_ERROR_DS_0(REG_SUB, "=REG= Backoff plmn Search Timer - Unexpected");
         return reg_state_curr_p;
       break;

    default:
      MSG_FATAL_DS(REG_SUB, "=REG= Backoff plmn Search Timer expiry received in unknown state %d ", reg_state_curr_p,0,0);
      break;
  }
}


reg_state_e_type process_update_lte_cap_timer_expiration
(
     void
)
{
//  (void) msg_p;

  switch (reg_state_curr_p)
  {
    case REG_STATE_IDLE:
         return reg_state_process_update_lte_cap_timer_expiration();
       break;
    
    case REG_STATE_REGISTERING:
    case REG_STATE_MANUAL_SEARCH :
    case REG_STATE_AUTOMATIC_SEARCH:
    case REG_STATE_INACTIVE:
    case REG_STATE_DEREGISTERING:
    case REG_STATE_OFFLINE:
         MSG_HIGH_DS_0(REG_SUB, "=REG= UPDATE LTE CAP TIMER expired - Ignored/Unexpected");
         return reg_state_curr_p;
       break;

    default:
      MSG_FATAL_DS(REG_SUB, "=REG= UPDATE LTE CAP TIMER expiry received in unknown state %d ", reg_state_curr_p,0,0);
      break;
  }
}

reg_state_e_type reg_state_process_t3245_timer_expiration
(
     void
)
{
//  (void) msg_p;

  switch (reg_state_curr_p)
  {
    case REG_STATE_MANUAL_SEARCH :
    case REG_STATE_AUTOMATIC_SEARCH:
    case REG_STATE_IDLE:
          return reg_state_process_t3245_expiry();
       break;

    case REG_STATE_INACTIVE:
    case REG_STATE_DEREGISTERING:
    case REG_STATE_OFFLINE:
    case REG_STATE_REGISTERING:
         return reg_state_t3245_exp_clean_up();
       break;

    default:
      MSG_FATAL_DS(REG_SUB, "=REG= T3245 Timer expiry received in unknown state %d ", reg_state_curr_p,0,0);
      break;
  }
}

reg_state_e_type process_mmr_reg_req_ignored_indication
(
     void
)
{

  switch (reg_state_curr_p)
  {
    case REG_STATE_REGISTERING:
          return reg_state_process_mmr_reg_req_ignored_ind();
       break;
    case REG_STATE_MANUAL_SEARCH :
    case REG_STATE_AUTOMATIC_SEARCH:
    case REG_STATE_IDLE:
    case REG_STATE_INACTIVE:
    case REG_STATE_DEREGISTERING:
    case REG_STATE_OFFLINE:
         MSG_HIGH_DS_0(REG_SUB, "=REG= MMR_REG_REQ_IGNORED_IND - Unexpected");
         return reg_state_curr_p;
       break;

    default:
      MSG_FATAL_DS(REG_SUB, "=REG= MMR_REG_REQ_IGNORED_IND received in unknown state %d ", reg_state_curr_p,0,0);
      break;
  }
}

reg_state_e_type process_update_irat_ciot_timer_expiration
(
     void
)
{
//  (void) msg_p;

  switch (reg_state_curr_p)
  {
    case REG_STATE_IDLE:
         return process_irat_ciot_search_timer_expiry();
       break;
    
    case REG_STATE_AUTOMATIC_SEARCH:
        return REG_STATE_IDLE;
       break;
    case REG_STATE_REGISTERING:
	case REG_STATE_INACTIVE:
	case REG_STATE_MANUAL_SEARCH :
          /* Timer running in Inactive state means Deep Sleep Alt Scan. Restart */
         switch(reg_mode_network_selection_mode_get())
         {
            case SYS_NETWORK_SELECTION_MODE_AUTOMATIC:
            case SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION:
           	  reg_timers_start_ciot_irat_search_period_timer_10s();
              break;

            default:
             break;
         }
         return reg_state_curr_p;
       break;

    case REG_STATE_DEREGISTERING:
    case REG_STATE_OFFLINE:
         MSG_ERROR_DS_0(REG_SUB, "=REG= CIOT IRAT Search Timer expiration - Unexpected");
         return reg_state_curr_p;
       break;

    default:
      MSG_FATAL_DS(REG_SUB, "=REG= CIOT IRAT Search Timer expiry received in unknown state %d ", reg_state_curr_p,0,0);
      break;
  }
}


#ifdef FEATURE_FEMTO_CSG
/*==============================================================================

FUNCTION NAME

  process_mmr_csg_list_read_complete
==============================================================================*/
reg_state_e_type process_mmr_csg_list_read_complete
(
 void
)
{
  reg_state_e_type next_state_p = REG_STATE_AUTOMATIC_SEARCH;
  sys_csg_info_s_type  csg_info = reg_state_csg_info_get();
  reg_sim_set_csg_list_read_complete(TRUE);

  if((reg_state_power_on_csg_search == TRUE) && 
    (reg_state_service_status_get()== SYS_SRV_STATUS_SRV) )
  {
    if((csg_info.csg_id == SYS_CSG_ID_INVALID) || 
       (csg_info.hybrid_cell == TRUE))
    {
      reg_send_cm_csg_search_trigger_ind();
    }
    reg_state_power_on_csg_search = FALSE; 
  }
  return next_state_p;
}
#endif 

/*==============================================================================

FUNCTION NAME

  number

==============================================================================*/

static reg_state_e_type number ( void )
{
  return REG_STATE_AUTOMATIC_SEARCH;
}


/*==============================================================================

FUNCTION NAME

  reg_state_output_name

==============================================================================*/

void reg_state_output_name ( void )
{
  switch (reg_state_next_p)
  {
    case REG_STATE_MANUAL_SEARCH:
         MSG_HIGH_DS_0(REG_SUB, "=REG= REG_STATE_MANUAL_SEARCH");
         break;
    case REG_STATE_AUTOMATIC_SEARCH:
         MSG_HIGH_DS_0(REG_SUB, "=REG= REG_STATE_AUTOMATIC_SEARCH");
         break;
    case REG_STATE_REGISTERING:
         MSG_HIGH_DS_0(REG_SUB, "=REG= REG_STATE_REGISTERING");
         break;
    case REG_STATE_IDLE:
         MSG_HIGH_DS_0(REG_SUB, "=REG= REG_STATE_IDLE");
         break;
    case REG_STATE_DEREGISTERING:
         MSG_HIGH_DS_0(REG_SUB, "=REG= REG_STATE_DEREGISTERING");
         break;
    case REG_STATE_INACTIVE:
         MSG_HIGH_DS_0(REG_SUB, "=REG= REG_STATE_INACTIVE");
         break;
    case REG_STATE_OFFLINE:
         MSG_HIGH_DS_0(REG_SUB, "=REG= REG_STATE_OFFLINE");
         break;
    default:       
      MSG_FATAL_DS(REG_SUB, "=REG= Unknown state %d ", reg_state_next_p,0,0);
      break;
  }
}

#ifdef FEATURE_TDSCDMA
/*==============================================================================

FUNCTION NAME

  process_cm_set_hplmn_irat_timer

==============================================================================*/
reg_state_e_type process_cm_set_hplmn_irat_timer
(
  const cm_set_hplmn_irat_search_timer_req_s_type *msg_p
)
{
  
  uint32 timer_val = msg_p->timer_value;

  MSG_HIGH_DS_1(REG_SUB, "=REG= CM sent HPLMN IRAT timer  - %lu(milli-sec)", timer_val);   

  if(timer_val == 0)
  {
    timer_val = REG_TIMERS_MAX_TIMER_VALUE;    
  }

  reg_nv_hplmn_irat_search_timer_set(timer_val);
  return (REG_STATE_AUTOMATIC_SEARCH);
}
#endif
#if defined FEATURE_TDSCDMA || defined FEATURE_SGLTE
/*==============================================================================

FUNCTION NAME

  process_hplmn_irat_search_timer_expiration

==============================================================================*/
static reg_state_e_type process_hplmn_irat_search_timer_expiration
(
  void
)
{
  reg_state_e_type next_state_p = REG_STATE_AUTOMATIC_SEARCH;
  if(reg_state_get_irat_lte_timer_search_ongoing() == TRUE)
  {
    /*starting 10 second timer */
    reg_timers_start_hplmn_irat_search_period_timer_10sec();
  }
  return next_state_p;
}


/*==============================================================================

FUNCTION NAME

  process_hplmn_irat_search_timer_expiration

==============================================================================*/
static reg_state_e_type process_lte_irat_search_timer_expiration
(
  void
)
{
  reg_state_e_type next_state_p = REG_STATE_AUTOMATIC_SEARCH;

  if(reg_state_get_irat_hplmn_timer_search_ongoing() == TRUE)
  {
    /*starting 10 second timer */
    reg_timers_start_lte_irat_search_period_timer_10sec();
  }
  
  return next_state_p;
}
#endif
#ifdef FEATURE_CIOT
/*==============================================================================

FUNCTION NAME

  process_irat_ciot_search_timer_expiry
==============================================================================
static reg_state_e_type process_irat_ciot_search_timer_expiry (void)
{
    reg_state_e_type next_state_p = REG_STATE_IDLE;

    return next_state_p;
}*/
#endif

/*==============================================================================

                              FUNCTION DEFINITIONS

==============================================================================*/

/*==============================================================================

FUNCTION NAME

  reg_state_automatic_search_initiated

==============================================================================*/

void reg_state_automatic_search_initiated
(
  boolean                      dual_mode,
  sys_radio_access_tech_e_type rat
)
{
#ifndef FEATURE_RAT_PRIORITY_LIST
#ifndef FEATURE_GTOW_BACKGROUND_PLMN_SEARCH
  reg_state_automatic_search_rat = rat;
  if (dual_mode)
  {
    reg_state_automatic_search_complete = FALSE;
  }
  else
  {
    reg_state_automatic_search_complete = TRUE;
  }
#endif
#endif
}


void reg_state_filter_rats (sys_rat_pri_list_info_s_type* rat_pri_list_info, sys_radio_access_tech_e_type  rat)
{
   uint8 i;
   sys_sys_mode_e_type  acq_sys_mode = SYS_SYS_MODE_NONE;
   boolean found = FALSE;
   acq_sys_mode = reg_mode_convert_access_tech_to_acq_mode(rat);

   for (i = 0; i < rat_pri_list_info->num_items; ++i )
   {
       /* Find the rat to be acquired, move its location to the top */
       if (acq_sys_mode == rat_pri_list_info->priority_list_info[i].acq_sys_mode)
       {
           found = TRUE;
           // here LTE rat to be acquired moves to the top
           memscpy(&rat_pri_list_info->priority_list_info[0], sizeof(sys_priority_list_info_s_type), 
                     &rat_pri_list_info->priority_list_info[i], sizeof(sys_priority_list_info_s_type));
       }
   }
   /*If current rat is not in net search rat list, we would have looped back earlier*/
   rat_pri_list_info->num_items = 1;
   
}

/*==============================================================================

                              FUNCTION DEFINITIONS

==============================================================================*/

/*==============================================================================

FUNCTION NAME

  reg_state_manual_search_initiated

==============================================================================*/

void reg_state_manual_search_initiated
(
#ifndef FEATURE_RAT_PRIORITY_LIST
  sys_mode_pref_e_type         mode_pref,
#else
  const sys_rat_pri_list_info_s_type  *rat_pri_list_info,
#endif
  reg_cm_network_list_req_e_type    search_type,
  sys_radio_access_tech_e_type rat
)
{
#ifndef FEATURE_RAT_PRIORITY_LIST
  reg_state_manual_search_mode_pref = mode_pref;
#ifndef FEATURE_GTOW_BACKGROUND_PLMN_SEARCH
  reg_state_manual_search_rat       = rat;

  if ( (mode_pref == SYS_MODE_PREF_GSM_RADIO_ACCESS_ONLY) ||
#ifdef FEATURE_UMTS_1X_HANDOVER_UMTSMSM 
       #error code not present
#endif
       (mode_pref == SYS_MODE_PREF_UMTS_RADIO_ACCESS_ONLY))

  {
    reg_state_manual_search_complete = TRUE;
  }
  else
  {
    reg_state_manual_search_complete = FALSE;
  }
#endif
#else  
   memscpy(&reg_state_manual_rat_pri_list_info, sizeof(sys_rat_pri_list_info_s_type), rat_pri_list_info, sizeof(sys_rat_pri_list_info_s_type));
#endif /*FEATURE_RAT_PRIORITY_LIST*/
 if(search_type != REG_CM_NETWORK_LIST_REQ_SRV_SEARCH_BGND)
 {
  reg_mode_clear_available_plmn_list_manual();
 }
 if(search_type == REG_CM_NETWORK_LIST_REQ_CELL_SEARCH)
 {
  reg_mode_clear_consolidated_cell_list();
 }

#ifdef FEATURE_CIOT
 /* Update the list with RATs */
 memscpy(&reg_state_rat_pri_list_ciot_manual, sizeof(sys_rat_pri_list_info_s_type), rat_pri_list_info, sizeof(sys_rat_pri_list_info_s_type));
 reg_state_manual_search_ciot_rat = rat;
 reg_mode_reset_manual_search_index();
#endif

/*Filter based on PCI search*/
/*By this time rat is in the net search list*/
/*Now this list contains only 1 rat*/
if(reg_state_single_cell_search_get()== TRUE)
{
  reg_state_filter_rats(&reg_state_rat_pri_list_ciot_manual,rat);
}

}

#ifdef FEATURE_CIOT
/* This function creates a detailed list out of a high priority network 
*/ 
void reg_mode_create_available_plmn_list_ciot
(
  const sys_plmn_rat_list_s_type*    higher_priority_plmn_list_p,  
  sys_detailed_plmn_list_s_type* detailed_list
)
{
   uint32 j,length;
   detailed_list->length = 0;
   ASSERT(higher_priority_plmn_list_p->length < SYS_PRIORITY_PLMN_LIST_MAX_LENGTH);

   for (j = 0; j < higher_priority_plmn_list_p->length && j < SYS_PLMN_LIST_MAX_LENGTH ; j++)
   {       
          length = detailed_list->length;     
          detailed_list->info[length].plmn                    = higher_priority_plmn_list_p->info[j].plmn;          
          detailed_list->info[length].rat                     = higher_priority_plmn_list_p->info[j].rat;
          detailed_list->info[length].plmn_service_capability = SYS_SRV_CAPABILITY_CS_PS_SERVICE;
          detailed_list->info[length].plmn_forbidden          = FALSE;
          detailed_list->info[length].list_category           = SYS_DETAILED_PLMN_LIST_CATEGORY_OTHER;
          detailed_list->info[length].signal_quality          = SYS_SIGNAL_QUALITY_HIGH;
          detailed_list->info[length].signal_strength         = 0;
          detailed_list->length++;
   }
}

/*==============================================================================

FUNCTION NAME

  reg_state_convert_edrx_pw
 
 DESCRIPTION
  This function converts the encoded duration of the Paging Time Window
  per 24.008 Table 10.5.5.32, into ms. 
 PARAMETERS
  Encoded Paging Time Window in a lower nibble
 RETURN VALUE
  Duration of the Paging Time Window
==============================================================================*/
static uint32 reg_state_convert_edrx_pw (uint8 pw)
{        
    if (reg_state_active_rat_get() == SYS_RAT_LTE_NB1_RADIO_ACCESS)
    {
        switch (pw)
        {
            case 0x0: return 2560;
            case 0x1: return 5120;
            case 0x2: return 7680;
            case 0x3: return 10240;
            case 0x4: return 12800;
            case 0x5: return 15360;
            case 0x6: return 17920;
            case 0x7: return 20480;
            case 0x8: return 23040;
            case 0x9: return 25600;
            case 0xa: return 28160;
            case 0xb: return 30720;
            case 0xc: return 33280;
            case 0xd: return 35840;
            case 0xe: return 38400;
            case 0xf: return 40960;
            default:return 0;
        }
    }
    else if (reg_state_active_rat_get() == SYS_RAT_LTE_M1_RADIO_ACCESS)
    {
        switch (pw)
        {
            case 0x0: return 1280;
            case 0x1: return 2560;
            case 0x2: return 3840;
            case 0x3: return 5120;
            case 0x4: return 6400;
            case 0x5: return 7680;
            case 0x6: return 8960;
            case 0x7: return 10240;
            case 0x8: return 11520;
            case 0x9: return 12800;
            case 0xa: return 14080;
            case 0xb: return 15360;
            case 0xc: return 16640;
            case 0xd: return 17920;
            case 0xe: return 19200;
            case 0xf: return 20480;
            default:return 0;
        }
    }
    else
    {
        return 0;
    }
}
/*==============================================================================

FUNCTION NAME

  reg_state_evaluate_edrx_alignment
 
 DESCRIPTION
  This function determines how close we are to the next eDRX Paging Cycle
  The goal is to proceed with the BPLMN selection only if we are outside
  of a predefined distance from the next paging cycle.
 RETURN VALUE
  0 if a timer does not need to be restarted
  non-zero : a new timeout value
==============================================================================*/
static uint64 reg_state_evaluate_edrx_alignment (void)
{
    reg_state_edrx_params_s_type edrx_info;
    ciot_params_s_type ciot_params;
    uint64 curr_time_ms, restart_value;
    uint64 max_limit = ~0;
    dword  max_limit_dw = ~0;

    reg_state_get_edrx_info(&edrx_info);
       
    ciot_params = reg_nv_get_ciot_params();
    curr_time_ms = time_get_ms_native();

    /* Compare the absolute timestamps */
    if (edrx_info.next_page_ms <= curr_time_ms)
    {
        /* Absoulte time of a next page occurence is behind current time,
           no action*/
        MSG_HIGH_DS_2(REG_SUB, "=REG= eDRX next page %x behind current time %x",
            edrx_info.next_page_ms, ciot_params.search_alignment);
        return 0;
    }

    /* Code safety */
    if ((ciot_params.search_alignment > max_limit_dw)/60000)
    {
        /*Overflow, use a default value*/
        ciot_params.search_alignment = 60000;
    }
    else
    {
       /* Express in ms */
       ciot_params.search_alignment *= 60000;
    }
    
    /* Calculate the time till the next page */
    restart_value = edrx_info.next_page_ms - curr_time_ms;
       
    if (restart_value < ciot_params.search_alignment)
    {
        /* We are within the alignment period. Restart the timer to start at the eDRX sleep
        */
        if (max_limit - restart_value < edrx_info.edrx.edrx_ptw)
        {
            /* Wraparound*/
            restart_value = 0;
        }
        else
        {
            /* A timer will be restarted at a new value, expressed in ms */
            restart_value += reg_state_convert_edrx_pw(edrx_info.edrx.edrx_ptw);
        }       
    }
    else
    {
        /* Current time is outside of the alignmnent period*/
         MSG_HIGH_DS_2(REG_SUB, "=REG= eDRX next page %x outside the alignment wdw %x",
            edrx_info.next_page_ms, ciot_params.search_alignment);
        return 0;
    }
    return restart_value;
}
/*==============================================================================

FUNCTION NAME

  reg_state_evaluate_psm_alignment
 
 DESCRIPTION
  This function calculates a restart value for a timer in order to meet the
  alignment requirements. Until the PSM READY or AUTO READY is sent,
  a restart value should be 10s. Otherwise, no restart (0), in which case
  PSM is blocked until a corresponding REG procedure is completed.
 
 RETURN VALUE
  0 if a timer does not need to be restarted
  non-zero : a new timeout value
==============================================================================*/
static uint64 reg_state_evaluate_psm_alignment (boolean check_reg_timer_expired)
{        
    if (reg_state_get_psm_ready() == FALSE && reg_state_get_psm_sleep_blocked() == FALSE)
    {
        /* NAS did not receive the indication that, indicate that the timer
           needs to be restarted with 10s
         */
        if(check_reg_timer_expired == FALSE)	return 10000;
    }
    /* The reg_state_psm_ready becomes true at MMR_PSM_AUTO_READY. Then on the
       following timer expiry, reset the flag
    */
    reg_state_set_psm_ready(FALSE);
    /* No restart required */
    return 0;
}
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
uint64 reg_state_calculate_restart_value (void)
{

    reg_state_edrx_params_s_type edrx_info;
    boolean check_reg_timer_expired = FALSE;    
        
    reg_state_get_edrx_info(&edrx_info);

    /* Check if the NW supports PSM, else if eDRX is enabled */
	if(mm_check_connection_active((sys_modem_as_id_e_type)reg_sub_id, 
									(sys_modem_as_id_e_type)reg_as_id) == TRUE)
	{
		reg_state_set_timer_expired(TRUE);
		return 10000;
	}								
    else if (mm_is_psm_enabled())
    {
        /* Indicate that one of the relevant timers have expired */ 
		check_reg_timer_expired = reg_state_get_timers_expired();
        reg_state_set_timer_expired(TRUE);
        return reg_state_evaluate_psm_alignment(check_reg_timer_expired);
    }
    else if (edrx_info.edrx.edrx_enabled == TRUE)
    {
        return reg_state_evaluate_edrx_alignment();
    }    
    return 0;
}
#endif


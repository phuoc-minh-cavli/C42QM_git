/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mm/vcs/mmact.c_v   1.10   03 Jul 2002 14:55:10   jault  $
$Header: //components/rel/mmcp.mpss/6.1.10/nas/mm/src/mmact.c#1 $    $DateTime: 2023/04/25 14:39:18 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/03/01   jca     Added code for RRC activation.
06/03/01   sbk     Global variables are renamed to use mm_ prefix
06/03/01   sbk     The selected plmn/lai processing is changed to use mmcoord data
06/08/01   jca     Deleted mm_power_down regional variable.
08/07/01   ts      Standardized MSG_ERROR and MSG_HIGH usage.
10/19/01   sbk     T3212 periodic timer is not started at the power-up
11/06/01   jca     Removed calls to transition substate to NO_SERVICE before
                   the SIM has been validated (i.e., keep substate NO_IMSI).
06/05/02   ks      Added support for Dual mode interface changes.
07/03/02   jca     Deleted mm_plmn_list_state.
09/23/02   mks     Added support for GSM Activation request from REG
11/14/02   mks     Update the Active RAT to be GSM on activation
01/07/03   mks     Changed the log level of an F3 message.
02/10/03   cd      Removed Lint errors
07/31/03   jca     Added input parameter to function mm_activate_state() to indicate
                   whether call to function was due to RAT change (or transition
                   from MM_NULL state).
08/29/03   cd      Added support for band_preference in rr_act_req
05/19/04   cd      Now sending the band_pref in a RR_ACT_REQ
                   if either GSM or WCDMA multiband feature is defined
09/12/11   HC      Replacing the #include "reg_mm.h" with #include "reg_mm_v.h" 
                   as a part of CMI activity
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "rex.h"
#include "sys_type.h"
#include "sys_cnst_v.h"
#include "sys_stru_v.h"
#include "timers_v.h"
#include "gs_v.h"
#include "mm_rr.h"
#include "mm_v.h"
#include "err.h"
#include "mm_umts_v.h"
#include "reg_mm_v.h"
#ifdef FEATURE_NAS_REL10
#include "gmm_int_v.h"
#endif

void mm_activate_state( boolean rat_change )
{
  if (!rat_change)
  {

#ifdef FEATURE_NAS_GW
     /* ----------------------------------------------------------------------
     ** Flag that MM has activated and so an IMSI attach may be required later
     ** ---------------------------------------------------------------------- */
     mm_possible_imsi_attach = TRUE;

     mm_clear_forbidden_LAIs();
#if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW)
     /* clear rejected csg list */
     mm_clear_rejected_csg_list();
#endif
     mm_start_timer( TIMER_MM_FORBIDDEN_LAI_CLEAR, DEFAULT_TIMEOUT );
#ifndef FEATURE_NAS_CS_DISABLED
#ifdef FEATURE_NAS_REL10
     if(mm_t3246_backoff_timer_value > 0)
     {
	    uint64 time_elapsed = 0;
        time_elapsed = time_get_ms_native() - reg_nv_timestamp_at_power_off_get();
	    if((uint64)mm_t3246_backoff_timer_value > time_elapsed)
        {
			mm_t3246_backoff_timer_value = (mm_t3246_backoff_timer_value - (dword)time_elapsed);
			mm_start_timer(TIMER_T3246,mm_t3246_backoff_timer_value);
	    }
     }
     mm_t3246_backoff_timer_value  = 0;
#endif
#endif
#endif
#if defined(FEATURE_NAS_REL10) || defined(FEATURE_LTE_REL10)
     if(gmm_t3346_backoff_timer_value > 0)
     {
       if(mm_psm_cache_ptr != NULL)
       {
         uint64 time_elapsed = 0;
         time_elapsed = time_get_ms_native() - mm_psm_cache_ptr->timers_timestamp;
         if((uint64)gmm_t3346_backoff_timer_value > time_elapsed)
         {
           mm_start_timer(TIMER_T3346, (uint64)gmm_t3346_backoff_timer_value - time_elapsed);
         }
         mm_psm_free_cache();
       }
       else
       {
		 uint64 time_elapsed = 0;
         time_elapsed = time_get_ms_native() - reg_nv_timestamp_at_power_off_get();
		 if((uint64)gmm_t3346_backoff_timer_value > time_elapsed)
         {
         	gmm_t3346_backoff_timer_value = (gmm_t3346_backoff_timer_value - (dword)time_elapsed);
			mm_start_timer(TIMER_T3346,gmm_t3346_backoff_timer_value);
		 }
       }
     }
     gmm_t3346_backoff_timer_value = 0;
#endif
#ifdef FEATURE_NAS_GW
     mm_clear_lu_attempt_counter();    
#endif
     if(mm_serving_plmn.info.sim_state == SYS_SIM_STATE_AVAILABLE && mm_is_sim_to_be_cs_invalid_after_power_off)
     {
       mm_serving_plmn.info.sim_state = SYS_SIM_STATE_CS_INVALID;
     }
     if(mm_serving_plmn.info.sim_state == SYS_SIM_STATE_AVAILABLE && mm_is_sim_to_be_ps_invalid_after_power_off)
     {
       mm_serving_plmn.info.sim_state = SYS_SIM_STATE_PS_INVALID;
     }    
  }
#ifdef FEATURE_NAS_GW
  /* Set the Default State information for various MM states */
  mm_state = MM_NULL;
  mm_idle_substate = MM_NULL_SUBSTATE;

  mm_substate_control( MM_STD_NO_IMSI );

  /* Change the MM state to MM_STD_MS_ACTIVATED */
  mm_state_control( MM_STD_MS_ACTIVATED );
  mm_state_control( MM_STD_ATTACH_LU_NOT_NEEDED);
#endif
}

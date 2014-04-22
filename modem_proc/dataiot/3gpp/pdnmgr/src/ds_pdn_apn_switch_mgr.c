/*===========================================================================
                      DS_PDN_APN_SWITCH_MGR.C
DESCRIPTION
This file defines APN switch manager APIs.

Copyright (c) 2018 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/3gpp/pdnmgr/src/ds_pdn_apn_switch_mgr.c#2 $
  $DateTime: 2023/01/19 23:06:43 $$Author: pwbldsvc $

when           who    what, where, why
--------    ---    ----------------------------------------------------------
08/09/17    sd      First checkin
===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "ds_pdn_defs.h"
#include "ds_pdn_apn_switch_mgr.h"
#include "ds_eps_pdn_manager.h"
#include "ds_pdn_nv_manager.h"


#define DS_APN_SWITCH_MGR_SERVICES_NOT_ALLOWED_CAUSE 7

/*===========================================================================

            GLOBAL VARIABLE DEFINATIONS

===========================================================================*/
ds_pdn_apn_switch_mgr_payload_info_type    ds_apn_switching_action_payload;
/*===========================================================================

            FUNCTION DEFINATIONS

===========================================================================*/

/*===========================================================================
FUNCTION       DS_PDN_APN_SWITCH_MGR_CHECK_IF_APN_SWITCH_NEEDED

DESCRIPTION    This function extracts APM rule block entry for a given 
               profile ID and compares the cause code with APN Switching
               cause codes to decide APN Switch is needed or not
 
PARAMETERS     
               profile_id: Profile of the PDN that is brought down
               cause_type: Detach/Reject Network cause
               call_info_ptr: call info passed by lower layers
               apn_switch_is_needed_p: Pointer to be filled with value that
                                       tells whether APN Switch is needed or 
                                       not
 
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds_pdn_apn_switch_mgr_check_if_apn_switch_needed
(
  void                             *call_info_ptr,
  boolean                          *apn_switch_is_needed_p,
  uint8                             profile_id,
  sys_sys_mode_e_type               call_mode,
  ds_pdn_apm_rule_param_name_enum_type cause_type
)
{
  uint16                           int_cause = 0;
  ds_bearer_deact_params_s        *deact_info_ptr = NULL;
  boolean                          switching_action_decided = FALSE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  do
  {
    if(apn_switch_is_needed_p == NULL)
    {
      DS_ERROR_LOG_0("Invalid APN switch flag passed ");
      break;
    }

    if(call_info_ptr == NULL)
    {
      DS_ERROR_LOG_0("Input Params are not valid");
      break;
    }
    /*----------------------------------------------------------------------- 
     In this function we take the following course of action:
     1. If SYS mode is LTE we check to see if APN Switch on Reject NV is set,
          in that case we perform APN Switch on reject for:
          a. ESM cause codes only
          b. EMM cause codes when cause type is not IRAT
     
        We also set the value of flag, switching_action_needed to imply no
        more code needs to be exercised to decide whether APN switch is needed
     
     2. If the above NV is not set, then we perform the cause code based checks
        which are applicable to both LTE & WCDMA/GSM
     
        For cause code based switching we switch for all ESM Cause codes for
        LTE and all NETWORK_CAUSES for UMTS
        In addition to that we perform APN switch for ESM Local Cause 7
        (EPS_SERVICES_NOT_ALLOWED_LOCAL) and SM_INTERNAL_CAUSE 22
        (GPRS_SERVICE_NOT_ALLOWED) for LTE and UMTS respectively.
     
     3. We set the local_variable "int_cause" to the value returned by lower
        layers depending on step2 and compare it will Rule block entry for the
        detached/rejected APN's profile which says whether APN Switch is needed
        for the given cause or not.
    -----------------------------------------------------------------------*/ 

    *apn_switch_is_needed_p = FALSE;
    deact_info_ptr = (ds_bearer_deact_params_s*)call_info_ptr;

    if(call_mode == SYS_SYS_MODE_LTE)
    {
      DS_MSG3(MSG_LEGACY_HIGH,
              "ESM Local is valid: %d, EMM Failure Cause: %d,"
              "ESM is valid: %d ", 
              deact_info_ptr->esm_local_cause.valid,
              deact_info_ptr->emm_failure_cause.cause_type,
              deact_info_ptr->esm_cause.valid);

      if(ds_pdn_nv_manager_get_switch_apn_on_reject())
      {
        /*----------------------------------------------------------------------- 
         For DCM we have a special requirement saying when T3402 timer expires
         NAS sends local cause code saying ATTACH_FAILED, in that case we need
         to switch APN and acquire PS services on next available one
        -----------------------------------------------------------------------*/ 
        if((ds_pdn_nv_manager_get_global_throttling_nv() == TRUE) &&
           (deact_info_ptr->esm_local_cause.valid == TRUE) &&
           (deact_info_ptr->esm_local_cause.local_cause == EMM_ATTACH_FAILED)&&
           (deact_info_ptr->emm_failure_cause.cause_type == LTE_NAS_IRAT_T3410_EXPIRY))
        {
          *apn_switch_is_needed_p = TRUE;
        }
        else
        {
          if (deact_info_ptr->esm_local_cause.valid == TRUE ||
              deact_info_ptr->emm_failure_cause.cause_type != 
                                                             LTE_NAS_IRAT_NONE)
          {
            *apn_switch_is_needed_p = FALSE;
          }
          else if (deact_info_ptr->esm_cause.valid == TRUE &&
                   deact_info_ptr->esm_cause.esm_cause == 
                                                NAS_ESM_PDN_CONN_DOES_NOT_EXIST)
          {
            *apn_switch_is_needed_p = FALSE;
          }
          else
          {
            *apn_switch_is_needed_p = TRUE;
          }
        }
        switching_action_decided = TRUE;
      }
      else
      {
        /*----------------------------------------------------------------------- 
         We perform a comparison for ESM Local cause and if its cause 7 we store
         it to compare with APM Rule block entry for the given APN
         
         The same comparison is performed for all ESM cause codes
        -----------------------------------------------------------------------*/ 
        if(deact_info_ptr->esm_local_cause.valid == TRUE)
        {
          if(deact_info_ptr->esm_local_cause.local_cause == 
                                        LTE_NAS_EPS_SERVICES_NOT_ALLOWED_LOCAL)
          {
            int_cause = deact_info_ptr->esm_local_cause.local_cause;
          }
          else
          {
            DS_MSG1(MSG_LEGACY_HIGH,
            	       "LTE Internal Cause provided by lower layer: %d "
                    "No APN Switching",
                    deact_info_ptr->esm_local_cause.local_cause);
            switching_action_decided = TRUE;
          }
        }
        else if(deact_info_ptr->esm_cause.valid == TRUE)
        {
          int_cause = deact_info_ptr->esm_cause.esm_cause;
        }
        else
        {
          switching_action_decided = TRUE;
        }
      }
    }
    else if (call_mode == SYS_SYS_MODE_GSM)
    {
      /*----------------------------------------------------------------------- 
       We perform a comparison for SM Internal Cause and if its cause 7 we store
       it to compare with APM Rule block entry for the given APN
       
       The same comparison is performed for all SM Network causes
      -----------------------------------------------------------------------*/ 
      if(deact_info_ptr->cause_type == SM_INTERNAL_CAUSE)
      {
        if(deact_info_ptr->cause.int_cause == SM_GPRS_SERVICES_NOT_ALLOWED)
        {
          int_cause = DS_APN_SWITCH_MGR_SERVICES_NOT_ALLOWED_CAUSE;
        }
        else
        {
          DS_MSG1(MSG_LEGACY_HIGH,
                  "Internal Cause provided by lower layer: %d "
                            "No APN Switching",
                  deact_info_ptr->cause.int_cause);
          switching_action_decided = TRUE;
        }
      }
      else if(deact_info_ptr->cause_type == SM_NETWORK_CAUSE)
      {
        int_cause = deact_info_ptr->cause.ext_cause;
      }
      else
      {
        switching_action_decided = TRUE;
      }
    }

    if(switching_action_decided == FALSE)
    {
      /*----------------------------------------------------------------------- 
       Once we know the cause code send by the Network, we see if an entry
       for that cause code is present in rule block for the APN, if it is
       we indicate saying APN Switch is needed.
      -----------------------------------------------------------------------*/ 
      if(ds_pdn_apm_match_cause_codes(profile_id,
                                      int_cause,
                                      cause_type))
      {
        *apn_switch_is_needed_p = TRUE;
      }
      else
      {
        *apn_switch_is_needed_p = FALSE;
       }   
    }
    
    DS_MSG1(MSG_LEGACY_HIGH,"APN Switch is needed: %d", 
    	       *apn_switch_is_needed_p);
  }while (0);  
}

/*===========================================================================
FUNCTION       DS_PDN_APN_SWITCH_MGR_RESET_SWITCHING_ACTION_PAYLOAD

DESCRIPTION    This function resets the switching action payload per subs
 
PARAMETERS     NONE
 
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds_pdn_apn_switch_mgr_reset_switching_action_payload
(
  void
)
{
  memset(&(ds_apn_switching_action_payload), 0, 
         sizeof(ds_pdn_apn_switch_mgr_payload_info_type));
}

/*===========================================================================
FUNCTION       DS_PDN_APN_SWITCH_MGR_SEND_IND_TO_APM

DESCRIPTION    This function retrieves the switching action payload 
                         and indicates it to APM 
 
PARAMETERS     None.
 
DEPENDENCIES   NONE

RETURN VALUE   ds_pdn_apm_ext_error_code: Error code after posting ind to APM

SIDE EFFECTS   NONE
===========================================================================*/
ds_pdn_apm_ext_error_code ds_pdn_apn_switch_mgr_send_ind_to_apm
(
  void
)
{
  ds_pdn_apm_ext_error_code              apm_ind_err_code = DS_PDN_APM_UNSEPCIFIED;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  if(ds_apn_switching_action_payload.payload_valid == TRUE)
  {
    apm_ind_err_code = ds_pdn_apm_ind(
                          ds_apn_switching_action_payload.apm_ind, 
                &ds_apn_switching_action_payload.apm_ind_payload);
  }

  /*-------------------------------------------------------------------
    Reset the Switching Action payload for next iteration
  --------------------------------------------------------------------*/
  ds_pdn_apn_switch_mgr_reset_switching_action_payload();

  return apm_ind_err_code;
}

/*===========================================================================
FUNCTION       DS_PDN_APN_SWITCH_MGR_POPULATE_SWITCHING_PAYLOAD_PER_RAT

DESCRIPTION    This function extracts APM rule block entry for a given 
               profile ID and compares the cause code with APN Switching
               cause codes to decide APN Switch is needed or not
 
PARAMETERS     bearer_call_mode: Call mode of the bearer 
               cause_type: DETACH/REJECT NETWORK cause type
               profile_id: Profile of the PDN detached or rejected
               PDN State: State of PDN Context
               call_info_ptr: Call info passed by lower layers
 
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds_pdn_apn_switch_mgr_populate_switching_payload_per_rat
(
  sys_sys_mode_e_type                  bearer_call_mode,
  uint16                               cause_type,
  uint8                                profile_id,
  ds_pdn_mgr_state_e                   pdn_state,
  void                                *call_info_ptr
)
{
  boolean                                     apn_switch_is_needed = FALSE;
  ds_pdn_apn_switch_mgr_payload_info_type   *local_switch_payload_ptr = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  do
  {
    if(call_info_ptr == NULL)
    {
      break;
    }
    /*-----------------------------------------------------------------
      We extract the Switching action payload info and see if its valid
     
      if its not we store the APN switching indication & payload info
      which is sent to APN when PDN is cleaned-up
     
      We decide whether to send a REJECT ind or DETACH Ind based on
      PDN state, if the PDN is coming up we consider it as PDP reject

      If the PDN is up, then we consider it as a PDP Deactivate
     
      Then we invoke the function to see if APN Switch is needed or not
      and accordingly fill the APN Switching action payload
    ------------------------------------------------------------------*/
    if(bearer_call_mode == SYS_SYS_MODE_GSM)
    {
      if(pdn_state == DS_PDN_MGR_STATE_UP)
      {
        cause_type = DS_PDN_APM_DETACH_NETWORK_CAUSE;
      }
      else if(pdn_state == DS_PDN_MGR_STATE_COMING_UP)
      {
        cause_type = DS_PDN_APM_REJECT_NETWORK_CAUSE;
      }
    }

    local_switch_payload_ptr = &ds_apn_switching_action_payload;

    if(local_switch_payload_ptr != NULL &&
       local_switch_payload_ptr->payload_valid == FALSE)
    {
      ds_pdn_apn_switch_mgr_check_if_apn_switch_needed(call_info_ptr,
                                                        &apn_switch_is_needed,
                                                        profile_id,
                                                        bearer_call_mode,
                                                        cause_type);

      memset(local_switch_payload_ptr, 0, 
             sizeof(ds_pdn_apn_switch_mgr_payload_info_type));

      if(cause_type == DS_PDN_APM_REJECT_NETWORK_CAUSE)
      {
        local_switch_payload_ptr->apm_ind = DS_PDN_APM_ATTACH_REJECT_IND;
        ds_pdn_apm_set_profile_id(&(local_switch_payload_ptr->apm_ind_payload),
                                  profile_id);
      }
      else if(cause_type == DS_PDN_APM_DETACH_NETWORK_CAUSE)
      {
        local_switch_payload_ptr->apm_ind = DS_PDN_APM_DETACH_IND;
      }
      else
      {
        DS_ERROR_LOG_1("Invalid Cause type: =d, Breaking OUT !!!!", 
                       cause_type);
        break;
      }

      ds_pdn_apm_set_switch_apn_flag(&(local_switch_payload_ptr->apm_ind_payload),
                                     apn_switch_is_needed);
      local_switch_payload_ptr->payload_valid = TRUE;
    }
  }while(0);
}


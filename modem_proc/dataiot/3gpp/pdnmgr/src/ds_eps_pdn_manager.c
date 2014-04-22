/*===========================================================================
                      DS_EPS_PDN_MANAGER.C
DESCRIPTION
This file defines LTE specific PDN manager APIs.

Copyright (c) 2018 - 2023 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/3gpp/pdnmgr/src/ds_eps_pdn_manager.c#17 $
  $DateTime: 2023/06/02 03:11:51 $$Author: pwbldsvc $

when           who    what, where, why
--------    ---    ----------------------------------------------------------
08/09/17    sd      First checkin
===========================================================================*/

#include "ds_pdn_defs.h"
#include "ds_eps_pdn_manager.h"
#include "ds_eps_attach_hdlr.h"
#include "ds_bearer_manager.h"
#include "ds_bearer_flow_manager.h"
#include "ds_pdn_manager.h"
#include "ds_pdn_flow_manager.h"
#include "ds_pdn_hdlr.h"
#include "ds_pdn_nv_manager.h"
#include "ds_pdn_call_hdlr.h"
#include "ds_pdn_cp_data_hdlr.h"
#include "ds_pdn_profile_validator.h"
#include "ds_pdn_apn_rate_ctrl.h"
#include "ds_pdn_apn_switch_mgr.h"
#include "ds_pdn_roaming_hdlr.h"
#include "ds_pdn_psm_hdlr.h"
#include "ds_3gpp_pdn_throt_spec.h"
#include "ds_3gpp_pdn_limit_hdlr.h"
#include "ds_3gpp_network_override_hdlr.h"
#include "ds_3gpp_data_retry.h"
#include "ds_3gpp_dam.h"
#include "ds_3gpp_srv_req_throt_sm.h"
#include "ds_3gpp_pdn_throt_rules.h"
#include "ds_pdn_clat_hdlr.h"
#include "ds_3gpp_cc_sp_throt_sm.h"


/*---------------------------------------------------------------------------
  Default profile
---------------------------------------------------------------------------*/
#define DS_EPS_DEF_PROF 1
#define DS_EPS_INVALID_PROF 0xFF


/*----------------------------------------------------------------------------
  LTE Attach procedure state machine
----------------------------------------------------------------------------*/
ds_eps_attach_sm_s ds_eps_pdn_attach_sm = {0};

/*----------------------------------------------------------------------------
  EMM T3402 value initialized to its default value: 12 mins(720000 ms). 
  Ref: ETSI TS 124301 V9.8.0 Sec 8.2.26.1
----------------------------------------------------------------------------*/
static uint32 ds_eps_t3402_value = 720000;

/*------------------------------------------------------------------------------
 Hold LTE attach PDN dual IP fall back IP type from NV
------------------------------------------------------------------------------*/
static ds_eps_attach_fb_info_type ds_eps_lte_attach_fb_ip_type = {0};

/*===========================================================================
FUNCTION ds_eps_pdn_get_default_socket_profile

DESCRIPTION
  This function is used to retrieve the default socket profile from pdp
  registry

PARAMETERS 
  None.
 
DEPENDENCIES
  None.

RETURN VALUE
  uint16 : default socket profile id
 
SIDE EFFECTS
  None.
===========================================================================*/
uint16 ds_eps_pdn_get_default_socket_profile 
(
  void
)
{
  ds_profile_status_etype             result = DS_PROFILE_RESULT_FAIL;
  uint8                               attach_prof = DS_EPS_DEF_PROF;  
  ds_profile_type_s                   profile_info = {{0}};
  /*-------------------------------------------------------------------------------*/

  attach_prof = ds_profile_get_profile_number_for_embeded_calls();
  
  result = ds_profile_setting_from_profile_id(attach_prof, &profile_info);
  if (DS_PROFILE_RESULT_SUCCESS == result)
  {
    if( TRUE == ds_3gpp_pdn_throt_is_apn_throttled((byte *)profile_info.context.apn,
                                                   DS_PDN_INSTANCE_MAX,
                                                   NULL, FALSE) )
    {
      DS_ERROR_LOG_1("Default socket profile: =d has APN blocked, "
                     "No APN to attach", attach_prof);
      attach_prof = DS_EPS_INVALID_PROF;
    }
  }
  else
  {
    attach_prof = DS_EPS_INVALID_PROF;
  }

  DS_MSG1(MSG_LEGACY_HIGH, "Attach profile: %d", attach_prof);
  return attach_prof;
} /* ds_eps_pdn_get_default_socket_profile */

/*===========================================================================
FUNCTION ds_eps_pdn_get_attach_prof

DESCRIPTION
  This function returns the profile number to be used to establish the
  default PDN connection during LTE attach.
 
  1. Query APM for the attach profile id and verify if this is a valid profile.
     If so use return this profile.
  2. If step 1 fails; return the profile used for socket calls.
  3. Is step 2 also fails; use the statically defined default profile DS_EPS_DEF_PROF.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  uint16 - the profile number to be used for attaching to the LTE network.

SIDE EFFECTS
  None.
===========================================================================*/
uint16 ds_eps_pdn_get_attach_prof
( 
  void
)
{
  uint8                               attach_prof = DS_EPS_DEF_PROF;
  ds_pdn_apm_req_pay_load             apm_payload = {0};
  ds_pdn_apm_ext_error_code               dsd_result = DS_PDN_APM_SUCCESS;
/*-------------------------------------------------------------------------*/
  do
  {
    //Request the attach profile from APM
    apm_payload.req_field_mask = DS_APM_REQ_PROFILE_ID_FIELD_MASK;
    dsd_result = ds_pdn_apm_req(DS_PDN_APM_ATTCH_REQ, &apm_payload);

    if (DS_PDN_APM_SUCCESS == dsd_result)
    {      
      if (ds_profile_number_is_valid(apm_payload.profile_id))
      {
        attach_prof = apm_payload.profile_id;
        break;
      }
      else
      {
        attach_prof = ds_eps_pdn_get_default_socket_profile();
        break;
      }
    }
    else 
    {
      DS_ERROR_LOG_2("Unable to get the attach profile from APM, "
                     "return_code =d, attach_apn_avail =d",
                     dsd_result,apm_payload.attach_apn_avail);
      if (dsd_result == DS_PDN_APM_ALL_ATTACH_APN_BLOCKED)
      {
        attach_prof = DS_EPS_INVALID_PROF;
      }
      else if (dsd_result == DS_PDN_APM_ATTACH_NOT_ALLOWED) 
      {
        attach_prof = DS_EPS_INVALID_PROF;
      }
      else if( (dsd_result == DS_PDN_APM_INVALID_APM) && 
               (apm_payload.attach_apn_avail == DS_PDN_APM_EFS_CONFIG_ERROR) )
      {
        /*----------------------------------------------------------------------- 
         We couldn't get a valid profile from APM because of attach APN EFS error.
         Using the default socket profile if the APN is unblocked
         -----------------------------------------------------------------------*/
        attach_prof = ds_eps_pdn_get_default_socket_profile();
      }
    }
  } while(0);

  DS_MSG1(MSG_LEGACY_HIGH, "Profile Id used for attach is %d", attach_prof);
  return attach_prof;
}

/*===========================================================================
FUNCTION DS_EPS_ATTACH_SM_SET_PREVIOUS_ATTACH_STATUS

DESCRIPTION
  Sets the previous attach information in attach SM.

PARAMETERS
  Attach status
  Down reason

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_eps_attach_sm_set_previous_attach_status
(
  boolean                  is_attach_fail,
  ds_net_down_reason_type  down_reason
)
{
  DS_MSG2(MSG_LEGACY_HIGH, "Previous atatch status. Attach failure : %d, "
  	                "Down reason : %d", is_attach_fail,
                   down_reason);
  
  ds_eps_pdn_attach_sm.prev_attach_status.is_attach_fail = is_attach_fail;
  ds_eps_pdn_attach_sm.prev_attach_status.down_reason = down_reason;
} /* ds_eps_pdn_attach_sm_set_state */

/*===========================================================================
FUNCTION ds_eps_pdn_attach_sm_set_state

DESCRIPTION
  Sets attach state for attach SM.

PARAMETERS
  Attach SM state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_eps_pdn_attach_sm_set_state
(
  ds_eps_attach_state_e   attach_state
)
{
  DS_MSG2(MSG_LEGACY_HIGH,
  	       "Attach SM. Old State: %d, New State: %d",
          ds_eps_pdn_attach_sm.attach_state, attach_state);
  ds_eps_pdn_attach_sm.attach_state = attach_state;
} /* ds_eps_pdn_attach_sm_set_state */

/*===========================================================================
FUNCTION ds_eps_pdn_attach_sm_get_state

DESCRIPTION
  Gets attach state for attach SM.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  Attach SM state.

SIDE EFFECTS
  None.
===========================================================================*/
ds_eps_attach_state_e ds_eps_pdn_attach_sm_get_state
(
  void
)
{
  return ds_eps_pdn_attach_sm.attach_state;
} /* ds_eps_pdn_attach_sm_get_state */

/*===========================================================================
FUNCTION DS_EPS_PDN_GET_T3402_TIMER_VAL

DESCRIPTION
  Returns T3402 timer value maintained within DS that is obtained via NAS.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
 T3402 timer value

SIDE EFFECTS
  None.

===========================================================================*/
uint32 ds_eps_pdn_cntxt_get_t3402_timer_val()
{
  return ds_eps_t3402_value;
}

/*===========================================================================
FUNCTION DS_EPS_PDN_CNTXT_SET_T3402_TIMER_VAL

DESCRIPTION
   Updates T3402 timer value maintained within DS that is obtained via NAS.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_eps_pdn_set_t3402_timer_val
(
  emm_ds_t3402_changed_ind_s_type  *t3402_ind
)
{
  ds_pdn_apm_ind_pay_load       ind_payload = {0};
/* - - - - - - - - - - - - - -  - - - - - - - - - - - - - - - - - - - - - - - */

  if(t3402_ind == NULL)
  {
    return;
  }

  DS_MSG1(MSG_LEGACY_HIGH, "Updating T3402 to %d", t3402_ind->t3402_value);
  ds_eps_t3402_value = t3402_ind->t3402_value;

  ind_payload.t3402_timer_val = ds_eps_t3402_value;
  ds_pdn_apm_ind(DS_PDN_APM_T3402_CHG_IND, &ind_payload);
}

/*===========================================================================
FUNCTION DS_EPS_ATTACH_SM_GET_SEQ_NUM

DESCRIPTION
  Gets the sequence number that needs to be sent as part of PDN
  connectivity.

PARAMETERS
  None.

DEPENDENCIES
  For additional PDN, send seq_num as 0 always.

RETURN VALUE
  uint8 - seq num

SIDE EFFECTS
  None.
===========================================================================*/
uint8 ds_eps_attach_sm_get_seq_num
(
  void
)
{
  if(ds_eps_pdn_attach_sm.attach_state != DS_EPS_ATTACHED)
  {
    return ds_eps_pdn_attach_sm.seq_num;
  }

  return 0;
} /* ds_eps_pdn_attach_sm_get_state */

/*===========================================================================
FUNCTION DS_EPS_ATTACH_SM_GET_ATTACH_WO_PDN_SUPPORT

DESCRIPTION
  Gets the attach wo PDN support.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  boolean - TRUE if supported, FALSE otherwise

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_eps_attach_sm_get_attach_wo_pdn_support
(
  void
)
{
  return ds_eps_pdn_attach_sm.attach_wo_pdn_supported;
} /* ds_eps_pdn_attach_sm_get_state */

/*===========================================================================
FUNCTION DS_EPS_ATTACH_SM_GET_RESPONDING_TO_PAGE

DESCRIPTION
  Gets the responding to page flag.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  boolean - TRUE if supported, FALSE otherwise

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_eps_attach_sm_get_responding_to_page
(
  void
)
{
  return ds_eps_pdn_attach_sm.responding_to_page;
} /* ds_eps_attach_sm_get_responding_to_page */


/*===========================================================================
FUNCTION DS_EPS_ATTACH_SM_RESET_PARAMS

DESCRIPTION
  This function is used to reset the attch state machine parameters

PARAMETERS 
  None
 
DEPENDENCIES
  None.

RETURN VALUE
  None
 
SIDE EFFECTS
  None.
===========================================================================*/
void ds_eps_attach_sm_reset_params
(
  void
)
{
  ds_pdn_apm_ext_error_code     apm_error_code = DS_PDN_APM_UNSEPCIFIED;
  ds_pdn_apm_ind_pay_load   apm_pay_load;
  /*-----------------------------------------------------------------------*/

  if (ds_eps_pdn_attach_sm_get_state() != DS_EPS_ATTACH_STATE_NULL)
  {
    apm_pay_load.ind_field_mask = 0;
    apm_error_code = ds_pdn_apm_ind(DS_PDN_APM_DETACH_IND, &apm_pay_load);

    // send NULL bearer to PS sys if required
  }

  ds_eps_pdn_attach_sm_set_state(DS_EPS_ATTACH_STATE_NULL);
  memset(&ds_eps_pdn_attach_sm, 0, sizeof(ds_eps_attach_sm_s));

  ds_pdn_hdlr_set_attach_complete_flag(FALSE);

  ds_3gpp_pdn_throt_advertise_blocked_apns_current_plmn();
  ds_3gpp_pdn_throt_sm_advertise_blocked_apns_to_mppm();

  return;
}/* ds_eps_attach_sm_reset_params */

/*===========================================================================
FUNCTION DS_EPS_ATTACH_SM_IS_PDN_CONN_REQ_PENDING

DESCRIPTION
  This function is used to check if a PDN CONN request to be sent to NAS
  is pending 

PARAMETERS 
  None
 
DEPENDENCIES
  None.

RETURN VALUE
  True  - If cached seq_num is > 0
  False - If cached seq number is 0
 
SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_eps_attach_sm_is_pdn_conn_req_pending
(
  void
)
{
  if(ds_eps_pdn_attach_sm.cached_seq_num > 0)
  {
    DS_MSG1(MSG_LEGACY_HIGH,
    	       "PDN_CONN_REQ is pending, cached_seq_num:%d ",
    	       ds_eps_pdn_attach_sm.cached_seq_num);
    return TRUE;
  }

  return FALSE;
}

/*===========================================================================
FUNCTION DS_EPS_PDN_MOVE_BEARER_FROM_PEND_REDIAL_TO_DOWN_IF_NEEDED

DESCRIPTION
   Make a few validations and make a decision, if the bearer and PDN
   should continue to remain in PENDING_REDIAL mode or move it to GOING_DOWN
   Basically if the validation fails, a redial on the same PDN is not
   possible and hence move the state to GOING DOWN.
 
PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_eps_pdn_move_bearer_from_pending_redial_to_down_if_needed
(
   ds_bearer_mgr_s *bearer_mgr_p
)
{
  ds_pdn_mgr_s           *pdn_mgr_p = NULL;
  ds_profile_type_s       profile_ptr = {{0}};
  ds_profile_pdp_type_e   old_pdp_type = DS_PROFILE_PDP_IPV4; 
  ds_profile_pdp_type_e   new_pdp_type = DS_PROFILE_PDP_IPV4;
  char                   *old_apn = NULL;
  char                   *new_apn = NULL;
  boolean                 has_apn_changed = FALSE;
  boolean                 set_bearer_going_down = FALSE;
  ds_pdn_mgr_inst_type_s *v4_inst = NULL;
  ds_pdn_mgr_inst_type_s *v6_inst = NULL;
  ds_pdn_v6_state_type    v6_sm_state = DS_PDN_V6_MAX_STATE;
  uint8                   pdn_index = 0;
  uint8                   pdn_limit_index = 0;
  ds_pdn_mgr_s           *temp_pdn_mgr_p = NULL;
  ds_apn_ip_support_type_e         ip_support = DS_IP_TYPE_INVALID;
  byte                            *resolved_apn  = NULL;
  boolean                          reset_gateway_iid = TRUE;
  uint8                            override_ip_bmask = 
                                    DS_3GPP_PDP_UNDEFINED_BMASK;
  sys_plmn_id_s_type               current_plmn = {0};
  ds_local_err_val_e_type  local_err = DS_LOCAL_ERR_VAL_NONE;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (!ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p))
  {
    return;
  }

  pdn_mgr_p = bearer_mgr_p->pdn_mgr_ptr;
  if(!ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p))
  {
    return;
  }

  old_pdp_type = pdn_mgr_p->pdp_profile->context.pdp_type;
  old_apn = (char *)pdn_mgr_p->pdp_profile->context.apn;

  /*------------------------------------------------------------------------ 
   Get the profile data corresponding 
   ------------------------------------------------------------------------*/
  if( ds_profile_setting_from_profile_id(
               pdn_mgr_p->pdp_profile_num, 
               &profile_ptr) != DS_PROFILE_RESULT_SUCCESS )
  {
    local_err = DS_LOCAL_ERR_VAL_0;
    goto exit_func;
  }

  new_pdp_type = profile_ptr.context.pdp_type;
  new_apn =  (char *)profile_ptr.context.apn;

  /*--------------------------------------------------------------------- 
    Check if profile's apn and pdn context's apn have changed. If they
    have changed, set a boolean to TRUE, This is needed for apn override
    checks.
    ---------------------------------------------------------------------*/
  if((strncasecmp((char*)old_apn,(char*)new_apn,
                   DS_PROFILE_APN_STRING_LEN+1)!= 0))
  {
    DS_MSG0(MSG_LEGACY_HIGH, "APN has changed");
    has_apn_changed = TRUE;
  }
  
  /*---------------------------------------------------------------------- 
    Old pdp type - represents the previous IP type of the PDN connection
    which was torn down
   
    PDP profile's IP type - New changed IP type.
   
   Old IP type                 ----- Changed IP type  -- PDP disconnect
    IPV4                             IPV6/IPV4V6                 NA
    IPv6                             IPV4/IPV4V6                 NA
    ----------------------------------------------------------------------*/
  DS_MSG2(MSG_LEGACY_HIGH, "PDN redial: Old PDP type:%d, New PDP type:%d",
                           old_pdp_type, new_pdp_type);
  switch(old_pdp_type)
  {
    case DS_PROFILE_PDP_IPV4:
      if(new_pdp_type != DS_PROFILE_PDP_IPV4)
      {
        set_bearer_going_down = TRUE;
        pdn_mgr_p->net_down_reason = DS_NET_DOWN_REASON_APN_PRM_CHG;
        local_err = DS_LOCAL_ERR_VAL_1;
        goto exit_func;
      }
      reset_gateway_iid = FALSE;
      break;

    case DS_PROFILE_PDP_IPV6:
      if(new_pdp_type != DS_PROFILE_PDP_IPV6)
      {
        set_bearer_going_down = TRUE;
        pdn_mgr_p->net_down_reason = DS_NET_DOWN_REASON_APN_PRM_CHG;
        local_err = DS_LOCAL_ERR_VAL_2;
        goto exit_func;
      }
      break;

   /*------------------------------------------------------------------------
    This piece of the code is to handle these situations
    Prev IP type = IPv4v6
    New IP type = IPv4
    If the companion iface(ipv6) is in use, we do not want to redial but simply
    tear down the call.
    If the comp iface is not in use update the apn tbl to the new ip type.
    This will ensure that the other iface type call will not be brought up
    in routing. The unused iface will be cleaned up 
   -------------------------------------------------------------------------*/
    case DS_PROFILE_PDP_IPV4V6:
      if(new_pdp_type == DS_PROFILE_PDP_IPV4)
      {
        v4_inst = pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV4];
        if((v4_inst == NULL) || (!v4_inst->in_use))
        {
          set_bearer_going_down = TRUE;
          pdn_mgr_p->net_down_reason = DS_NET_DOWN_REASON_APN_PRM_CHG;
          local_err = DS_LOCAL_ERR_VAL_3;
          goto exit_func;
        }

        ip_support = DS_IPV4_ONLY;
        reset_gateway_iid = FALSE;

        v6_inst = pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV6];
        if (v6_inst == NULL)
        {
          set_bearer_going_down = TRUE;
          pdn_mgr_p->net_down_reason = DS_NET_DOWN_REASON_APN_PRM_CHG;
          local_err = DS_LOCAL_ERR_VAL_4;
          goto exit_func;
        }

      /*------------------------------------------------------------------------ 
          Here do not dissociate V6 because it needs to be gracefully torn down
          by V6 SM.
        --------------------------------------------------------------------------*/
        v6_sm_state = v6_inst->ds_pdn_v6_sm_cb->state;

        if((v6_sm_state ==  DS_PDN_V6_WAITING_FOR_NET_UP_STATE) ||
           (v6_sm_state ==  DS_PDN_V6_NET_UP_STATE))       
        {
          ds_pdn_v6_process_ip6_stop_ev(v6_inst->ds_pdn_v6_sm_cb);
        }
      }
      else if (new_pdp_type == DS_PROFILE_PDP_IPV6)
      {

        v4_inst = pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV4];
        v6_inst = pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV6];
        
        if((v6_inst == NULL) || (!v6_inst->in_use))
        {
          set_bearer_going_down = TRUE;
          pdn_mgr_p->net_down_reason = DS_NET_DOWN_REASON_APN_PRM_CHG;
          local_err = DS_LOCAL_ERR_VAL_5;
          goto exit_func;
        }

        ip_support = DS_IPV6_ONLY;
        if (v4_inst != NULL)
        {
          ds_pdn_mgr_dissociate_instance(pdn_mgr_p, ip_support);
        }

        ds_apn_table_update_apn_table(old_apn, ip_support, pdn_mgr_p);
      }      
      break;

    default:
      break;

  }

  /*------------------------------------------------------------------------ 
    Check for network override IP
  ------------------------------------------------------------------------*/
  ds_pdn_hdlr_get_current_plmn(&current_plmn);

  if(has_apn_changed)
  {
    override_ip_bmask = ds_3gpp_network_override_hdlr_get_overridden_ip(
                             (byte *)new_apn,
                             DS_3GPP_THROT_RAT_LTE,
                             &current_plmn);
  }
  else
  {
    override_ip_bmask = ds_3gpp_network_override_hdlr_get_overridden_ip(
                             (byte *)old_apn,
                             DS_3GPP_THROT_RAT_LTE,
                             &current_plmn);
  }

  /*------------------------------------------------------------------------ 
    Overridden IP type and PDP IP changed should be the same.
    ------------------------------------------------------------------------*/
  DS_MSG2(MSG_LEGACY_HIGH, "Network override IP in place:Overridden IP:%d, "
          "new PDP IP:%d",override_ip_bmask, new_pdp_type);

  if(override_ip_bmask != DS_3GPP_PDP_UNDEFINED_BMASK)
  {      
    if (DS_3GPP_NW_OVERRIDE_IPV4_ALLOWED(override_ip_bmask))
    {
      if(DS_3GPP_NW_OVERRIDE_NON_IP_ALLOWED(override_ip_bmask))
      {
        if ((new_pdp_type != DS_PROFILE_PDP_NON_IP) || 
            (new_pdp_type != DS_PROFILE_PDP_IPV4))
        {
          set_bearer_going_down = TRUE;
        }
      }
      
      reset_gateway_iid = FALSE;           
    } /* IPV4 only allowed */

    if (DS_3GPP_NW_OVERRIDE_IPV6_ALLOWED(override_ip_bmask))
    {
      if(DS_3GPP_NW_OVERRIDE_NON_IP_ALLOWED(override_ip_bmask))
      {
        if ((new_pdp_type != DS_PROFILE_PDP_NON_IP) || 
            (new_pdp_type != DS_PROFILE_PDP_IPV6))
        {
          set_bearer_going_down = TRUE;
        }
      }
    } /* IPV6 only allowed */ 
           
    if (DS_3GPP_NW_OVERRIDE_IPV6_ALLOWED(override_ip_bmask))
    {
      if((new_pdp_type != DS_PROFILE_PDP_IPV4V6) || 
         (new_pdp_type != DS_PROFILE_PDP_IPV4) || 
         (new_pdp_type != DS_PROFILE_PDP_IPV6))
      {
        set_bearer_going_down = TRUE;         
      }
    } /* IPV4V6 only allowed */ 

    if (DS_3GPP_NW_OVERRIDE_NON_IP_ALLOWED(override_ip_bmask))
    {
      if(new_pdp_type != DS_PROFILE_PDP_NON_IP)
      {
        set_bearer_going_down = TRUE;          
      }
    } /* NON-IP only allowed */

    if (set_bearer_going_down) 
    {
      pdn_mgr_p->net_down_reason = DS_NET_DOWN_REASON_APN_PRM_CHG;
      set_bearer_going_down = TRUE;
      local_err = DS_LOCAL_ERR_VAL_10;
      goto exit_func;
    }

  } /* Throttled by network override */

  /*--------------------------------------------------------------------- 
    If apn_disable flag is set move the bearer context to DOWN
  ----------------------------------------------------------------------*/
  if (profile_ptr.apn_disable_flag)
  {    
    set_bearer_going_down = TRUE;
    pdn_mgr_p->net_down_reason = DS_NET_DOWN_REASON_APN_PRM_CHG;
    local_err = DS_LOCAL_ERR_VAL_6;
    goto exit_func;
  }

  /*---------------------------------------------------------------------- 
   If APN bearer is not  DS_UMTS_PDP_PROFILE_APN_BEARER_FLAG_L tear down
   the call because it is an incompatible bearer
   ----------------------------------------------------------------------*/
  if((profile_ptr.apn_bearer & DS_PROFILE_APN_BEARER_FLAG_L) == 0)
  {
    set_bearer_going_down = TRUE;
    pdn_mgr_p->net_down_reason = DS_NET_DOWN_REASON_APN_PRM_CHG;
    local_err = DS_LOCAL_ERR_VAL_7;
    goto exit_func;
  }

  /*---------------------------------------------------------------------- 
    If apn name has changed, check if there exists another PDN UP with the
    same APN name. If it does then the bearer will have to be brought
    down.
    Also check if the changed apn is throttled.
   
    Use the new PDP type because we will anyways be reoriginating a
    new PDN context with the new PDP type.
  ------------------------------------------------------------------------*/
  if (has_apn_changed)
  {
    switch(new_pdp_type)
    {
      case DS_PROFILE_PDP_IPV4:
      	 v4_inst = pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV4];
        if(v4_inst != NULL)
        {
          if(ds_3gpp_pdn_throt_is_apn_throttled((byte *)profile_ptr.context.apn,
                                                DS_PDN_INSTANCE_IPV4,
                                                pdn_mgr_p, TRUE))
          {
            pdn_mgr_p->net_down_reason = DS_NET_DOWN_REASON_APN_PRM_CHG;
            set_bearer_going_down = TRUE;
            goto exit_func;
          }
        }
        reset_gateway_iid = FALSE;
        break;

      case DS_PROFILE_PDP_IPV6:
        v6_inst = pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV6];      	
        if(v6_inst != NULL)
        {
          if(ds_3gpp_pdn_throt_is_apn_throttled((byte *)profile_ptr.context.apn,
                                                DS_PDN_INSTANCE_IPV6,
                                                pdn_mgr_p, TRUE))
          {
            pdn_mgr_p->net_down_reason = DS_NET_DOWN_REASON_APN_PRM_CHG;
            set_bearer_going_down = TRUE;
            goto exit_func;
          }
        }
        break;

      case DS_PROFILE_PDP_IPV4V6:
        v4_inst = pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV4];       
        v6_inst = pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV6];       
        if( v4_inst != NULL && v6_inst != NULL )
        {
          if(ds_3gpp_pdn_throt_is_apn_throttled((byte *)profile_ptr.context.apn,
                                                DS_PDN_INSTANCE_IPV4,
                                                pdn_mgr_p, TRUE) ||
             ds_3gpp_pdn_throt_is_apn_throttled((byte *)profile_ptr.context.apn,
                                                DS_PDN_INSTANCE_IPV6,
                                                pdn_mgr_p, TRUE))
          {
            pdn_mgr_p->net_down_reason = DS_NET_DOWN_REASON_APN_PRM_CHG;
            set_bearer_going_down = TRUE;
            goto exit_func;
          }
        }
        break;

      default:
         break;
    }

    /*------------------------------------------------------------------------ 
      In case there were 2 PDN's to the same APN because of networking
      returning a single bearer..In this case
      PDN1 --> IPV4 single bearer --> apn1
      PDN2 --> IPV6 single bearer --> apn1
     
      Now PDN 1 will be torn down and a redial will happen on PDN 2.
     ------------------------------------------------------------------------*/
    for (pdn_index = 0;pdn_index < DS_3GPP_MAX_PDN_CONTEXT;pdn_index++)
    {
      temp_pdn_mgr_p = ds_pdn_mgr_get_pdn_ptr_from_index(pdn_index);
      if( !ds_pdn_mgr_validate_pdn_ptr(temp_pdn_mgr_p) ||
      	   pdn_index == pdn_mgr_p->index )
      {
        continue;
      }

      (void)ds_apn_table_get_resolved_apn_name(temp_pdn_mgr_p,
                                               (char**)&resolved_apn);
      if((strncasecmp((char*)new_apn,
      	                (char*)temp_pdn_mgr_p->pdp_profile->context.apn, 
                       DS_PROFILE_APN_STRING_LEN+1) == 0) ||
         ((resolved_apn != NULL) && 
          (strncasecmp((char*)new_apn, (char*)resolved_apn,
                        DS_PROFILE_APN_STRING_LEN+1)== 0)))
      {
        /*-------------------------------------------------------------------- 
          APN match has occured, Check if PDN is up
          ---------------------------------------------------------------------*/
        if (temp_pdn_mgr_p->state == DS_PDN_MGR_STATE_UP)
        {
          DS_ERROR_LOG_1("PDN =d has the new changed apn and "
                         "is UP. No redial",
                         temp_pdn_mgr_p->index);
          set_bearer_going_down = TRUE;
          pdn_mgr_p->net_down_reason = DS_NET_DOWN_REASON_APN_PRM_CHG;
          local_err = DS_LOCAL_ERR_VAL_8;
          goto exit_func;
        }
      }
    }

    /*----------------------------------------------------------------------- 
      If apn has changed, clear the PDN limit timer
     -----------------------------------------------------------------------*/
    if((pdn_limit_index = ds_3gpp_pdn_limit_tbl_find_entry((byte*)new_apn)) 
                                        != DS_3GPP_PDN_LIMIT_TBL_INVALID_ENTRY)
    {
      ds_3gpp_pdn_limit_reset_pdn_conn_cntr(pdn_limit_index);
      ds_3gpp_pdn_limit_clr_pdn_conn_tmr(pdn_limit_index);
    }

    if(!ds_pdn_nv_manager_get_null_apn_support_for_non_attach_pdn() &&
       ds_pdn_mgr_reject_null_apn(pdn_mgr_p, new_apn))
    {
      set_bearer_going_down = TRUE;
      pdn_mgr_p->net_down_reason = DS_NET_DOWN_REASON_NULL_APN_DISALLOWED;
      local_err = DS_LOCAL_ERR_VAL_9;
      goto exit_func;
    }

    /*------------------------------------------------------------------------ 
      If this part of code is hit then bearer state will be left in PENDING
      REDIAL, now update the apn tbl.
      This will be donw in two steps if apn_chg occured
      1. Delete the old apn tbl entry
      2. Add the new apn tbl entry
      -----------------------------------------------------------------------*/
    if(ds_apn_table_remove_entry_in_apn_table(pdn_mgr_p))
    {
      switch(new_pdp_type)
      {
        case DS_PROFILE_PDP_IPV4:
         ip_support = DS_IPV4_SINGLE_BEARER;
         break;

        case DS_PROFILE_PDP_IPV6:
         ip_support = DS_IPV6_SINGLE_BEARER;
         break;
        
        case DS_PROFILE_PDP_IPV4V6:
         ip_support = DS_IPV4V6;
         break;

        default:
          break;
      }

      DS_MSG1(MSG_LEGACY_HIGH, "New apn tbl addition : IP_TYPE %d",
      	                        ip_support);
      ds_apn_table_add_pdn(new_apn, ip_support, pdn_mgr_p);      
    }
    else
    {
      DS_ERROR_LOG_0("APN tbl entry deletion failure");
    }
  }

 /*-------------------------------------------------------------------------- 
   If set_bearer_down is set, Set both the PDN and the bearer to GOING DOWN
   state
   --------------------------------------------------------------------------*/
exit_func:
  if(set_bearer_going_down)
  {
    DS_ERROR_LOG_1("Setting bearer and PDN to going down state, local_err =d",
    	              local_err);
    bearer_mgr_p->state = DS_BEARER_MGR_STATE_GOING_DOWN;
    pdn_mgr_p->state = DS_PDN_MGR_STATE_GOING_DOWN;
    pdn_mgr_p->lte_params->apn_param_chg_teardown = FALSE;
  }
  
  if(pdn_mgr_p->lte_params->apn_param_chg_teardown)
  {
    if(reset_gateway_iid == TRUE)
    {
      v6_inst = pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV6];
      if((v6_inst != NULL) && (v6_inst->in_use == TRUE))
      {
        DS_MSG0(MSG_LEGACY_HIGH,
        	       "Reseting the Gateway IID of primary IPv6 addr");
        //PS_IFACE_SET_IPV6_GATEWAY_IID(v6_iface_p, 0);   need to change this later
      }
    }
  } 
 
  return;
}/* ds_eps_pdn_move_bearer_from_pending_redial_to_down_if_needed */

/*===========================================================================
FUNCTION  ds_eps_pdn_mgr_redial_hdlr

DESCRIPTION
  This functions redials PDN connectivity for pending redial PDNs.

PARAMETERS
  Bearer context.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_eps_pdn_mgr_redial_hdlr
(
  ds_bearer_mgr_s      *bearer_mgr_p
)
{  
  ds_pdn_mgr_call_info_type              call_info = {0};

  if(!ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p))
  {
   	return;
  }

  call_info.call_instance = bearer_mgr_p->index;
  call_info.low_priority_signaling = 
         ds_pdn_mgr_get_low_priority_signaling_bit(bearer_mgr_p->pdn_mgr_ptr);
  
  ds_eps_initiate_pdn_connection(&call_info);

  bearer_mgr_p->state = DS_BEARER_MGR_STATE_COMING_UP;
}/* ds_eps_pdn_mgr_redial_hdlr */

/*===========================================================================
FUNCTION DS_EPS_PDN_HANDLE_APN_PARAM_CHG_REDIAL

DESCRIPTION
 Fetch the apn parameters from profile and send out a pdn connectivity request.
 
PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_eps_pdn_handle_apn_param_chg_redial
(
   ds_pdn_mgr_s     *pdn_mgr_p,
   ds_bearer_mgr_s  *bearer_mgr_p
)
{

  if ( !ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p) ||
  	    !ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p) )
  {
    return;
  }

 /*------------------------------------------------------------------------- 
    Populate the PDN context with the modified APN parameters and move the
    state to COMING_UP
  -------------------------------------------------------------------------*/
  if( ds_profile_setting_from_profile_id(
  	            pdn_mgr_p->pdp_profile_num, 
               pdn_mgr_p->pdp_profile) != DS_PROFILE_RESULT_SUCCESS )
  {
   	return;
  }

  pdn_mgr_p->state = DS_PDN_MGR_STATE_COMING_UP;
  (void)ds_eps_pdn_mgr_redial_hdlr(bearer_mgr_p);
  
  return;
}

/*===========================================================================
FUNCTION ds_eps_pdn_attach_sm_handle_last_bearer_deact

DESCRIPTION
  If there are any pending calls, then GET_PDN_CONN_IND is cached and its
  processing is delayed till all pending calls are ended.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_eps_pdn_attach_sm_handle_last_bearer_deact
(
  void
)
{
   sys_sys_mode_e_type          mode = SYS_SYS_MODE_NONE;
   ds_eps_attach_sm_info_type   attach_info = {0};
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  mode = ds_pdn_hdlr_get_current_network_mode();
  if(mode == SYS_SYS_MODE_LTE)
  {
    /*----------------------------------------------------------------------- 
      Here the attac_sm state is changed to DS_EPS_PROCESS_GET_PDN_CONN_IND
      to handle get_pdn_conn_ind_hdlr
    ------------------------------------------------------------------------*/
    DS_MSG0(MSG_LEGACY_HIGH, "No pending bearers, Processing GET_PDN_CONN_IND");
    ds_eps_pdn_attach_sm_set_state(DS_EPS_REDIAL_GET_PDN_CONN_IND);
    attach_info.responding_to_page = ds_eps_pdn_attach_sm.cached_paging_response;
    attach_info.seq_num = ds_eps_pdn_attach_sm.cached_seq_num;
    attach_info.attach_wo_pdn_supported = ds_eps_pdn_attach_sm.cached_attach_wo_pdn_supported;
    ds_eps_get_pdn_conn_ind_hdlr(&attach_info);
  }
  else
  {
    DS_MSG0(MSG_LEGACY_HIGH, "Resetting EPS attach SM");
    ds_eps_attach_sm_reset_params();
    /*--------------------------------------------------------------------- 
      Reset network override handler when throttle behaviour is to
      BLOCK PLMN
    -----------------------------------------------------------------------*/
    if(ds_3gpp_data_retry_is_enabled())
    {
      ds_3gpp_network_override_hdlr_reset(FALSE);
    }
  }

  return;
} /* ds_eps_pdn_attach_sm_handle_last_bearer_deact */

/*===========================================================================
FUNCTION DS_EPS_RESET_ATTACH_SM_PARAMS

DESCRIPTION
  This function is used to reset the attch state machine parameters

PARAMETERS 
  None
 
DEPENDENCIES
  None.

RETURN VALUE
  None
 
SIDE EFFECTS
  None.
===========================================================================*/
void ds_eps_reset_attach_sm_params
(
  void
)
{
  ds_pdn_apm_ext_error_code     apm_error_code = DS_PDN_APM_UNSEPCIFIED;
  ds_pdn_apm_ind_pay_load   apm_pay_load = {0};
/*-----------------------------------------------------------------------*/

  DS_MSG0(MSG_LEGACY_HIGH, "Resetting attach SM params.");
  /*--------------------------------------------------------------------------
    Clear LTE ATTACH procedure state machine
  --------------------------------------------------------------------------*/
  if (ds_eps_pdn_attach_sm.attach_state != DS_EPS_ATTACH_STATE_NULL)
  {
    apm_pay_load.ind_field_mask = 0;
    apm_error_code = ds_pdn_apm_ind(DS_PDN_APM_DETACH_IND, &apm_pay_load);
    DS_MSG1(MSG_LEGACY_HIGH, "Posting Detach indication to DSD and APM,"
                     "Err code: %d",apm_error_code);
  }

  ds_pdn_cp_data_reset_max_ul_packet_cap();

  ds_eps_pdn_attach_sm_set_state(DS_EPS_ATTACH_STATE_NULL);
  ds_pdn_hdlr_set_attach_complete_flag(FALSE);
  ds_eps_pdn_attach_sm.ota_attach_performed = FALSE;
  ds_eps_pdn_attach_sm.seq_num = 0;
  ds_eps_pdn_attach_sm.is_gw_originated = FALSE;
  ds_eps_pdn_attach_sm.responding_to_page = FALSE;
  ds_eps_pdn_attach_sm.attach_wo_pdn_supported = FALSE;
  memset((void*)&ds_eps_pdn_attach_sm.prev_attach_status,
         0,
         sizeof(ds_eps_prev_attach_status_type));

  if(ds_pdn_mgr_is_irat_in_progress())
  {
    ds_pdn_hdlr_ps_data_suspend_resume_hdlr(FALSE);
    ds_pdn_mgr_clear_irat_info();
  }

  if(ds_pdn_nv_manager_is_iot_rpm_enabled())
  {
    ds_3gpp_pdn_cntxt_disable_lte_if_attach_profile_unavailable();
  }

  ds_3gpp_pdn_throt_advertise_blocked_apns_current_plmn();
  ds_3gpp_pdn_throt_sm_advertise_blocked_apns_to_mppm();

  return;
}/*ds_eps_pdn_cntx_reset_attach_sm_params*/

/*===========================================================================
FUNCTION ds_eps_pdn_get_lte_attach_dual_ip_fb_status

DESCRIPTION 
  This function returns the flag which tells whether dual IP FB feature is
  enabled or not
 
PARAMETERS
  None
 
DEPENDENCIES
  None 

RETURN VALUE
  TRUE: If the feature is enabled
  FALSE: Otherwise
 
SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_eps_pdn_get_lte_attach_dual_ip_fb_status
(
  void
)
{
  DS_MSG1(MSG_LEGACY_HIGH, "Returning Dual IP FB feature status : %d",
          ds_eps_lte_attach_fb_ip_type.fallback_enabled);
  return ds_eps_lte_attach_fb_ip_type.fallback_enabled;
}

/*===========================================================================
FUNCTION ds_eps_pdn_get_lte_attach_dual_ip_fb_ip_type

DESCRIPTION 
  This function returns Attach Dual IP FB IP type.
 
PARAMETERS
  None
  
DEPENDENCIES
  The fall back IP type is to be used when the network responds with a
  PDN Conn Rej with ESM cause #28 for Initial Attach request (only). 

RETURN VALUE
  Cached PDN IP type that UE will fall back to for certain attach failures with
  specific ESM cause codes
  
SIDE EFFECTS
  None.
===========================================================================*/
ds_eps_attach_fb_type ds_eps_pdn_get_lte_attach_dual_ip_fb_ip_type
(  
  void
)
{
  ds_eps_attach_fb_type             fb_ip_type = DS_EPS_ATTACH_FB_IP_TYPE_MIN;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(ds_eps_lte_attach_fb_ip_type.fallback_enabled == TRUE)
  {
    fb_ip_type = ds_eps_lte_attach_fb_ip_type.fallback_ip_type;
  }

  return fb_ip_type;
}

/*===========================================================================
FUNCTION ds_eps_pdn_set_lte_attach_dual_ip_fb_ip_type

DESCRIPTION 
  This function sets Attach Dual IP FB IP type.
 
PARAMETERS
  None
  
DEPENDENCIES
  The fall back IP type is to be used when the network responds with a
  PDN Conn Rej with ESM cause #28 for Initial Attach request (only). 

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void ds_eps_pdn_set_lte_attach_dual_ip_fb_ip_type
(
  boolean                           activation_status,
  ds_eps_attach_fb_type             attach_fb_ip_type
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DS_MSG1(MSG_LEGACY_HIGH,"Setting Attach FB ip type activation status :%d",
                   activation_status);

  ds_eps_lte_attach_fb_ip_type.fallback_enabled = activation_status;
  if(activation_status == TRUE)
  {
    if(attach_fb_ip_type <= DS_EPS_ATTACH_FB_IP_TYPE_MIN ||
       attach_fb_ip_type >= DS_EPS_ATTACH_FB_IP_TYPE_MAX)
    {
      DS_ERROR_LOG_0("Invalid FB IP Type passed, default value V4 selected");
      ds_eps_lte_attach_fb_ip_type.fallback_ip_type = 
                                            DS_EPS_ATTACH_FB_IP_TYPE_IPV4;
    }
    else
    {
      ds_eps_lte_attach_fb_ip_type.fallback_ip_type = attach_fb_ip_type;
    }
  }
  else
  {
    DS_ERROR_LOG_0("Dual IP fallback feature disabled!!!!");
    ds_eps_lte_attach_fb_ip_type.fallback_ip_type = 
                                           DS_EPS_ATTACH_FB_IP_TYPE_MIN;
  }

  return;
}

/*===========================================================================
FUNCTION  DS_EPS_GET_ATTACH_PDN

DESCRIPTION
This function gets the attach PDN from the APN table if it exists.

PARAMETERS
  pdn_mgr_p : OUT - Double Pointer to PDN context if there is one already
                     present in APN table.

DEPENDENCIES
  None.

RETURN VALUE
 TRUE: Success
 FALSE: Failure
SIDE EFFECTS
  None.

===========================================================================*/
static boolean ds_eps_get_attach_pdn
(
  ds_pdn_mgr_s        **pdn_mgr_p
)
{
  uint16                       attach_profile_num;
  ds_profile_type_s            attach_profile_info = {{0}};
  ds_profile_context_type      attach_profile_context = {0};
  ds_apn_ip_support_type_e     attach_ip_support;
  ds_pdn_mgr_s                *v4_pdn_p=NULL,*v6_pdn_p=NULL,*non_ip_pdn_p = NULL;
  /*----------------------------------------------------------------------*/

  *pdn_mgr_p = ds_pdn_mgr_get_def_pdn();

  /* MO exception Handling - in the case of single PDN MO Exception
     data call from ACB state, default PDN may exist already but
     still needs to go through with the force attach properly */
  if((*pdn_mgr_p != NULL) &&
     ((*pdn_mgr_p)->lte_params != NULL) &&
     (FALSE == (*pdn_mgr_p)->lte_params->mo_exception_data))
  {
    DS_MSG1(MSG_LEGACY_HIGH,
    	      "There is already a default PDN 0x%x", *pdn_mgr_p);
    return TRUE;
  }

  attach_profile_num = ds_eps_pdn_get_attach_prof();
  if(DS_PROFILE_RESULT_SUCCESS != ds_profile_setting_from_profile_id(
  	                                  attach_profile_num, &attach_profile_info))
  {
    return FALSE;
  }

  attach_profile_context = attach_profile_info.context;
  
  /*--------------------------------------------------------------------------
  Check if there is an entry in APN table for attach APN
  This is to handle the case where an APN entry has a PDN context 
  allocated but the default flag was not set(No iface_up processed yet). 
  ------------------------------------------------------------------------*/
  if (ds_apn_table_get_apn_ip_support_info(attach_profile_context.apn,
                                           &attach_ip_support) == TRUE)
  {
    /*-------------------------------------------------------------------------
    Below fns must return TRUE always since we entered here by finding an
    entry in APN table.
    -------------------------------------------------------------------------*/
    if ( FALSE == ds_apn_table_get_v4_pdn(attach_profile_context.apn,
                                        &v4_pdn_p) ||
         FALSE == ds_apn_table_get_v6_pdn(attach_profile_context.apn,
                                        &v6_pdn_p) ||
         FALSE == ds_apn_table_get_non_ip_pdn(attach_profile_context.apn,
                                            &non_ip_pdn_p))
    {
      DS_ASSERT(0);
      return FALSE;
    }

    /*-----------------------------------------------------------------------
    Now check if the IP type in attach profile and APN table matches.
    ----------------------------------------------------------------------*/
    DS_MSG_SPRINTF_3(MSG_LEGACY_HIGH,
                     "Attach APN: %s is present in APN table "
                     "IP type: %d, PDP Type: %d",
                     attach_profile_context.apn,
                     attach_ip_support, 
                     attach_profile_context.pdp_type);

    *pdn_mgr_p = (ds_pdn_mgr_s*)
                  ds_pdn_call_hdlr_get_attach_pdn_based_on_profile
                  (
                    v4_pdn_p,
                    v6_pdn_p,
                    non_ip_pdn_p,
                    attach_profile_context,
                    attach_ip_support
                  );
    /*-------------------------------------------------------------------------
    If we got a valid PDN out of our comparison in the function above, then
    we can set that as default here
    -------------------------------------------------------------------------*/
    if(ds_pdn_mgr_validate_pdn_ptr(*pdn_mgr_p))
    {
      (*pdn_mgr_p)->is_default = TRUE;
    }
  }//APN present in APN table
  else
  {//There is no entry in APN table.
    *pdn_mgr_p = NULL;
  }
  
  return TRUE;
}

/*===========================================================================
FUNCTION  ds_eps_bringup_instances_for_force_attach

DESCRIPTION
  This function brings up the instances for force attach as triggered
  by MO Exception.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_eps_bringup_instances_for_force_attach
(
  ds_pdn_mgr_s*     pdn_mgr_p
)
{
  DS_MSG0(MSG_LEGACY_HIGH,"Bring up instance for force attach");

  /*----------------------------------------------------------------------
    Set this PDN as default, since it was never done before
  -----------------------------------------------------------------------*/
  pdn_mgr_p->is_default = TRUE;

  /* Bring up the Instance that we saved for attach for the single PDN case */
  ds_pdn_cp_data_resume_inst_up_for_force_attach();

  return;
} /* ds_eps_bringup_instances_for_force_attach */

/*===========================================================================
FUNCTION  ds_eps_allocate_pdn_and_bringup_instances

DESCRIPTION
  This functions allocates a new PDN context and new DS ifaces and brings up
  the instances.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  DS3G_SUCCESS - if all operations went fine
  DS3G_FAILURE - on error

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_eps_allocate_pdn_and_bringup_instances
(
  uint8                    profile_no
)
{
  ds_apn_ip_support_type_e  ip_support;
  ds_pdn_mgr_s             *pdn_mgr_p       = NULL;
  ds_pdn_mgr_inst_type_s   *v4_inst         = NULL;
  ds_pdn_mgr_inst_type_s   *v6_inst         = NULL;
  ds_pdn_mgr_inst_type_s   *non_ip_inst     = NULL;
  ds_pdn_apm_ind_pay_load   ind_payload;
  ds_eps_attach_fb_type     attach_fb_ip_support = DS_EPS_ATTACH_FB_IP_TYPE_MIN;
  ds_local_err_val_e_type   local_err = DS_LOCAL_ERR_VAL_NONE;

  /*---------------------------------------------------------------------------
   Allocate a new PDN context
  ---------------------------------------------------------------------------*/
  pdn_mgr_p = ds_pdn_mgr_allocate_new_pdn_context();

  if(!ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p))
  {
    local_err = DS_LOCAL_ERR_VAL_0;
    goto exit_function;
  }

  /*----------------------------------------------------------------------
  Set this PDN as default, since it was never done before
  -----------------------------------------------------------------------*/
  pdn_mgr_p->is_default = TRUE;
  pdn_mgr_p->pdp_profile_num = profile_no;
  
  if(ds_profile_setting_from_profile_id(pdn_mgr_p->pdp_profile_num, 
                                        pdn_mgr_p->pdp_profile))
  {
    local_err = DS_LOCAL_ERR_VAL_1;
    goto exit_function;
  }

  if( FALSE == ds_pdn_mgr_process_pdp_profile_info(pdn_mgr_p,
  	                                                &ip_support) )
  {
    local_err = DS_LOCAL_ERR_VAL_2;
    goto exit_function;
  }

  /*------------------------------------------------------------------------- 
    Notify APM regarding ATTACH PDP Type only for attach PDN
  -------------------------------------------------------------------------*/
  if( pdn_mgr_p->is_default )
  {
    memset(&ind_payload, 0, sizeof(ds_pdn_apm_ind_pay_load));
    ind_payload.attach_pdp_type = pdn_mgr_p->pdp_profile->context.pdp_type;
    (void)ds_pdn_apm_ind(DS_PDN_APM_ATTACH_PDP_TYPE_IND, &ind_payload);
  }

  /*-------------------------------------------------------------------------
      Check if there was a previous attach attempt, that failed. This may
      be a retry attach request attempt
    -------------------------------------------------------------------------*/
  if(ds_eps_pdn_get_lte_attach_dual_ip_fb_status() == TRUE)
  {
    if (ds_eps_pdn_attach_sm.prev_attach_status.is_attach_fail == TRUE)
    {
      DS_MSG1(MSG_LEGACY_HIGH, 
      	       "Previous attach attempt failed with cause 0x%x",
              ds_eps_pdn_attach_sm.prev_attach_status.down_reason);

      if ((ip_support  == DS_IPV4V6)&&
          (ds_eps_pdn_attach_sm.prev_attach_status.down_reason ==
                                            DS_NET_DOWN_REASON_UNKNOWN_PDP)) 
      {
        /*--------------------------------------------------------------- 
          The previous attach attempt failed with ESM cause #28. So we
          cannot set PDN IP Type to IPV4V6. Set the PDN Ip type to
          IPV4 (or) IPV6 only depending on the NV setting. If NV setting
          is not configured, current behaviour default to PDN IP type
          IPV4.
          ---------------------------------------------------------------*/
        attach_fb_ip_support = ds_eps_pdn_get_lte_attach_dual_ip_fb_ip_type();

        if (attach_fb_ip_support == DS_EPS_ATTACH_FB_IP_TYPE_IPV6)
        {
          pdn_mgr_p->pdp_profile->context.pdp_type = DS_PROFILE_PDP_IPV6;
          ip_support = DS_IPV6_SINGLE_BEARER;
        }
        else
        {
          pdn_mgr_p->pdp_profile->context.pdp_type = DS_PROFILE_PDP_IPV4;
          ip_support = DS_IPV4_SINGLE_BEARER;
        }
      }
    }
  }

  /*--------------------------------------------------------------------------
      Update the APN table so that we have some info in the table before the 
      instance up handling is initiated.
  --------------------------------------------------------------------------*/
  if(ds_apn_table_add_pdn(pdn_mgr_p->pdp_profile->context.apn,
                          ip_support,
                          pdn_mgr_p) != TRUE)
  {
    local_err = DS_LOCAL_ERR_VAL_3;
    goto exit_function;
  }

  /*-------------------------------------------------------------------------
    Set the flag to TRUE. Flag is used to indicate that the attach process
    has been initiated
  -------------------------------------------------------------------------*/
  ds_pdn_hdlr_set_attach_initiated_flag(TRUE);

  if( pdn_mgr_p->pdp_profile->context.pdp_type == DS_PROFILE_PDP_IPV4 )
  {
    v4_inst = ds_pdn_mgr_allocate_new_instance(pdn_mgr_p,DS_PDN_INSTANCE_IPV4);
    
    if(v4_inst == NULL )
    {
      local_err = DS_LOCAL_ERR_VAL_4;
      goto exit_function;
    }

    //ds_3gpp_pdn_context_v6_sm_update(v4_inst); // revisit, need to update dssnet6 and ip6sm

    // Instance bring up
    if(!ds_pdn_mgr_instance_up_hdlr(pdn_mgr_p,DS_PDN_INSTANCE_IPV4,NULL,FALSE))
    {
      local_err = DS_LOCAL_ERR_VAL_5;
      goto exit_function;
    }
  }    
  else if( pdn_mgr_p->pdp_profile->context.pdp_type == DS_PROFILE_PDP_IPV6 )
  {
    v6_inst = ds_pdn_mgr_allocate_new_instance(pdn_mgr_p,DS_PDN_INSTANCE_IPV6);
   
    if(v6_inst == NULL )
    {
      local_err = DS_LOCAL_ERR_VAL_6;
      goto exit_function;
    }

    //ds_3gpp_pdn_context_v6_sm_update(v6_inst);  // revisit, need to update dssnet6 and ip6sm

    // Instance bring up
    if(!ds_pdn_mgr_instance_up_hdlr(pdn_mgr_p,DS_PDN_INSTANCE_IPV6,NULL,FALSE))
    {
      local_err = DS_LOCAL_ERR_VAL_7;
      goto exit_function;
    }
  }
  else if( pdn_mgr_p->pdp_profile->context.pdp_type == DS_PROFILE_PDP_IPV4V6 )
  {
    v4_inst = ds_pdn_mgr_allocate_new_instance(pdn_mgr_p,DS_PDN_INSTANCE_IPV4);
    v6_inst = ds_pdn_mgr_allocate_new_instance(pdn_mgr_p,DS_PDN_INSTANCE_IPV6);
   
    if(v4_inst == NULL || v6_inst == NULL )
    {
      local_err = DS_LOCAL_ERR_VAL_8;
      goto exit_function;
    }

    // Instance bring up - companion instance(v4) handling will be done as part of same function()
    if(!ds_pdn_mgr_instance_up_hdlr(pdn_mgr_p,DS_PDN_INSTANCE_IPV6,NULL,FALSE))
    {
      local_err = DS_LOCAL_ERR_VAL_9;
      goto exit_function;
    }
  }


  if( pdn_mgr_p->pdp_profile->context.pdp_type == DS_PROFILE_PDP_NON_IP)
  {
    non_ip_inst = ds_pdn_mgr_allocate_new_instance(pdn_mgr_p,DS_PDN_INSTANCE_NON_IP);
   
    if(non_ip_inst == NULL)
    {
      local_err = DS_LOCAL_ERR_VAL_11;
      goto exit_function;
    }
    
    // No APM rules for NON-IP PDN
    ds_pdn_apm_unset_ip_rule_required();

    // Instance bring up
    if(!ds_pdn_mgr_instance_up_hdlr(pdn_mgr_p,DS_PDN_INSTANCE_NON_IP,NULL,FALSE))
    {
      local_err = DS_LOCAL_ERR_VAL_12;
      goto exit_function;
    }
  }
  
exit_function:

  if(local_err != DS_LOCAL_ERR_VAL_NONE)
  {
    DS_MSG1(MSG_LEGACY_HIGH,
    	       "Bring up PDN failure, error : %d", local_err);  

    if( ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p) )
    {    
      ds_pdn_mgr_cleanup_pdn(pdn_mgr_p, DS_PDN_INSTANCE_MAX);
    }
    
    return FALSE;
  }

  return TRUE;
}

/*===========================================================================
FUNCTION  DS_EPS_BRING_UP_ATTACH_PDN

DESCRIPTION
  This function is responsible for the initiation of sending out PDN conn
  request during attach process. This function accesses the PDP registry,
  caches info in the PDN context, binds instances and PDNs appropriately and
  brings up instances.

  This function is called from 3GPP HDLR for EMM attach or from Resume handler
  to bring up the connection to default PDN

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  DS_EPS_BRINGUP_ATTACH_PDN_FAIL - if the attach APN is disabled
  DS_EPS_BRINGUP_ATTACH_PDN_PEND_REDIAL - if all operatins went
                      fine in Pending Redial handling
  DS_EPS_BRINGUP_ATTACH_PDN_SUCCESS - if all operations went fine
                      when it is not Pending Redial handling

SIDE EFFECTS
  None.
===========================================================================*/
ds_eps_bringup_attach_pdn_result_e ds_eps_bring_up_attach_pdn
(
  void
)
{
  ds_pdn_mgr_s                          *pdn_mgr_p = NULL;
  ds_eps_bringup_attach_pdn_result_e     return_value = 
                                           DS_EPS_BRINGUP_ATTACH_PDN_SUCCESS;
  int                                    index;
  uint16                                 profiles_list
                                           [DS_PDN_CFG_PROFILES_MAX + 1] = {0};
  int                                    num_profiles;
  uint8                                  profile;
  uint8                                  attach_profile_id = 0;
  ds_pdn_apm_ind_pay_load                ind_payload = {0};
  boolean                                attach_init = FALSE;
  boolean                                attach_wo_pdn_support = FALSE;
  ds_profile_type_s                      profile_info = {{0}};
  sys_plmn_id_s_type                     current_plmn_id;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  attach_wo_pdn_support = ds_eps_pdn_attach_sm.attach_wo_pdn_supported;

  ds_pdn_hdlr_get_current_plmn(&current_plmn_id);
  ds_3gpp_pdn_process_new_plmn(current_plmn_id, SYS_SYS_MODE_LTE);

  /*-----------------------------------------------------------------------
    Ensure that MH posts ATTACH_INIT_IND to APM only if this is a fresh 
    attach and not just because attach PDN is being brought up while there 
    is another active PDN.
   -----------------------------------------------------------------------*/
  if (FALSE == ds_pdn_mgr_is_any_other_pdn_up(NULL))
  {
    attach_init = TRUE;
    (void)ds_pdn_apm_ind(DS_PDN_APM_ATTACH_INIT_IND,&ind_payload); 
  
    /*----------------------------------------------------------------------
      Initiation of attach. Set Target_Rat to LTE
    ----------------------------------------------------------------------*/
    attach_profile_id = ds_eps_pdn_get_attach_prof();
    DS_MSG0(MSG_LEGACY_HIGH, "Attach initiation.");

    /*-------------------------------------------------------------------------
      Get attach wo PDN  value from the profile id
    -------------------------------------------------------------------------*/
    if( DS_PROFILE_RESULT_SUCCESS != ds_profile_setting_from_profile_id(
                                      attach_profile_id, &profile_info) )
    {
      DS_ERROR_LOG_0("Cannot read profile info");
    }
    
    if (TRUE == attach_wo_pdn_support && profile_info.attach_wo_pdn )
    {
      return ds_eps_attach_hdlr_bring_up_attach_wo_pdn();
    }
  }
  else
  {
    DS_ERROR_LOG_0("Other PDN UP. failing attach bring up.");
    return DS_EPS_BRINGUP_ATTACH_PDN_FAIL;
  }

  /*-------------------------------------------------------------------------
      We are here , it mean either
      1. NAS indicated attach with PDN or
      2. NAS indicated attach w/o PDN but profile indicated attach w/o PDN 
         is not supported.
      In case 2, we have to block the APN which were unblocked earlier 
    -------------------------------------------------------------------------*/
  if ( TRUE == attach_wo_pdn_support )
  {
    ds_eps_pdn_attach_sm.attach_wo_pdn_supported = FALSE;
    ds_3gpp_pdn_throt_advertise_blocked_apns_current_plmn();
  }

  /*-------------------------------------------------------------------------
    Check min APN list and if any APN is diabled,call the CM API to disable
    LTE altogether
  -------------------------------------------------------------------------*/
  if(ds_pdn_nv_manager_read_efs_nv(
                    "/nv/item_files/modem/data/3gpp/lteps/profile_list",
                    (unsigned char *) &profiles_list[0],
                    (sizeof(uint16) * DS_PDN_CFG_PROFILES_MAX)) == NV_DONE_S)
  {
    num_profiles = profiles_list[0];
    if((num_profiles < 0) || (num_profiles >=  DS_PDN_CFG_PROFILES_MAX))
    {
      DS_ERROR_LOG_0("Invalid Profile List, not checking apn disable flag");
    }
    else
    {
      for(index = 1; index <= num_profiles; index++)
      {
         profile = profiles_list[index];
         memset(&profile_info, 0 ,sizeof(ds_profile_type_s));
         
         if (DS_PROFILE_RESULT_SUCCESS != ds_profile_setting_from_profile_id(
                                           profile, &profile_info) )
         {
           continue;
         }

         if(profile_info.apn_disable_flag == TRUE)
         {
           /*------------------------------------------------------------------
             Disable the option of attaching to LTE network
           ------------------------------------------------------------------*/
           DS_MSG1(MSG_LEGACY_HIGH, "Disabling LTE srv during init - APN (%d)"
                                    "is disabled", profile);
           ds3g_msh_disable_lte_selection_ex();
           return_value = DS_EPS_BRINGUP_ATTACH_PDN_FAIL;
           goto exit_function;
        }
      }
    }
  }

  /*-----------------------------------------------------------------------
                         START FRESH ATTACH
  ------------------------------------------------------------------------*/
  /*-------------------------------------------------------------------------
   Check if we already have a PDN configured with attach profile in APN table.
   This can happen if the app tries to bring up a call before we do attach
   processing.  We may also already have a PDN configured for MO Exception
   call triggered on the same APN as the attach APN when we aren't attached
   yet, which is acceptable behavior.
  -------------------------------------------------------------------------*/
  if (ds_eps_get_attach_pdn(&pdn_mgr_p) == FALSE)
  {
    DS_ERROR_LOG_0("Error while getting Attach PDN ");
    return_value = DS_EPS_BRINGUP_ATTACH_PDN_FAIL;
    goto exit_function;
  }

  if( ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p) )
  {
    if ((pdn_mgr_p->lte_params != NULL) &&
        (TRUE == pdn_mgr_p->lte_params->mo_exception_data) &&
        (TRUE == ds_pdn_cp_data_is_force_attach_in_progress()) )
    {
      ds_eps_bringup_instances_for_force_attach(pdn_mgr_p);
    } /* MO Exception supported force attach case */
    else /* Normal case where PDN context exists */
    {
      if(pdn_mgr_p->state == DS_PDN_MGR_STATE_PEND_REDIAL)
      {
        if(ds_eps_is_silent_redial_allowed(pdn_mgr_p))
        {
          DS_MSG0(MSG_LEGACY_HIGH, "Redialing as part of atatch");

          ds_pdn_apm_unset_ip_rule_required();
          pdn_mgr_p->state = DS_PDN_MGR_STATE_COMING_UP;
       /*-------------------------------------------------------------------------
             Notify APM regarding ATTACH PDP Type
           -------------------------------------------------------------------------*/
          memset(&ind_payload, 0, sizeof(ds_pdn_apm_ind_pay_load));
          ind_payload.attach_pdp_type
            = (ds_profile_pdp_type_e)pdn_mgr_p->pdp_profile->context.pdp_type;
          (void)ds_pdn_apm_ind(DS_PDN_APM_ATTACH_PDP_TYPE_IND, &ind_payload);
          
          ds_eps_pdn_mgr_redial_hdlr(pdn_mgr_p->bearer_ptr_tbl
                                              [DS_3GPP_DEFAULT_BEARER_INDEX]);
          return DS_EPS_BRINGUP_ATTACH_PDN_PEND_REDIAL;          
        }
        else
        {
          ds_eps_pdn_attach_sm_set_state(DS_EPS_CACHED_GET_PDN_CONN_IND);
          ds_eps_pdn_attach_sm.cached_seq_num = ds_eps_pdn_attach_sm.seq_num;
          ds_eps_pdn_attach_sm.seq_num = 0;
          ds_bearer_mgr_local_cleanup(pdn_mgr_p->bearer_ptr_tbl
          	                                 [DS_3GPP_DEFAULT_BEARER_INDEX]);
        }
      }
      else if(pdn_mgr_p->state != DS_PDN_MGR_STATE_DOWN)
      {    
        DS_ERROR_LOG_1("Default PDN in state =d - Ignore Bring up",
                       pdn_mgr_p->state);
        return_value = DS_EPS_BRINGUP_ATTACH_PDN_FAIL;
        goto exit_function;
      }
    } /* Normal case where PDN context exists */
  } /* pdn_cntx_p != NULL */
  else
  {
    if(FALSE == ds_eps_allocate_pdn_and_bringup_instances(attach_profile_id))
    {
      return_value = DS_EPS_BRINGUP_ATTACH_PDN_FAIL;
    }
  }
  
exit_function:
  if(return_value == DS_EPS_BRINGUP_ATTACH_PDN_FAIL)
  {
    ds_eps_reset_attach_sm_params();
  }
  
  return return_value;
} /* ds_eps_pdn_cntxt_bring_up_attach_pdn */


/*===========================================================================
FUNCTION DS_EPS_ATTACH_SM_HANDLE_GET_PDN_CONN_IND

DESCRIPTION
  If there are any pending calls, then GET_PDN_CONN_IND is cached and its
  processing is delayed till all pending calls are ended.

PARAMETERS
  attach_info.

DEPENDENCIES
  None.

RETURN VALUE
  Returns True if GET_PDN_CONN_IND is cached

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_eps_attach_sm_handle_get_pdn_conn_ind
(
  ds_eps_attach_sm_info_type     *attach_info_ptr
)
{
  boolean                    any_active_bearers = FALSE;
  ds_pdn_mgr_s              *pdn_mgr_p = NULL;
  boolean                    any_hanging_context = FALSE;
  uint16                     attach_profile_id = 0;
  ds_bearer_mgr_s           *def_bearer = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (attach_info_ptr == NULL)
  {
    return FALSE;
  }

  /*--------------------------------------------------------------------------
    Request deactivation if any bearers are in COMING_UP state.
    3 Cases being handled here
   
    1. A bearer in coming up state which is default PDN's bearer and we
       receive PDN CONN IND
   
    Here we end that call and put the PDN and Bearer in Pending Redial
    Since we had a hanging bearer (COMING_UP or GOING_STATE)
    Now we need to cache the PDN Connectivity Request 
   
    Once the call end is received for hanging bearer we will perform a silent
    redial. The second check is not needed since it checks for Active bearers
   
    2. A bearer is in UP state and we receive a PDN CONN IND
   
    Here we see there is no hanging bearer but there is an active bearer so
    we still cache the PDN Conn request and once that call ends we will process
    this cached request
   
    3. Bearer is in PEND_REDIAL state and we receive a PDN CONN IND
   
    In this we dont need to cache the PDN CONN IND since it is not a hanging or
    active bearer so we go ahead and process the request right away.
   
    Note: Even if we get PDN CONN IND when a non-default PDN's bearer is being
    brought up or UP we will cache this request. This will just delay the
    processing of PDN CONN IND
   
    Check if a PDN is in going down state, This is needed because a bearer
    could be in DOWN STATE but the PDN may not be in DOWN state.
  --------------------------------------------------------------------------*/
 
  if(ds_eps_bearer_mgr_req_deact_for_pending_bearers() ||
     ds_pdn_mgr_is_any_pdn_going_down())
  {
    any_hanging_context = TRUE;
  }

  if(any_hanging_context == FALSE)
  {
    any_active_bearers = ds_eps_bearer_mgr_is_any_bearer_not_down();

  }

  if (any_active_bearers || any_hanging_context)
  {
    DS_MSG2(MSG_LEGACY_HIGH,
    	       "Found active bearer:%d  hanging bearer:%d, caching PDN_CONN_REQ",
            any_active_bearers, any_hanging_context);
    /*--------------------------------------------------------------------------
      If Def bearer for the attach/default PDN is in PEND_REDIAL state then
      change the PDN state to REDIAL as well.
    --------------------------------------------------------------------------*/
    pdn_mgr_p = ds_pdn_mgr_get_def_pdn();
    attach_profile_id = ds_eps_pdn_get_attach_prof();

    /*-------------------------------------------------------------------------
    Currently, only default PDNs can be put to PENDING_REDIAL
    -------------------------------------------------------------------------*/
    if(ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p))
    {
      def_bearer = pdn_mgr_p->bearer_ptr_tbl[DS_3GPP_DEFAULT_BEARER_INDEX];
      if( ds_bearer_mgr_validate_bearer_ptr(def_bearer) &&
          def_bearer->state == DS_BEARER_MGR_STATE_PEND_REDIAL )
      {
        DS_MSG0(MSG_LEGACY_HIGH, "Default bearer/Default PDN Pending REDIAL");
        pdn_mgr_p->state = DS_PDN_MGR_STATE_PEND_REDIAL;
        attach_profile_id = (uint8)pdn_mgr_p->pdp_profile_num;
      }
    }

    /*--------------------------------------------------------------------------
      Update the ATTACH state
    --------------------------------------------------------------------------*/
    ds_eps_pdn_attach_sm_set_state(DS_EPS_CACHED_GET_PDN_CONN_IND);
    ds_eps_pdn_attach_sm.cached_seq_num = attach_info_ptr->seq_num;
    ds_eps_pdn_attach_sm.cached_paging_response = 
                           attach_info_ptr->responding_to_page;
    ds_eps_pdn_attach_sm.cached_attach_wo_pdn_supported
                    = attach_info_ptr->attach_wo_pdn_supported;
    return TRUE;
  }
  
  return FALSE;
} /* ds_eps_attach_sm_handle_get_pdn_conn_ind */

/*===========================================================================
FUNCTION DS_EPS_GET_PDN_CONN_IND_HDLR

DESCRIPTION
  This function is used to handle the incoming attach request from NAS.
  It dispatches the EPS PDN context function to appropriately send out a
  PDN Connectivity request.

PARAMETERS
  attach_info_p.

DEPENDENCIES
  None.

RETURN VALUE
  DS3G_SUCCESS - if all operations went fine
  DS3G_FAILURE - if the attach APN is disabled

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_eps_get_pdn_conn_ind_hdlr
(
  ds_eps_attach_sm_info_type    *attach_info_ptr
)
{
  sys_sys_mode_e_type            network_mode;
  sys_plmn_id_s_type             current_plmn_id;
  ds_eps_attach_state_e          attach_state   = DS_EPS_ATTACH_STATE_INVALID;
  
  sys_radio_access_tech_e_type   active_rat;  
  ds_eps_bringup_attach_pdn_result_e  bringup_attach_pdn_status 
                                              = DS_EPS_BRINGUP_ATTACH_PDN_FAIL;  
  ds_pdn_apm_ind_pay_load        ind_payload;
  ds_local_err_val_e_type        err_val = DS_LOCAL_ERR_VAL_NONE;
  /*- - - - - - - - - - - - - - - - - - - - - - --------------- - - - - - - - - - - - - -*/

  network_mode = ds_pdn_hdlr_get_current_network_mode();
  if(network_mode != SYS_SYS_MODE_LTE)
  {
    ds_pdn_hdlr_set_network_mode(SYS_SYS_MODE_LTE);
  }

  memset(&current_plmn_id, 0, sizeof(sys_plmn_id_s_type));  
  ds_pdn_hdlr_get_current_plmn(&current_plmn_id);    

  do
  {  
    /*--------------------------------------------------------------------------- 
    On receiving a fresh attach request, Clear the PSM Restoration Info
    ---------------------------------------------------------------------------*/
    ds_pdn_psm_set_restoration_state(FALSE);
    ds_pdn_psm_free_info();
    ds_pdn_psm_free_rohc_info();
  
    /*--------------------------------------------------------------------------- 
      Check the state of attach_sm, If
      * It is DS_EPS_PROCESSING_GET_PDN_CONN_IND or
        DS_EPS_CACHED_GET_PDN_CONN_IND, then we are currently processing a
        previous attach request.
      * If it is DS_EPS_ATTACHED (or) DS_EPS_PROCESSED_CNTXT_TXFR_IND, we are
        currently cleaning up the old attach PDN while, this new attach request
        is received
        In above such cases save the cached seq num.
     
      However when a fresh attach request is serviced the attach state must be
      DS_EPS_PROCESS_GET_PDN_CONN_IND or NULL.
    -----------------------------------------------------------------------------*/
    attach_state = ds_eps_pdn_attach_sm.attach_state;
    if((attach_state  == DS_EPS_PROCESSING_GET_PDN_CONN_IND) ||
       (attach_state  == DS_EPS_CACHED_GET_PDN_CONN_IND)     ||
       (attach_state  == DS_EPS_ATTACHED))
    {
      DS_MSG2(MSG_LEGACY_HIGH,
      	       "Dup Attach request rx'ed (or) prev attach clean up in"
              "progress.Cache current seq_num:%d attach state %d",
              attach_info_ptr->seq_num, attach_state);
      ds_eps_pdn_attach_sm.cached_seq_num = attach_info_ptr->seq_num;
      ds_eps_pdn_attach_sm.cached_paging_response = attach_info_ptr->responding_to_page;
      ds_eps_pdn_attach_sm.cached_attach_wo_pdn_supported
        = attach_info_ptr->attach_wo_pdn_supported;
      err_val = DS_LOCAL_ERR_VAL_0;
      break;
    }
  
   /*---------------------------------------------------------------------------
      Set the seq_num in the attach_sm, Reset the cached_seq_num. This will be the
      seq num that would be processed.Set the is_attach_complete flag to FALSE.
      This is needed if the device is detached and is reattaching.
     ---------------------------------------------------------------------------*/
    ds_pdn_hdlr_set_attach_complete_flag(FALSE);
  
   /*----------------------------------------------------------------------
      If there are any pending calls, then GET_PDN_CONN_IND is cached and its
      processing is delayed till all pending calls are ended.
     ----------------------------------------------------------------------*/
    if(ds_eps_attach_sm_handle_get_pdn_conn_ind(attach_info_ptr))
    {
      err_val = DS_LOCAL_ERR_VAL_1;
      break;
    }
  
    /*----------------------------------------------------------------------
      Get the active RAT from NAS before attach attach PDN is brought up.
      Store this info and use it to determine if PCO /EPCO is to be used during
      attach.
    ----------------------------------------------------------------------*/
    active_rat = emm_get_active_rat();
    ds_pdn_mgr_set_active_rat(active_rat);
  
    /*--------------------------------------------------------------------------
      Move the attach SM to Processing GET_PDN_CONN ind as an attach req will
      go out shortly. If we decide not to perform attach, we will reset the SM
      to NULL
    --------------------------------------------------------------------------*/  
    ds_eps_pdn_attach_sm.seq_num = attach_info_ptr->seq_num;
    ds_eps_pdn_attach_sm.responding_to_page = attach_info_ptr->responding_to_page;
    ds_eps_pdn_attach_sm.attach_wo_pdn_supported 
                                   = attach_info_ptr->attach_wo_pdn_supported;  
    ds_eps_pdn_attach_sm.cached_seq_num = 0;
    ds_eps_pdn_attach_sm.cached_paging_response = FALSE;
    ds_eps_pdn_attach_sm.cached_attach_wo_pdn_supported = FALSE;
    ds_eps_pdn_attach_sm_set_state(DS_EPS_PROCESSING_GET_PDN_CONN_IND);

   /*--------------------------------------------------------------------------- 
     On receiving a fresh attach request, Clear the APN  reject throttling flag
   ---------------------------------------------------------------------------*/
    if(ds_3gpp_dam_set_pdn_blocked(FALSE))
    {
      ds_3gpp_dam_clr_timer();
    }

    /* Bring up attach PDN */
    bringup_attach_pdn_status = ds_eps_bring_up_attach_pdn();
  
    if(bringup_attach_pdn_status == DS_EPS_BRINGUP_ATTACH_PDN_FAIL)
    {
      /*----------------------------------------------------------------------
           Attach init failed.
           ----------------------------------------------------------------------*/
      DS_MSG0(MSG_LEGACY_HIGH,
              "Failed to bring up attach PDN, Reset RAT");
      memset(&ind_payload, 0, sizeof(ds_pdn_apm_ind_pay_load));
      ds_pdn_apm_ind(DS_PDN_APM_ATTACH_REJECT_IND, &ind_payload);  
      
      ds_eps_pdn_attach_sm.attach_wo_pdn_supported = FALSE;
      err_val = DS_LOCAL_ERR_VAL_2;
      break;
    }
  
    if(bringup_attach_pdn_status != DS_EPS_BRINGUP_ATTACH_PDN_SUCCESS)
    {
      err_val = DS_LOCAL_ERR_VAL_3;
      break;
    }
  }while(0);

  if(err_val == DS_LOCAL_ERR_VAL_NONE)
  {
    return TRUE;
  }
  else
  {
    DS_ERROR_LOG_1("GET_PDN_CONN_IND_HDLR - Failed with error =d", err_val);
    return FALSE;
  }
} /* ds_eps_get_pdn_conn_ind_hdlr */


/*===========================================================================
FUNCTION DS_EPS_IS_SILENT_REDIAL_ALLOWED

DESCRIPTION
  This function determines whether we can allow silent redial based on 
  pdp_type_to_use.

PARAMETERS
  pdn_cntxt_ptr.

DEPENDENCIES
  None.

RETURN VALUE
  boolean

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_eps_is_silent_redial_allowed
(
  ds_pdn_mgr_s  *pdn_mgr_p
)
{
  boolean                   status = FALSE;
  ds_pdn_mgr_s             *temp_pdn_p = NULL;
  boolean                   redial_allowed = FALSE;  
  ds_apn_ip_support_type_e  ip_support = DS_IP_TYPE_INVALID;
  ds_local_err_val_e_type   local_err = DS_LOCAL_ERR_VAL_NONE;

  do
  {
    DS_SYSTEM_HEAP_MEM_ALLOC(temp_pdn_p , sizeof(ds_pdn_mgr_s),ds_pdn_mgr_s*);
    if( NULL == temp_pdn_p )
    {
      local_err = DS_LOCAL_ERR_VAL_0;
      break;
    }
    
    memset(temp_pdn_p, 0, sizeof(ds_pdn_mgr_s));
    memscpy(temp_pdn_p, sizeof(ds_pdn_mgr_s), pdn_mgr_p, sizeof(ds_pdn_mgr_s));

    // Local copy of profile is used to avoid overwriting in original PDN.
    DS_SYSTEM_HEAP_MEM_ALLOC(temp_pdn_p->pdp_profile, sizeof(ds_profile_type_s),
                             ds_profile_type_s*);
    if( NULL == temp_pdn_p->pdp_profile )
    {
      local_err = DS_LOCAL_ERR_VAL_1;
      break;
    }
    
    memset(temp_pdn_p->pdp_profile, 0, sizeof(ds_profile_type_s));
    if(ds_profile_setting_from_profile_id(temp_pdn_p->pdp_profile_num, 
    	                                     temp_pdn_p->pdp_profile))
    {
      local_err = DS_LOCAL_ERR_VAL_2;
      break;
    }
    
    status = ds_pdn_mgr_process_pdp_profile_info(temp_pdn_p,
                                                 &ip_support);   
    if(status == FALSE)
    {
      local_err = DS_LOCAL_ERR_VAL_3;
      break;
    }

    /*-------------------------------------------------------------------------
      Silent Redial will be allowed only when the application requested ip_type 
      matches that of the pdp_type_to_use after determining pdp_type_to_use
      in roam/home scenarios.
      -------------------------------------------------------------------------*/
    if(temp_pdn_p->pdp_profile->context.pdp_type == 
                        pdn_mgr_p->pdp_profile->context.pdp_type)
    {
      redial_allowed = TRUE;
      break;
    }
  }while(0);

  if(DS_LOCAL_ERR_VAL_NONE != local_err)
  {
    DS_MSG1(MSG_LEGACY_ERROR, "ds_eps_is_silent_redial_allowed, error : %d",
                               local_err);
  }

  if (NULL != temp_pdn_p)
  {
    if( NULL != temp_pdn_p->pdp_profile )
    {      
      DS_MSG2(MSG_LEGACY_HIGH, "PDP type to use: %d, Redial_allowed: %d",
                       temp_pdn_p->pdp_profile->context.pdp_type,
                       redial_allowed);
      DS_SYSTEM_HEAP_MEM_FREE(temp_pdn_p->pdp_profile);
    }
  
    DS_SYSTEM_HEAP_MEM_FREE(temp_pdn_p);
  }

  return redial_allowed;
}/* ds_eps_is_silent_redial_allowed */

/*===========================================================================
FUNCTION DS_EPS_PDN_IP_FILL_PPP_PARAMS

DESCRIPTION
  This function fills in the PPP authentication and IPCP packet information
  for a PDP-IP call
    
PARAMETERS  
      profile_p      -  Ptr to the PDP profile parameters


DEPENDENCIES
  None.
  
RETURN VALUE
  none  

SIDE EFFECTS
  orig_params_ptr is updated with the PPP packet information from the profile
  
===========================================================================*/
boolean ds_eps_pdn_ip_fill_ppp_params
(
  ds_profile_type_s               *profile_p,
  ds_pdn_network_params_info_type *info_p,
  boolean                         *auth_info_being_sent,
  ds_pdn_protocol_cfg_options_s  *ds_proto_config_option_p
)
{
  boolean result = FALSE;
  net_cfg_params_mask_e_type valid_fields = 
      (net_cfg_params_mask_e_type)0x00000000; /* IPCP options to be requested*/
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  /*-------------------------------------------------------------------------
   Fill in Protocol specific options : for IP, this would contain the PPP 
   pkts (lcp, ipcp ) The purpose of the protocol configuration options
   information element is to transfer external network protocol options 
   associated with a PDP context activation . The IE is defined in 10.5.6.3.
  -------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
   start off assuming that the field is not valid ( nothing filled in ) 
  -------------------------------------------------------------------------*/
  ds_proto_config_option_p->valid  = FALSE; 
  
  /*-------------------------------------------------------------------------
    Config protocol field is 0x80 : ref: TS24.008 sec 10.5.6.3
    MSB should be 1 (ext field) protocol field b0-b2 should be 0x0 
    Initilize this field
  -------------------------------------------------------------------------*/
  ds_proto_config_option_p->config_protocol   = PROT_CFG_IE_OCT3_VAL; 
  ds_proto_config_option_p->num_protocol_ids  = 0;

  *auth_info_being_sent = FALSE;
  
  if( info_p != NULL &&
  	   ((info_p->valid_fields & NET_CFG_PRM_AUTH_PREF_MASK)||
       (info_p->valid_fields & NET_CFG_PRM_AUTH_USERNAME_MASK)||
       (info_p->valid_fields & NET_CFG_PRM_AUTH_PASSWORD_MASK)) &&
       (info_p->auth_info.params.user_id_len != 0))
  {
    DS_MSG1(MSG_LEGACY_HIGH, "User id length %d", info_p->auth_info.params.user_id_len);
   /*-------------------------------------------------------------------------
       Use the auth info from info_p
     -------------------------------------------------------------------------*/  
    result = ds_pdn_mgr_fill_auth_params_from_info(
                  ds_proto_config_option_p, profile_p, info_p);
  }
  else
  {
   /*-------------------------------------------------------------------------
      Use the auth info from profile
    -------------------------------------------------------------------------*/
    result = ds_pdn_mgr_fill_auth_params_from_profile(
                  ds_proto_config_option_p,profile_p);   
  }

  if(result) 
  {
    *auth_info_being_sent = TRUE;
  }
 
  if((profile_p->context.pdp_type == DS_PROFILE_PDP_IPV4) ||
     (profile_p->context.pdp_type == DS_PROFILE_PDP_IPV4V6) )
  {
    if((info_p != NULL) &&
    	  ((info_p->valid_fields & NET_CFG_PRM_PRI_DNS_MASK) ||
        (info_p->valid_fields & NET_CFG_PRM_SEC_DNS_MASK)) )
    {
      valid_fields = (net_cfg_params_mask_e_type)(info_p->valid_fields);
   /*---------------------------------------------------------------------------
       We convert the DNS address from Little Endian to Big Endian before
       sending the request to OTA, since QMI sends information in Little Endian
       format, however modifications to the profile are sent in Big Endian format
     ---------------------------------------------------------------------------*/
      result = ds_pdn_mgr_fill_ipcp_orig_params 
              (
               ds_proto_config_option_p,
               TRUE,
               ds_pdn_mgr_change_byte_order(info_p->ipcp_info.primary_dns),
               ds_pdn_mgr_change_byte_order(info_p->ipcp_info.secondary_dns),
               valid_fields
             );
    }
    else
    {
    /*-------------------------------------------------------------------------
        Request Primary and Secondary DNS addresses from the network.
        Since we are populating values from profile, only request DNS address.
      -------------------------------------------------------------------------*/
      valid_fields = (net_cfg_params_mask_e_type)
                 (NET_CFG_PRM_PRI_DNS_MASK | NET_CFG_PRM_SEC_DNS_MASK);
      result = ds_pdn_mgr_fill_ipcp_orig_params
               (
                 ds_proto_config_option_p,
                 FALSE,
                 profile_p->dns_addr.primary_dns_addr.pdp_addr.pdp_addr_ipv4,
                 profile_p->dns_addr.secondary_dns_addr.pdp_addr.pdp_addr_ipv4,
                 valid_fields
               );
    }
    
    result = ds_pdn_mgr_fill_ipv4_dns_container
             (
               ds_proto_config_option_p
             );
  }
  
  if((profile_p->context.pdp_type == DS_PROFILE_PDP_IPV6) ||
     (profile_p->context.pdp_type == DS_PROFILE_PDP_IPV4V6))
  {
    result = ds_pdn_mgr_fill_ipv6_dns_params
             (
               ds_proto_config_option_p
             );
  }

  return TRUE;
}

/*===========================================================================
FUNCTION DS_EPS_PDN_CNTXT_UPDATE_NTW_OVERRIDDEN_EPS_ORIG_PARAMS

DESCRIPTION
  This function updates the eps_orig params because the IP type
  corresponding to a particular apn has been overridden by the
  network.

PARAMETERS
  eps_orig_params_p - Pointer to EPS orig parameters
  pdn_ptr  - Pdn ptr

DEPENDENCIES
  None.

RETURN VALUE
  boolean TRUE - If the updation is successful
  `       FALSE - Otherwise

SIDE EFFECTS

===========================================================================*/

static boolean ds_eps_pdn_cntxt_update_ntw_overridden_eps_orig_params
(
   pdn_connection_data_T         *eps_orig_params_p,
   ds_pdn_mgr_s              *pdn_cntx_p,
   uint8                         ntw_overridden_ip_bmask

)
{
  boolean                       ret_val = FALSE;
  ds_profile_type_s      *profile_p = NULL;
  ds_pdn_apm_ind_pay_load       ind_payload = {0};
  ds_apn_ip_support_type_e      ds_apn_ip_support = DS_IP_TYPE_INVALID;
  /*--------------------------------------------------------------------------*/


  if (eps_orig_params_p == NULL ||
      !ds_pdn_mgr_validate_pdn_ptr(pdn_cntx_p))
  {
    return ret_val;
  }

  profile_p = pdn_cntx_p->pdp_profile;
  
  DS_MSG1(MSG_LEGACY_HIGH, "nw Overridden IP bmask %d", ntw_overridden_ip_bmask);

  /*--------------------------------------------------------------------- 
    Failure is returned for IPV6 if
    1. IPV6 NV is not turned on
    2. Profile type is V4 but Network overridden IP is IPV6 only
    Failure is returned for IPV4 if
    1. Profile type is V6 and Network overridden IP is IPV4 only
    If successful, update the PDN cache and APN table.
  -----------------------------------------------------------------------*/
  if (DS_3GPP_NW_OVERRIDE_IPV6_ALLOWED(ntw_overridden_ip_bmask))
  {
    if ((profile_p->context.pdp_type == DS_PROFILE_PDP_IPV6) || 
        (profile_p->context.pdp_type == DS_PROFILE_PDP_IPV4V6))
    {
      eps_orig_params_p->pdn_type = NAS_ESM_IPV6;
      profile_p->context.pdp_type = DS_PROFILE_PDP_IPV6;
      ds_apn_ip_support = DS_IPV6_ONLY;

      ds_pdn_apm_set_invalid_ipv4_addr(&ind_payload, 
                                       DS_NET_DOWN_REASON_IP_V6_ONLY_ALLOWED);
      ret_val = TRUE;
    }

  } /* IPv6 Allowed */

  else if (DS_3GPP_NW_OVERRIDE_IPV4_ALLOWED(ntw_overridden_ip_bmask))
  {
    if((profile_p->context.pdp_type == DS_PROFILE_PDP_IPV4) || 
       (profile_p->context.pdp_type == DS_PROFILE_PDP_IPV4V6)) 
    {
      eps_orig_params_p->pdn_type = NAS_ESM_IPV4;
      profile_p->context.pdp_type = DS_PROFILE_PDP_IPV4;
      ds_apn_ip_support = DS_IPV4_ONLY;
      ds_pdn_apm_set_invalid_ipv6_addr(&ind_payload,
                                       DS_NET_DOWN_REASON_IP_V4_ONLY_ALLOWED);
      ret_val = TRUE;
    }
  } /*IPv4 Allowed */
  
  if (ret_val)
  {
    (void)ds_apn_table_update_apn_table(
        pdn_cntx_p->pdp_profile->context.apn,
        ds_apn_ip_support, pdn_cntx_p);
  }

  return ret_val;
} /*ds_eps_pdn_cntxt_update_ntw_overridden_eps_orig_params*/

/*===========================================================================
FUNCTION DS_EPS_PDN_GET_ORIG_PARAMS

DESCRIPTION
This function is used to fill up the orig params for LTE data call.

PARAMETERS
  pdn_cntx_p       - Pointer to the PDN context
  mm_orig_params_p - Pointer to the orig params structure
  call_info        - Pointer to the call info structure

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_eps_pdn_get_orig_params
(
  ds_pdn_mgr_s                    *pdn_mgr_p,
  emm_pdn_connectivity_req_s_type *lte_orig_params
)
{
  uint16                  sdf_id = 0x0;
  pdn_connection_data_T  *pdn_data = NULL;
  ds_profile_type_s      *profile_p = NULL;
  ds_pdn_protocol_cfg_options_s   ds_proto_config_option = {0};
  boolean                          result = FALSE;
  boolean                          auth_info_being_sent = FALSE;
  uint16                           loop = 0;
  uint8                            apn_encoded_length = 0;
  ds_pdn_protocol_id_s           *temp_proto_info = NULL;
  uint8                             ntw_overridden_ip_bmask = 
                                        DS_3GPP_PDP_UNDEFINED_BMASK;
  sys_plmn_id_s_type               current_plmn = {0};
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Generate the SDF ID. Fill up the SDF ID and the request type.
  -------------------------------------------------------------------------*/
  sdf_id |= (uint16)((pdn_mgr_p->index) << 8);

  lte_orig_params->esm_hdr.sdf_id = sdf_id;
  lte_orig_params->request_type = ESM_INITIAL_REQUEST;

  /*-------------------------------------------------------------------------
    Fill up the pdn_connection_data_T
  -------------------------------------------------------------------------*/
  pdn_data = &(lte_orig_params->pdn_data);
  profile_p = pdn_mgr_p->pdp_profile;

  ds_pdn_hdlr_get_current_plmn(&current_plmn);
  
  if ((ntw_overridden_ip_bmask = 
            ds_3gpp_network_override_hdlr_get_overridden_ip(
      (byte *)profile_p->context.apn,DS_3GPP_THROT_RAT_LTE, &current_plmn)) 
      != DS_3GPP_PDP_UNDEFINED_BMASK)
  {
    DS_MSG_SPRINTF_2(MSG_LEGACY_HIGH,"IP is nw overriden for apn:%s,"
                      "network_overridden_ip_type:%d",profile_p->context.apn,
                      ntw_overridden_ip_bmask);

    result = ds_eps_pdn_cntxt_update_ntw_overridden_eps_orig_params
               (pdn_data,pdn_mgr_p,ntw_overridden_ip_bmask);
    if (!result)
    {
      return FALSE;
    }
  }
  else
  {
    if(profile_p->context.pdp_type == DS_PROFILE_PDP_IPV4)
    {
      pdn_data->pdn_type = NAS_ESM_IPV4;
    }
    else if(profile_p->context.pdp_type == DS_PROFILE_PDP_IPV6)
    {
      pdn_data->pdn_type = NAS_ESM_IPV6;
    }
    else if (profile_p->context.pdp_type == DS_PROFILE_PDP_IPV4V6)
    {
      pdn_data->pdn_type = NAS_ESM_IPV4V6;
    }
    else if (profile_p->context.pdp_type == DS_PROFILE_PDP_NON_IP)
    {
      pdn_data->pdn_type = NAS_ESM_NONIP;
    }
    else
    {
      return FALSE;
    }
  }
  
  /*-------------------------------------------------------------------------
     Fill in the PCO
   -------------------------------------------------------------------------*/
  /*-------------------------------------------------------------------------
    Config protocol field is 0x80 : ref: TS24.008 sec 10.5.6.3
    MSB should be 1 (ext field) protocol field b0-b2 should be 0x0
    Initilize this field
  -------------------------------------------------------------------------*/
  ds_proto_config_option.config_protocol   = PROT_CFG_IE_OCT3_VAL;

  if (NAS_ESM_NONIP == pdn_data->pdn_type)
  {
    /*-------------------------------------------------------------------------
      Fill Non-IP Link MTU Request if profile pdp_type is Non-IP
    --------------------------------------------------------------------------*/
    ds_pdn_call_hdlr_fill_non_ip_link_mtu_container(&ds_proto_config_option);
  }
  else
  {
   	result = ds_eps_pdn_ip_fill_ppp_params(
                      profile_p,
                      &pdn_mgr_p->info,
                      &auth_info_being_sent,
                      &ds_proto_config_option);

    if (result == FALSE)
    {
      DS_ERROR_LOG_0("Error in filling PPP parameters");
    }

    // Operarator PCO from app supported ?? ds_pdn_cntxt_fill_oper_pco_info_from_app
  
    (void)ds_pdn_mgr_fill_oper_pco_info_from_profile(
                            &ds_proto_config_option, profile_p);
  
#ifdef FEATURE_DATA_IMS
    /*-------------------------------------------------------------------------
      Get the request_pcscf_address_flag from profile registry to see if pcscf
      address should be requested for this profile.
    -------------------------------------------------------------------------*/
    /* Create a pcscf address request */
    if( ds_pdn_mgr_gen_pcscf_address_request_packet(&ds_proto_config_option,
                                                    profile_p) == FALSE )
    {
      /*-------------------------------------------------------------------
        If an error occurs in encoding pkt, do not include the pkt but
        proceed with call If we return error here, call setup would fail and
         the user would'nt know why?  and all subsequent call attemtps for the
         PDP ctxt would also fail (using the same params)
      ------------------------------------------------------------------*/
      DS_ERROR_LOG_0("PCSCF Address request packet generation failed");
    }

    /*-------------------------------------------------------------------------
      Get the im_cn_flag from call table to see if im cn flag should be
      requested for this call.
    -------------------------------------------------------------------------*/
    if(profile_p->im_cn_flag == TRUE)
    {
      if( ds_bearer_mgr_is_IMCN_call_up() == TRUE)
      {
        DS_ERROR_LOG_0("Call with IM CN Flag already up");
        goto memfree;
      }
      else
      {
        if(ds_pdn_mgr_gen_imcn_subsystem_request_packet(
            &ds_proto_config_option) == FALSE)
        {
          DS_ERROR_LOG_0("IM CN flag generate failed.");
        }
      }
    }
#endif //FEATURE_DATA_IMS
  
  /*-------------------------------------------------------------------------
      Determine IP address allocation setting from the profile and fill the
      PCO accordingly
    -------------------------------------------------------------------------*/
    ds_pdn_mgr_fill_address_alloc_param(
                                 &ds_proto_config_option,
                                 profile_p);

    /*-------------------------------------------------------------------------
      Fill IPV4 Link MTU Request if profile pdp_type is IPV4 or IPV4V6
    --------------------------------------------------------------------------*/
    ds_pdn_mgr_fill_ipv4v6_link_mtu_container(&ds_proto_config_option);
  
    /*-------------------------------------------------------------------------
      Fill local address support in TFT container
    --------------------------------------------------------------------------*/    
    if( ds_pdn_nv_manager_is_rel_11_or_above_supported())
    {        
      ds_pdn_mgr_fill_ipv4v6_local_address_support_container(&ds_proto_config_option);            
    }
  }
  
  /*-------------------------------------------------------------------------
    Fill in the APN Name
 -------------------------------------------------------------------------*/
  pdn_data->apn_name.valid = FALSE;
  memset(pdn_data->apn_name.address, 0, MAX_APN_ADR_LEN);

  /*-------------------------------------------------------------------------
    Start with a default APN
 -------------------------------------------------------------------------*/
  pdn_data->apn_name.apn_addr_len  = 0;

  apn_encoded_length = ds_pdn_mgr_store_encoded_apn(
    (byte*)pdn_data->apn_name.address, (byte*)profile_p->context.apn);

  DS_MSG1(MSG_LEGACY_HIGH, "APN Name:length=%d", apn_encoded_length);

  if(apn_encoded_length > 1 )
  {
    pdn_data->apn_name.valid = TRUE;
    /*-----------------------------------------------------------------------
        Fill the total length of the encoded APN
        -----------------------------------------------------------------------*/
    pdn_data->apn_name.apn_addr_len = apn_encoded_length;
  }

  /*-------------------------------------------------------------------------
    Initialize the ESM transfer flag and PCO ciphering flag to be FALSE
  -------------------------------------------------------------------------*/
  pdn_data->esm_info_transfer_flag = FALSE;
  pdn_data->pco_ciphering_flag = FALSE;

  /*-------------------------------------------------------------------------
    Fill in the esm_information. This is to be set only if we are sending APN
    name
  -------------------------------------------------------------------------*/
  if( (DS_EPS_ATTACHED != ds_eps_pdn_attach_sm.attach_state) && 
      (apn_encoded_length > 1 || auth_info_being_sent))
  {
    pdn_data->esm_info_transfer_flag = TRUE;

   /*----------------------------------------------------------------------
      Mark ciphering is needed if PAP is to be sent
     -----------------------------------------------------------------------*/
    if( auth_info_being_sent == TRUE)
    {
      pdn_data->pco_ciphering_flag = TRUE;
    }
  }

  /*-------------------------------------------------------------------------
    Fill in APN Rate Control information   
  -------------------------------------------------------------------------*/    
  if (TRUE == ds_pdn_nv_manager_get_apn_rate_control_config())
  {
    ds_pdn_mgr_fill_pco_container_info(&ds_proto_config_option,
                                       APN_RATE_CONTROL_CONTAINER_ID,
                                       &temp_proto_info);   
  }

  if (TRUE == ds_pdn_nv_manager_get_additional_apn_rate_control_config())
  {
    ds_pdn_mgr_fill_pco_container_info(&ds_proto_config_option,
                                       ADDITIONAL_APN_RATE_CONTROL_CONTAINER_ID,
                                       &temp_proto_info);   
  }
  if(profile_p->msisdn_flag == TRUE)
  {
    /*-------------------------------------------------------------------------
      Fill in MSISDN information   
    -------------------------------------------------------------------------*/
    ds_pdn_mgr_fill_pco_container_info(&ds_proto_config_option,
                                       MSIDSN_CONTAINER_ID,
                                       &temp_proto_info); 
  }

  /*-------------------------------------------------------------------------
    Convert the DS PCO/EPCO to NAS format 
  --------------------------------------------------------------------------*/  
  if ( ds_pdn_pco_epco_hdlr_is_epco_enabled())
  {
    pdn_data->ext_protocol_conf_opt.valid = 
      ds_pdn_pco_epco_hdlr_convert_ds_epco_to_nas
      (&ds_proto_config_option,&(pdn_data->ext_protocol_conf_opt));
  }
  else
  {
    pdn_data->protocol_config_options.valid = 
      ds_pdn_pco_epco_hdlr_convert_ds_pco_to_nas
        (&ds_proto_config_option,&(pdn_data->protocol_config_options));

  }

  return TRUE;
    
memfree:
  for ( loop = 0; loop < ds_proto_config_option.num_protocol_ids; loop ++)
  {
    DS_SYSTEM_HEAP_MEM_FREE( ds_proto_config_option.protocol_info_p[loop]);
    ds_proto_config_option.protocol_info_p[loop] = NULL;   
  }
  
  return FALSE;
} /* ds_eps_pdn_cntxt_get_orig_params */

/*===========================================================================
FUNCTION DS_EPS_INITIATE_PDN_CONNECTION

DESCRIPTION
  This function originates a PDN connection. This is applicable only to the
  LTE mode of operation.

PARAMETERS
  user_info_ptr - currently unused. Will be needed if user_end_hdlr is to be
                         invoked
  seq_num       - Seq_num to be sent out
  
DEPENDENCIES
  None

RETURN VALUE  
  DS3G_SUCCESS - call was originated with CM.  
  DS3G_FAILURE - call could not be originated.

SIDE EFFECTS
  None
===========================================================================*/
boolean ds_eps_initiate_pdn_connection
(
  ds_pdn_mgr_call_info_type   *call_info  /* call infor params */
)
{
  emm_pdn_connectivity_req_s_type *lte_orig_params = NULL; /* Pointer to orig params*/
  emm_restore_bearer_ctxt_req_s_type 
                              *restore_bearer_req_ptr = NULL;
  uint8                        psm_index = 0;
  ds_bearer_mgr_s             *bearer_mgr_p = NULL;
  ds_pdn_mgr_s                *pdn_mgr_p = NULL;
  boolean                      result = TRUE; 
#ifdef FEATURE_NBIOT_NTN 
  boolean                      is_ntn_ind = ds_pdn_mgr_is_ntn_restoration_in_progress();
#endif /* FEATURE_NBIOT_NTN */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - - - */

  DS_SYSTEM_HEAP_MEM_ALLOC(lte_orig_params, sizeof(emm_pdn_connectivity_req_s_type),
                            emm_pdn_connectivity_req_s_type*);
  if(lte_orig_params == NULL)
  {
    return FALSE;
  }

  DS_SYSTEM_HEAP_MEM_ALLOC(restore_bearer_req_ptr,
  	                       sizeof(emm_restore_bearer_ctxt_req_s_type),
                           emm_restore_bearer_ctxt_req_s_type*);
  if(restore_bearer_req_ptr == NULL)
  {
    DS_SYSTEM_HEAP_MEM_FREE(lte_orig_params);
    return FALSE;
  }

  /* Memset the orig params structure to 0 */
  memset(lte_orig_params,0,sizeof(emm_pdn_connectivity_req_s_type));
  memset(restore_bearer_req_ptr,0,
         sizeof(emm_restore_bearer_ctxt_req_s_type));

  lte_orig_params->pdn_data.seq_num = ds_eps_attach_sm_get_seq_num();
  lte_orig_params->esm_dummy_flag = call_info->attach_wo_pdn;
  lte_orig_params->pdn_data.low_priority_signaling = 
                                           call_info->low_priority_signaling;

  DS_MSG1(MSG_LEGACY_HIGH,"Initiate PDN connectivity request with seq %d",
  	       lte_orig_params->pdn_data.seq_num);

  if( FALSE == call_info->attach_wo_pdn)
  {
    bearer_mgr_p = ds_bearer_mgr_get_instance_ptr(call_info->call_instance);
    pdn_mgr_p = bearer_mgr_p->pdn_mgr_ptr;
    
    if(bearer_mgr_p->is_default)
    {    
      result = ds_eps_pdn_get_orig_params(pdn_mgr_p, lte_orig_params);
    }
  
    if(result)
    {
      ds_eps_bearer_get_orig_params(bearer_mgr_p, lte_orig_params);
    }
  }

  if(result == TRUE)
  {
    // PSM handling
    psm_index = ds_pdn_psm_info_find_current_index();

    if( psm_index != DS_3GPP_MAX_BEARER_CONTEXT)
    {
      if((ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p) && 
	      ds_pdn_psm_fill_bearer_restore_bearer_ctxt_req(restore_bearer_req_ptr)))
      {
        restore_bearer_req_ptr->message_header.message_id = EMM_RESTORE_BEARER_CTXT_REQ;
        restore_bearer_req_ptr->message_header.message_set = MS_MM_DS;	
        restore_bearer_req_ptr->message_header.message_len_lsb =
          (sizeof( emm_restore_bearer_ctxt_req_s_type ) - sizeof(IMH_T)) % 256;
        restore_bearer_req_ptr->message_header.message_len_msb =
          (sizeof( emm_restore_bearer_ctxt_req_s_type ) - sizeof(IMH_T)) / 256; 
        /* Set the connection ID as invalid */
        lte_orig_params->esm_hdr.connection_id = ESM_INVALID_CON_ID;

        restore_bearer_req_ptr->sdf_id = lte_orig_params->esm_hdr.sdf_id;
        bearer_mgr_p->bearer_id = restore_bearer_req_ptr->eps_bearer_id;
        #ifdef FEATURE_NBIOT_NTN
        restore_bearer_req_ptr->ntn_ctxt_restore = is_ntn_ind;
        #endif /* FEATURE_NBIOT_NTN */
        ds_pdn_mgr_ds_to_nas_send_msg((void*)restore_bearer_req_ptr);	  

         
        DS_MSG1(MSG_LEGACY_HIGH,"PSM Syncup with SDF_ID %d",
                       restore_bearer_req_ptr->sdf_id);
      }
    }
    else
    {
      lte_orig_params->message_header.message_id = EMM_PDN_CONNECTIVITY_REQ;
      lte_orig_params->message_header.message_set = MS_MM_DS;      
      lte_orig_params->message_header.message_len_lsb =
        (sizeof( emm_pdn_connectivity_req_s_type ) - sizeof(IMH_T)) % 256;
      lte_orig_params->message_header.message_len_msb =
        (sizeof( emm_pdn_connectivity_req_s_type ) - sizeof(IMH_T)) / 256;
      
      /* Set the connection ID as invalid */
      lte_orig_params->esm_hdr.connection_id = ESM_INVALID_CON_ID;
    
      DS_MSG1(MSG_LEGACY_HIGH,"Call origination with SDF_ID %d", 
                              lte_orig_params->esm_hdr.sdf_id);   
      ds_pdn_mgr_ds_to_nas_send_msg((void*)lte_orig_params);    
    }
  }

  DS_SYSTEM_HEAP_MEM_FREE(lte_orig_params);
  DS_SYSTEM_HEAP_MEM_FREE(restore_bearer_req_ptr);

  return result;
}

/*===========================================================================
FUNCTION DS_EPS_PDN_MGR_VALIDATE_APN_NAME_AND_DECODE

DESCRIPTION
  This function is used to validate resolved apn name return by network 
  according to LTE spec, also compared with apn name in the profile


PARAMETERS
  info_ptr      - call info ptr from NAS
  decoded_apn - to store decoded apn

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - if apn parameter is valid and accepted  
  FALSE - otherwise  

SIDE EFFECTS
  None.
===========================================================================*/ 
boolean ds_eps_pdn_mgr_validate_apn_name_and_decode
(
  emm_act_default_bearer_context_request_ind_s_type  *info_ptr,
  byte                                               *decoded_apn,
  boolean                                             disable_apn_matching
)
{
  ds_local_err_val_e_type local_err = DS_LOCAL_ERR_VAL_NONE;

  if (info_ptr->apn_name.valid != TRUE)
  {
    local_err = DS_LOCAL_ERR_VAL_0;
    info_ptr->esm_cause.esm_cause = NAS_ESM_INVALID_MANDATORY_INFORMATION;
    goto func_exit;
  }

  if(info_ptr->apn_name.apn_addr_len > DS_PROFILE_APN_STRING_LEN)
  {
    local_err = DS_LOCAL_ERR_VAL_1;
    info_ptr->esm_cause.esm_cause = NAS_ESM_UNKNOWN_MISSING_APN;
    goto func_exit;
  }

  /*---------------------------------------------------------------------------- 
    Try to decode apn name in a strict manner and if it fails and if
    disable apn matching NV is turned on try to decode the apn in a
    non strict manner
   ----------------------------------------------------------------------------*/
  if(ds_pdn_decode_apn_name(info_ptr->apn_name.address,
                          (uint32)info_ptr->apn_name.apn_addr_len,
                          decoded_apn) == FALSE)
  {
    if(!disable_apn_matching)
    {
      local_err = DS_LOCAL_ERR_VAL_2;
      info_ptr->esm_cause.esm_cause = NAS_ESM_INVALID_MANDATORY_INFORMATION;
    }
    else
    {
      if(ds_pdn_decode_apn_name_non_strict(info_ptr->apn_name.address,
                                         (uint32)info_ptr->apn_name.apn_addr_len,
                                         decoded_apn) == FALSE)
      {
        local_err = DS_LOCAL_ERR_VAL_3;
        info_ptr->esm_cause.esm_cause = NAS_ESM_INVALID_MANDATORY_INFORMATION;
      }
    }
  }

func_exit :
  if(local_err != DS_LOCAL_ERR_VAL_NONE)
  {
    DS_ERROR_LOG_1("Decoding of apn name failed with err =d", local_err);
    info_ptr->esm_cause.valid = TRUE;
    return FALSE;
  }

  return TRUE;
}/* ds_eps_pdn_mgr_validate_apn_name_and_decode */


/*===========================================================================
FUNCTION  DS_EPS_PDN_MGR_DEFAULT_BEARER_ACTIVATE_IND

DESCRIPTION
  This function accepts or rejects the the EPS default bearer activate request.

PARAMETERS
  info_ptr_i:  Default bearer activate information

DEPENDENCIES
  None.

RETURN VALUE
  Boolean - Bearer activation status.

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_eps_pdn_mgr_default_bearer_activate_ind
(
  emm_act_default_bearer_context_request_ind_s_type   *info_ptr
)
{
  ds_pdn_mgr_s                  *pdn_mgr_p = NULL;
  ds_bearer_mgr_s               *bearer_mgr_p = NULL;
  ds_profile_type_s             *profile_ptr = NULL;
  ds_pdn_mgr_inst_type_s        *v4_inst = NULL;
  ds_pdn_mgr_inst_type_s        *v6_inst = NULL;
  ds_pdn_mgr_inst_type_s        *non_ip_inst = NULL;
  ds_pdn_mgr_inst_type_s        *comp_inst = NULL;
  ds_apn_ip_support_type_e       ip_support = DS_IPV4_ONLY;
  ds_bearer_flow_manager_s      *bflow_manager_p = NULL;
  ds_pdn_flow_manager_s         *pdn_flow_manager_p = NULL;
  protocol_cfg_options_T        *protocol_config_options_p;
  char                           decoded_apn[DS_PROFILE_APN_STRING_LEN] = {0};
  uint8                          index = 0;
  uint8                          resolved_apn_index = 0;
  ds_profile_addr_type           ds_pdn_addr = {0};

  void                           *sm_ptr;  
  ds_net_down_reason_type        ds_down_reason = 
                                     DS_NET_DOWN_REASON_NOT_SPECIFIED;
  ds_pdn_apm_ind_pay_load        ind_payload = {0};
  boolean                        decode_apn_result = FALSE;
  uint32                         rat_mask = DS_3GPP_THROT_RAT_MASK_GLOBAL;
                                 /* RATs on which throttling needs to
                                                       be unblocked */
  ds_3gpp_throt_rat_e_type       rat_type =  DS_3GPP_THROT_RAT_GLOBAL;
                                 /* RAT on which throttling needs to
                                                       be performed */
  ds_3gpp_throttle_sm_cb_data_type   cb_data;
  ds_3gpp_pdn_throt_ident_info_type  throt_ident_info;
  boolean                         rel12_plus_support =  FALSE;
  ext_protocol_cfg_options_T     *ext_protocol_conf_opt_p = NULL;
  ds_pdn_protocol_cfg_options_s  ds_proto_config_options;
  boolean                         is_redial_allowed = FALSE;
  pdn_type_T                      nas_pdn_type;  
//  emm_act_bearer_rsp_s_type      *act_response = NULL;
  esm_cause_T                     temp_esm_cause = {0};
  esm_cause_enum_T                local_esm_cause = NAS_ESM_NO_FAILURE;
  lte_nas_local_cause_T           esm_local_cause = {0};
  ds_local_err_val_e_type         local_err = DS_LOCAL_ERR_VAL_NONE;
  emm_act_default_bearer_context_accept_T  accept_respone={{0}};
  emm_act_default_bearer_context_rej_T     reject_respone={{0}};
  ds_bearer_flow_internal_cause_code_enum_type icause_code = 
                                      DS_BEARER_FLOW_INTERNAL_CC_NONE;  
  reattempt_ind_T                 reattempt_indicator = {0};
  ds_apn_ip_support_type_e        dbg_ip_type = DS_IP_TYPE_INVALID;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( info_ptr == NULL )
  {
    return FALSE;
  }

  if(info_ptr->esm_hdr.lbi != ESM_INVALID_BEARER_ID)
  {
    local_err = DS_LOCAL_ERR_VAL_0;
    goto func_exit;
  }
  
  rel12_plus_support =  ds_pdn_nv_manager_is_rel_12_or_above_supported();
  if (ds_pdn_nv_manager_get_enable_pdn_throt_per_rat() ||
      rel12_plus_support)
  {
    rat_type = DS_3GPP_THROT_RAT_LTE;
    rat_mask |= DS_3GPP_THROT_RAT_MASK_LTE;
  }

  bearer_mgr_p = ds_bearer_mgr_get_bearer_from_sdf_id(info_ptr->esm_hdr.sdf_id);
  if(ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p))
  {
    pdn_mgr_p = bearer_mgr_p->pdn_mgr_ptr;
    if(!ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p))
    {
      bearer_mgr_p->state = DS_BEARER_MGR_STATE_GOING_DOWN;
      local_err = DS_LOCAL_ERR_VAL_1;
      goto func_exit;
    }

    /*--------------------------------------------------------------------------
      Get the throttling state macine and reset it. NULL checking need not be
      performed here as there is no dereferencing of the opaque handle in this
      function
      Call this function with argument FALSE meaning we need not create a sm
      if it does not already exists. Here we are interested in only retrieving
      the SM for reset
     --------------------------------------------------------------------------*/
    sm_ptr = ds_3gpp_srv_req_get_throt_sm(FALSE);
    ds_3gpp_srv_req_throt_reset(sm_ptr);

    /*------------------------------------------------------------------------ 
      Set apn_param_chg_teardown to FALSE
      ------------------------------------------------------------------------*/
    pdn_mgr_p->lte_params->apn_param_chg_teardown = FALSE;

    /*-------------------------------------------------------------------------
      get the pointer to the profile in the PDN context.
    -------------------------------------------------------------------------*/
    profile_ptr = pdn_mgr_p->pdp_profile;

    /*-------------------------------------------------------------------------
      Ensure that the PDN state is correct. If the PDN state is in GOING_DOWN,
      then we have to reject the bearer activation.
      This is a case of a race between the Abort PDN connectivity handling and
      the handling of Activate Bearer Ind. In such a case, we will reject the
      bearer activation and not send out an abort. As a result of rejecting
      bearer activation, we will get a CALL_END from CM during which we clean up
      our resources.
    -------------------------------------------------------------------------*/
    if(pdn_mgr_p->state == DS_PDN_MGR_STATE_GOING_DOWN)
    {
      bearer_mgr_p->state = DS_BEARER_MGR_STATE_GOING_DOWN;
      local_err = DS_LOCAL_ERR_VAL_2;
      goto func_exit;     
    }
    
    if(bearer_mgr_p->is_default != TRUE)
    {
      bearer_mgr_p->state = DS_BEARER_MGR_STATE_GOING_DOWN;
      local_err = DS_LOCAL_ERR_VAL_3;
      goto func_exit;           
    }
    else
    {
      pdn_flow_manager_p = ds_pdn_flow_mgr_get_entry(pdn_mgr_p->index);
      if(NULL != pdn_flow_manager_p)
      {
        pdn_flow_manager_p->bcm_info = DS_3GPP_BCM_UE_ONLY;
      }
    }
  } /* UE initiated call */
  else
  {    
    local_err = DS_LOCAL_ERR_VAL_4;
    goto func_exit;    
  }

  /*----------------------------------------------------------------------
    Compare and copy the PDN address
  ----------------------------------------------------------------------*/
  if (info_ptr->pdn_address.valid != TRUE)
  {
    local_esm_cause = NAS_ESM_INVALID_MANDATORY_INFORMATION;
    local_err = DS_LOCAL_ERR_VAL_5;
    goto func_exit;     
  }

  if(ds_apn_table_get_ip_support_info_for_pdn(pdn_mgr_p, &ip_support) != TRUE)
  {
    local_esm_cause = NAS_ESM_INVALID_MANDATORY_INFORMATION;
    local_err = DS_LOCAL_ERR_VAL_6;
    goto func_exit;     
  }

  nas_pdn_type = info_ptr->pdn_address.pdn_type_val;
  DS_MSG3(MSG_LEGACY_HIGH,
  	       "Profile pdp_type: %d, initializing ip_support to:%d, "
  	       "NAS PDN type %d",
          profile_ptr->context.pdp_type, ip_support, nas_pdn_type);

  /*----------------------------------------------------------------------
    Now compare the PDP type stored in the profile with the ESM cause that
    is returned in the Activate Default Bearer Context Request message.
    If there is any mismatch in the PDP type requested and returned, we
    have to reject the call.
    If we requested for Dual IP bearer and NW returns IPV4, or V6 or single
    IP bearer only, this is also handled here.
   ----------------------------------------------------------------------*/

  /*----------------------------------------------------------------------
    If we requested for Dual IP bearer and NW returns IPV4/V6 only
    without any cause then if redial allow NV is set ,ip support gets updated
    and  pdn_dissociate function will clean up companion instance (based on NV )
  ---------------------------------------------------------------------------*/
  if((ip_support == DS_IPV4V6) &&
     (nas_pdn_type != NAS_ESM_IPV4V6) &&
     (info_ptr->esm_cause.valid != TRUE))
  {
    is_redial_allowed = ds_pdn_nv_manager_get_redial_no_cause_code_from_nv();

    if(nas_pdn_type == NAS_ESM_IPV4)
    {
      if (is_redial_allowed)
      {
        ip_support = DS_IPV4_SINGLE_BEARER;
      }
      else
      {
        ip_support = DS_IPV4_ONLY;
      }
    }
    else if(nas_pdn_type == NAS_ESM_IPV6)
    {
      if (is_redial_allowed)
      {
        ip_support = DS_IPV6_SINGLE_BEARER;
      }  
      else
      {
        ip_support = DS_IPV6_ONLY;
      }
    }

    DS_MSG2(MSG_LEGACY_ERROR,
            "Received invalid cause code for v4v6, is redial allowed %d, "
            "IP type updated from IPV4V6 to %d ",
            is_redial_allowed, ip_support);
    
#ifdef FEATURE_DATA_CC_SP_THROTTLING
  if(ds_3gpp_throt_sm_is_cc_sp_throttling_enabled() != TRUE)
    {		
      ds_eps_pdn_cntxt_update_param_for_new_ip_type(ip_support,pdn_mgr_p,&ind_payload);
    }	
#endif /* FEATURE_DATA_CC_SP_THROTTLING */

    switch (ip_support)
    {
      case DS_IPV4_ONLY:
      case DS_IPV4_SINGLE_BEARER:
        comp_inst = pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV6];
        break;

      case DS_IPV6_ONLY:
      case DS_IPV6_SINGLE_BEARER:
        comp_inst = pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV4];
        break;

      default:
        break;
    }

    if (comp_inst)
    {
      ds_pdn_mgr_dissociate_instance(pdn_mgr_p, ip_support);
    }    
  }

  if(info_ptr->esm_cause.valid == TRUE)
  {
    DS_MSG1(MSG_LEGACY_HIGH, "Processing esm_cause: %d", 
            info_ptr->esm_cause.esm_cause);
    
    /*------------------------------------------------------------------------------- 
      To populate the ds_down_reason, This would be used if regular throttling
      is performed. 
      esm_local_cause is dummy variable passed to maintain the 
      compatibility for function call. It is set to 0, so no side effects.
    ---------------------------------------------------------------------------------*/
    ds_eps_bearer_mgr_map_esm_cause_codes_to_ds(&ds_down_reason, 
                                        info_ptr->esm_cause,
                                        esm_local_cause);

    /*-----------------------------------------------------------------------
      Populate the Callback data that needs to be passed when 
      PDN Throttle Start, Stop or Get Timer value callback functions are 
      invoked
    -----------------------------------------------------------------------*/
    ds_3gpp_throt_fill_cb_data (&cb_data, NULL, NULL, (void*)pdn_mgr_p);

    /*-----------------------------------------------------------------------
      Populate the throttle identifier info. This info will be used to 
      fetch the appropriate PDN Throttle SM Handle and then perform 
      PDN Throttling
    -----------------------------------------------------------------------*/
    ds_3gpp_pdn_throt_fill_ident_info_ex
    (
       &throt_ident_info, 
       (byte *)pdn_mgr_p->pdp_profile->context.apn,
       FALSE,
       rat_type,
       DS_3GPP_PDN_THROT_PROC_GLOBAL,
       DS_3GPP_PDN_THROT_TYPE_DEFAULT,
       DS_PROFILE_PDP_IPV4V6,
       FALSE,
       ds_down_reason,
       pdn_mgr_p,
       &cb_data
    );

    if((info_ptr->esm_cause.esm_cause != NAS_ESM_PDN_TYPE_IPV4_ONLY_ALLOWED)
       && (info_ptr->esm_cause.esm_cause != NAS_ESM_PDN_TYPE_IPV6_ONLY_ALLOWED)
       && (info_ptr->esm_cause.esm_cause != NAS_ESM_SINGLE_ADR_BEARERS_ONLY_ALLOWED)
       && (info_ptr->esm_cause.esm_cause != NAS_ESM_PDN_TYPE_IPv4v6_ONLY_ALLOWED)
       && (info_ptr->esm_cause.esm_cause != NAS_ESM_PDN_TYPE_NON_IP_ONLY_ALLOWED))
    {
      local_esm_cause = NAS_ESM_ACTIVATION_REJECTED;
      local_err = DS_LOCAL_ERR_VAL_5;
      goto func_exit;     
    }
    else switch(profile_ptr->context.pdp_type)
    {
      case DS_PROFILE_PDP_IPV4:
        if(info_ptr->esm_cause.esm_cause == NAS_ESM_PDN_TYPE_IPV6_ONLY_ALLOWED)
        {
          ip_support = DS_IPV6_ONLY;
          local_esm_cause = NAS_ESM_ACTIVATION_REJECTED;
          local_err = DS_LOCAL_ERR_VAL_7;
          ds_pdn_apm_set_invalid_ipv4_addr(
          	               &ind_payload, DS_NET_DOWN_REASON_IP_V6_ONLY_ALLOWED); 
       /*------------------------------------------------------------------------------------ 
            For REL12 plus support set network override handler to the cause code received
            Cause_code             Overrride_hdlr_IP_set(IP allowed)
            IPV4 only                  IPV4
            IPV6 only                  IPV6
            ------------------------------------------------------------------------------------*/
          if (rel12_plus_support)
          {
            ds_3gpp_network_override_hdlr_set_overridden_ip(
                (byte *)profile_ptr->context.apn,
                DS_3GPP_PDN_THROT_PDP_IPV6,
                DS_3GPP_PDN_THROT_BEARER_ACT_ACCEPT,
                DS_3GPP_THROT_RAT_MASK_LTE,
                reattempt_indicator);
          }
        }
        else if(info_ptr->esm_cause.esm_cause == 
        	                      NAS_ESM_SINGLE_ADR_BEARERS_ONLY_ALLOWED)
        {
          if(nas_pdn_type == NAS_ESM_IPV4)
          {
            ip_support = DS_IPV4_SINGLE_BEARER;
          }
          else if (nas_pdn_type == NAS_ESM_IPV6)
          {
            ip_support = DS_IPV6_SINGLE_BEARER;
            local_esm_cause = NAS_ESM_ACTIVATION_REJECTED;
            local_err = DS_LOCAL_ERR_VAL_8;
            ds_pdn_apm_set_invalid_ipv4_addr(
            	       &ind_payload, DS_NET_DOWN_REASON_SINGLE_ADDR_BEARER_ONLY); 
          }
          else
          {
            local_err = DS_LOCAL_ERR_VAL_9;
            local_esm_cause = NAS_ESM_UNKNOWN_PDN_TYPE;
          }
        }
        break;
        
      case DS_PROFILE_PDP_IPV6:
        if(info_ptr->esm_cause.esm_cause == NAS_ESM_PDN_TYPE_IPV4_ONLY_ALLOWED)
        {
          ip_support = DS_IPV4_ONLY;
          local_esm_cause = NAS_ESM_ACTIVATION_REJECTED;
          local_err = DS_LOCAL_ERR_VAL_10;
          ds_pdn_apm_set_invalid_ipv6_addr(
          	           &ind_payload, DS_NET_DOWN_REASON_IP_V4_ONLY_ALLOWED); 

          ds_3gpp_network_override_hdlr_set_overridden_ip(
               (byte *)pdn_mgr_p->pdp_profile->context.apn,
               DS_3GPP_PDN_THROT_PDP_IPV4,
               DS_3GPP_PDN_THROT_BEARER_ACT_ACCEPT, 
               DS_3GPP_THROT_RAT_MASK_LTE,
               reattempt_indicator);

        }
        else if(info_ptr->esm_cause.esm_cause == 
                        NAS_ESM_SINGLE_ADR_BEARERS_ONLY_ALLOWED)
        {
          if(nas_pdn_type == NAS_ESM_IPV4)
          {
            ip_support = DS_IPV4_SINGLE_BEARER;
            local_esm_cause = NAS_ESM_ACTIVATION_REJECTED;
            local_err = DS_LOCAL_ERR_VAL_11;            
            ds_pdn_apm_set_invalid_ipv6_addr(
            	     &ind_payload, DS_NET_DOWN_REASON_SINGLE_ADDR_BEARER_ONLY); 
          }
          else if(nas_pdn_type == NAS_ESM_IPV6)
          {
            ip_support = DS_IPV6_SINGLE_BEARER;
          }
          else
          {
            local_err = DS_LOCAL_ERR_VAL_12;
            local_esm_cause = NAS_ESM_UNKNOWN_PDN_TYPE;
          }
        }
        break;
        
      case DS_PROFILE_PDP_IPV4V6:
        if(info_ptr->esm_cause.esm_cause == NAS_ESM_PDN_TYPE_IPV4_ONLY_ALLOWED)
        {
          DS_MSG0(MSG_LEGACY_HIGH,
          	       "Received ESM cause IPV4 only while profile req is V4V6");
          ip_support = DS_IPV4_ONLY;
          comp_inst = pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV6];
          /*---------------------------------------------------------------------- 
                 Treat this as if the network failed a V6 connection request.
                -----------------------------------------------------------------------*/
#ifdef FEATURE_DATA_CC_SP_THROTTLING
          if(ds_3gpp_throt_sm_is_cc_sp_throttling_enabled() != TRUE)
          {
#endif /* FEATURE_DATA_CC_SP_THROTTLING */
             throt_ident_info.pdp_type = DS_PROFILE_PDP_IPV6;
             ds_3gpp_pdn_throt_perform_reg_throttling(&throt_ident_info);
#ifdef FEATURE_DATA_CC_SP_THROTTLING
          }
#endif /* FEATURE_DATA_CC_SP_THROTTLING */

          ds_pdn_apm_set_invalid_ipv6_addr(
                      &ind_payload, DS_NET_DOWN_REASON_IP_V4_ONLY_ALLOWED);

          ds_3gpp_network_override_hdlr_set_overridden_ip(
               (byte *)pdn_mgr_p->pdp_profile->context.apn,
               DS_3GPP_PDN_THROT_PDP_IPV4,
               DS_3GPP_PDN_THROT_BEARER_ACT_ACCEPT,
               DS_3GPP_THROT_RAT_MASK_LTE,
               reattempt_indicator);
        }
        else if(info_ptr->esm_cause.esm_cause == 
        	                             NAS_ESM_PDN_TYPE_IPV6_ONLY_ALLOWED)
        {
          DS_MSG0(MSG_LEGACY_HIGH,
          	       "Received ESM cause IPV6 only while profile req is V4V6");
          
          if(ds_pdnmgr_is_prof_clat_enabled(pdn_mgr_p->pdp_profile_num))
          {
            pdn_mgr_p->cache_v4_inst_down = TRUE;
            ip_support = DS_IPV6_ONLY;
            break;
          }
            
          ip_support = DS_IPV6_ONLY;
          comp_inst = pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV4];
          /*----------------------------------------------------------------------
                 Treat this as if the network failed a V4 connection request.
                -----------------------------------------------------------------------*/
#ifdef FEATURE_DATA_CC_SP_THROTTLING
          if(ds_3gpp_throt_sm_is_cc_sp_throttling_enabled() != TRUE)
          {
#endif /* FEATURE_DATA_CC_SP_THROTTLING */
            throt_ident_info.pdp_type = DS_PROFILE_PDP_IPV4;
            ds_3gpp_pdn_throt_perform_reg_throttling (&throt_ident_info);
#ifdef FEATURE_DATA_CC_SP_THROTTLING
          }
#endif /* FEATURE_DATA_CC_SP_THROTTLING */

          ds_pdn_apm_set_invalid_ipv4_addr(
                   &ind_payload, DS_NET_DOWN_REASON_IP_V6_ONLY_ALLOWED);
          ds_3gpp_network_override_hdlr_set_overridden_ip(
               (byte *)pdn_mgr_p->pdp_profile->context.apn,
               DS_3GPP_PDN_THROT_PDP_IPV6,
               DS_3GPP_PDN_THROT_BEARER_ACT_ACCEPT,
               DS_3GPP_THROT_RAT_MASK_LTE,
               reattempt_indicator);
        }
        else if(info_ptr->esm_cause.esm_cause == \
                          NAS_ESM_SINGLE_ADR_BEARERS_ONLY_ALLOWED)
        {
          DS_MSG0(MSG_LEGACY_HIGH,
          	       "Received ESM cause Single addr bearer only "
          	       "while profile req is V4V6");
          if(nas_pdn_type== NAS_ESM_IPV4)
          {
            ip_support = DS_IPV4_SINGLE_BEARER;
            ds_pdn_apm_set_invalid_ipv6_addr(
            	    &ind_payload, DS_NET_DOWN_REASON_SINGLE_ADDR_BEARER_ONLY);
            comp_inst = pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV6];
          }
          else if(nas_pdn_type == NAS_ESM_IPV6)
          {
            ip_support = DS_IPV6_SINGLE_BEARER;
            ds_pdn_apm_set_invalid_ipv4_addr(
            	    &ind_payload, DS_NET_DOWN_REASON_SINGLE_ADDR_BEARER_ONLY);
            comp_inst = pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV4];
          }
          else
          {
            local_err = DS_LOCAL_ERR_VAL_13;
            local_esm_cause = NAS_ESM_UNKNOWN_PDN_TYPE;
            break;
          }
        }
        
        if (comp_inst)
        {
          ds_pdn_mgr_dissociate_instance(pdn_mgr_p, ip_support);
        }
        break;

      case DS_PROFILE_PDP_NON_IP:
        if(nas_pdn_type != NAS_ESM_NONIP)
        {
          local_err = DS_LOCAL_ERR_VAL_14;
          local_esm_cause = NAS_ESM_ACTIVATION_REJECTED;
        }
        break;
        
      default:
        local_err = DS_LOCAL_ERR_VAL_15;
        local_esm_cause = NAS_ESM_UNKNOWN_PDN_TYPE;
    }
  }

  if(local_err != DS_LOCAL_ERR_VAL_NONE)
  {
    goto func_exit;
  }

  if ( pdn_mgr_p->is_default &&
       ind_payload.ind_field_mask != 0 )
  {           
    ds_pdn_apm_set_profile_id(&ind_payload, pdn_mgr_p->pdp_profile_num);  
    (void)ds_pdn_apm_ind(DS_PDN_APM_IP_ADDRESS_IND, &ind_payload);
  }

  if( (ds_eps_pdn_attach_sm.attach_state == DS_EPS_ATTACHED) &&
      (pdn_mgr_p->is_default == FALSE) )
  {
    /*--------------------------------------------------------------------
      Now that we have determined the IP support returned by the network,
      we need to make sure that this is something that we need. If n/w
      returns an IP type that is not the one that the app requested for
      then we have to reject bearer activation and request a new PDN
      connection with the app requested IP type only.
    --------------------------------------------------------------------*/
    v4_inst = pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV4];
    v6_inst = pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV6];

    switch(ip_support)
    {
      case DS_IPV4_ONLY:
      case DS_IPV4_SINGLE_BEARER:
        if(v4_inst->in_use != TRUE)
        {
          /*------------------------------------------------------------------
            Reject bearer activation since IP V6 pipe cannot be obtained
            If a V6 pipe were needed, the request would have been made while
            dissociating. It is safe to reject this bearer activation
          ------------------------------------------------------------------*/
          local_err = DS_LOCAL_ERR_VAL_16;
          local_esm_cause = NAS_ESM_ACTIVATION_REJECTED;
          goto func_exit;
        }
        break;

      case DS_IPV6_ONLY:
      case DS_IPV6_SINGLE_BEARER:
        if(pdn_mgr_p->cache_v4_inst_down == TRUE)
        {
          /* Case to handle v4 only call for v4v6 PDP type for additional 
             PDN. Accept the bearer activation even though in_use for v6 
             is false as it wont be set during the PDN bringup*/
           v6_inst->in_use = TRUE;
        }
        else if(v6_inst->in_use != TRUE)
        {
          /*------------------------------------------------------------------
            Reject bearer activation since IP V4 pipe cannot be obtained
            If a V4 pipe were needed, the request would have been made while
            dissociating. It is safe to reject this bearer activation
          ------------------------------------------------------------------*/
          local_err = DS_LOCAL_ERR_VAL_17;
          local_esm_cause = NAS_ESM_ACTIVATION_REJECTED;
          goto func_exit;
        }
        break;

      default:
        break;
    }
  } // Only done for additional PDN


  /*----------------------------------------------------------------------
    Verify the resolved apn name is valid according to LTE specification and 
    decode the apn name network identifier and store
  ----------------------------------------------------------------------*/
  if ((decode_apn_result = ds_eps_pdn_mgr_validate_apn_name_and_decode(
               info_ptr, (byte*)decoded_apn,
               ds_pdn_nv_manager_get_disable_apn_matching_from_nv())) == FALSE) 
  {
    local_err = DS_LOCAL_ERR_VAL_18;
    goto func_exit;
  }

  DS_MSG_SPRINTF_2(MSG_LEGACY_HIGH,
                   "APN Name in Prof is :%s , Resolved APN name: %s", 
                   profile_ptr->context.apn, decoded_apn);

  /*--------------------------------------------------------------------------
    Store the resolved APN name in the APN table.
    Ensure that we decode the source string so that we break down the FQDN
    to the labels of interest.
    Copy this formatted string into the APN table
  --------------------------------------------------------------------------*/
  if(!ds_apn_table_update_resolved_apn_name(pdn_mgr_p,
                                            decoded_apn))
  {
    DS_ERROR_LOG_0("Error updating resolved name in APN table");
  }

  if(ds_3gpp_pdn_limit_is_enabled())
  {
    index = ds_3gpp_pdn_limit_tbl_find_entry((byte *)profile_ptr->context.apn);
  }

  if(strlen(profile_ptr->context.apn) == 0)
  {    
     /*-------------------------------------------------------------------------
       Check whether there is already an entry in PDN Limit TBL 
       corresponding to resolved APN Name.
       If there is we just increment PDN Connection Cntr 
       corresponding to that entry. 
       If there is no such entry then we update NULL APN to Resolved APN Name
       and update PDN Connection Cntr
      -------------------------------------------------------------------------*/
      
    if(ds_3gpp_pdn_limit_is_enabled() &&
       ds_eps_pdn_attach_sm_get_state() == DS_EPS_ATTACHED)
      {
        if ((resolved_apn_index = 
             ds_3gpp_pdn_limit_tbl_find_entry((byte *)decoded_apn)) !=
            DS_3GPP_PDN_LIMIT_TBL_INVALID_ENTRY)
        {
          index = resolved_apn_index;
        }
        else
        {
          ds_3gpp_pdn_limit_tbl_update_apn_name(index, (char*)decoded_apn);
        }
      }
    }//if the APN name in PDN profile cache is NULL

  /*--------------------------------------------------------------------- 
    Increment the PDN Connection Counter as the call came up successfully.
    But don't take consideration if it's emergency call
  -----------------------------------------------------------------------*/

  if(ds_3gpp_pdn_limit_is_enabled() &&
     ds_eps_pdn_attach_sm_get_state() == DS_EPS_ATTACHED &&
     bearer_mgr_p->is_default == TRUE)
  {
    if(index != DS_3GPP_PDN_LIMIT_TBL_INVALID_ENTRY)
    {
      ds_3gpp_pdn_limit_increment_pdn_conn_cntr(index);
    }
  }

  /*---------------------------------------------------------------------
    Make sure that the ip support requested was IPV4. It could be IPV4
    only or Dual IP bearer. In case of a dual IP bearer request, we will
    enter here if the network returns a single IP bearer.
  ---------------------------------------------------------------------*/
  if(nas_pdn_type == NAS_ESM_IPV4 &&
     ip_support != DS_IPV4_ONLY &&
     ip_support != DS_IPV4_SINGLE_BEARER)
  {
    local_err = DS_LOCAL_ERR_VAL_20;
    goto func_exit;
  }

  /*---------------------------------------------------------------------
    Make sure that the ip support requested was IPV6. It could be IPV6
    only or Dual IP bearer. In case of a dual IP bearer request, we will
    enter here if the network returns a single IP bearer.
  ---------------------------------------------------------------------*/
  if(nas_pdn_type == NAS_ESM_IPV6 &&
     (ip_support != DS_IPV6_ONLY &&
     ip_support != DS_IPV6_SINGLE_BEARER && 
     ds_pdnmgr_is_prof_clat_enabled(pdn_mgr_p->pdp_profile_num) == FALSE)) 
  {
    local_err = DS_LOCAL_ERR_VAL_21;
    goto func_exit;
  }
  
  /*----------------------------------------------------------------------
    In the event which UE profile req V4 or V6 but NW returns both V4 V6
    address, we will bring up as per the profile req
  ----------------------------------------------------------------------*/
  if(nas_pdn_type == NAS_ESM_IPV4V6)
  {
    if(ip_support != DS_IPV4V6)
    {
      DS_ERROR_LOG_0("Wrong profile config");
  
      if(ip_support == DS_IPV4_ONLY)
      {
        ds_pdn_addr.ip_vsn = DS_PROFILE_IP_V4;
      }
      else if(ip_support == DS_IPV6_ONLY)
      {
        ds_pdn_addr.ip_vsn = DS_PROFILE_IP_V6;
      }
      else
      {
        local_err = DS_LOCAL_ERR_VAL_22;
        goto func_exit;
      }
    }
    else
    {
      ds_pdn_addr.ip_vsn = DS_PROFILE_IP_V4V6;
    }
  }
  else if(nas_pdn_type == NAS_ESM_IPV4)
  {
    ds_pdn_addr.ip_vsn = DS_PROFILE_IP_V4;    
  }
  else if(nas_pdn_type == NAS_ESM_IPV6)
  {
    if(ds_pdnmgr_is_prof_clat_enabled(pdn_mgr_p->pdp_profile_num))
    {
      ds_pdn_addr.ip_vsn = DS_PROFILE_IP_V4V6;
      pdn_mgr_p->cache_v4_inst_down = TRUE;
      DS_MSG0(MSG_LEGACY_HIGH, "CLAT enabled so Setting the ip vsn to v4v6 .");
    }
    else
    {
      ds_pdn_addr.ip_vsn = DS_PROFILE_IP_V6;
    }
  }
  else if(nas_pdn_type == NAS_ESM_NONIP)
  {
    ds_pdn_addr.ip_vsn = DS_PROFILE_NONIP;    
  }

  if( ds_pdn_addr.ip_vsn == DS_PROFILE_IP_V4 ||
  	   ds_pdn_addr.ip_vsn == DS_PROFILE_IP_V4V6 )
  {
    v4_inst = pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV4];

    if(nas_pdn_type == NAS_ESM_IPV4)
    {
      memscpy(&ds_pdn_addr.pdp_addr.pdp_addr_ipv4,
	             sizeof(uint32),
              info_ptr->pdn_address.address,
              sizeof(uint32));
    }
    else if(nas_pdn_type == NAS_ESM_IPV4V6)
    {
     /*---------------------------------------------------------------------
         Now store the V4 Address in the profile. The IPV4 address is present 
         in the octets 8-11 of the field "address" for dual IP PDN. Ref 9.9.4.9
        ---------------------------------------------------------------------*/
      memscpy(&ds_pdn_addr.pdp_addr.pdp_addr_ipv4, 
              sizeof(uint32),
              &(info_ptr->pdn_address.address[DS_3GPP_IPV4V6_V4_OFFSET]),
              sizeof(uint32));
    }

    if((ds_pdn_addr.pdp_addr.pdp_addr_ipv4 == 0) && !pdn_mgr_p->cache_v4_inst_down)
    {
      DS_ERROR_LOG_0("NULL V4 IP address provided by NW!");

      /* For the default PDN; inform APM if no V4 address is obtained. */
      if (pdn_mgr_p->is_default)
      {
        memset(&ind_payload, 0, sizeof(ds_pdn_apm_ind_pay_load));
        ds_pdn_apm_set_invalid_ipv4_addr(
        	               &ind_payload, DS_NET_DOWN_REASON_NOT_SPECIFIED);   
        ds_pdn_apm_set_profile_id(
        	               &ind_payload, pdn_mgr_p->pdp_profile_num);  
        (void)ds_pdn_apm_ind(DS_PDN_APM_IP_ADDRESS_IND, &ind_payload);
      }
                                        
      local_err = DS_LOCAL_ERR_VAL_23;
      local_esm_cause = NAS_ESM_INVALID_MANDATORY_INFORMATION;
      goto func_exit;
    }
    /*---------------------------------------------------------------------
      V4 address is valid, clearing throttle state for v4
    ----------------------------------------------------------------------*/
    ds_3gpp_pdn_throt_clear_throttle_state((byte *)pdn_mgr_p->pdp_profile->context.apn,
                                           DS_PROFILE_PDP_IPV4,
                                           rat_mask,
                                           DS_3GPP_THROT_PROC_MASK_GLOBAL);

  }

  if( ds_pdn_addr.ip_vsn == DS_PROFILE_IP_V6 ||
      ds_pdn_addr.ip_vsn == DS_PROFILE_IP_V4V6 )
  {
    v6_inst = pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV6];

    /*---------------------------------------------------------------------
      IID is the last 64 bits of the IP address. Hence it is copied into
      the lower 64 bits of the PDP addr field.
      The IPV6 IID is present in octets 0-7 of the field "address" inside
      the structure pdn_address_T
    ---------------------------------------------------------------------*/
    memscpy(&(ds_pdn_addr.pdp_addr.pdp_addr_ipv6.in6_u.\
    	                      u6_addr64[DS_3GPP_IPV6_IID_OFFSET]), 
    	       sizeof(uint64),
            info_ptr->pdn_address.address, 
            sizeof(uint64));

    if(ds_pdn_addr.pdp_addr.pdp_addr_ipv6.in6_u.\
                      u6_addr64[DS_3GPP_IPV6_IID_OFFSET] == 0)
    {
      DS_ERROR_LOG_0(" NULL V6 IID address provided by NW!");

      /* For the default PDN; inform APM if no V6 address is obtained. */
      if (pdn_mgr_p->is_default)
      {
        memset(&ind_payload, 0, sizeof(ds_pdn_apm_ind_pay_load));
        ds_pdn_apm_set_invalid_ipv6_addr(
        	              &ind_payload, DS_NET_DOWN_REASON_NOT_SPECIFIED); 
        ds_pdn_apm_set_profile_id(
        	              &ind_payload, pdn_mgr_p->pdp_profile_num);      
        (void)ds_pdn_apm_ind(DS_PDN_APM_IP_ADDRESS_IND, &ind_payload);
      }
      local_err = DS_LOCAL_ERR_VAL_24;
      local_esm_cause = NAS_ESM_INVALID_MANDATORY_INFORMATION;
      goto func_exit;
    }
  }

  if(nas_pdn_type == NAS_ESM_NONIP)
  {
    /*---------------------------------------------------------------------
      Make sure that the ip support requested was Non-IP.
    ---------------------------------------------------------------------*/
    if(ip_support != DS_NON_IP && ip_support != DS_NON_IP_SINGLE_BEARER)
    {
      local_err = DS_LOCAL_ERR_VAL_25;
      goto func_exit;
    }

    /*---------------------------------------------------------------------
      Determine the DS Iface from the PDN context
    ----------------------------------------------------------------------*/
    non_ip_inst = pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_NON_IP];
    ds_pdn_addr.ip_vsn = DS_PROFILE_NONIP;    
    memset(&ds_pdn_addr.pdp_addr,0,sizeof(ds_pdn_addr.pdp_addr));
  }

  /*-------------------------------------------------------------------------
    Update APN table with APN name and IP support
  -------------------------------------------------------------------------*/
  DS_MSG_SPRINTF_3(MSG_LEGACY_HIGH,
                   "Update APN table: ip_support:%d for pdn %d apn:%s",
                   ip_support,pdn_mgr_p->index,
                   profile_ptr->context.apn);

  if (ds_apn_table_update_apn_table(profile_ptr->context.apn,
                                    ip_support,
                                    (void*)pdn_mgr_p) != TRUE)
  {
    local_err = DS_LOCAL_ERR_VAL_27;
    local_esm_cause = NAS_ESM_INSUFFICIENT_RESOURCES;
    goto func_exit;    
  }

  /*----------------------------------------------------------------------
    Update pdp group id provided by NAS
  ----------------------------------------------------------------------*/
  pdn_mgr_p->pdp_group_id = info_ptr->group_id;

  /*----------------------------------------------------------------------
    If sdf qos info is passed in default bearer activation, store it in
    the bearer flow manager
  ----------------------------------------------------------------------*/
  if (info_ptr->sdf_qos.valid == TRUE)
  {
    qos_spec_type app_qos;

    DS_MSG0(MSG_LEGACY_HIGH, "Storing sdf qos info for default bearer.");
    bflow_manager_p= ds_bearer_flow_manager_find_entry(bearer_mgr_p);

    if(bflow_manager_p == NULL)
    {
      local_err = DS_LOCAL_ERR_VAL_27;
      local_esm_cause = NAS_ESM_INSUFFICIENT_RESOURCES;
      goto func_exit;    
    }

    //Convert and Store the data rate for later use.
    memset(&app_qos, 0, sizeof(qos_spec_type));
    if ( ds_bearer_flow_manager_convert_data_rate(bearer_mgr_p,
    	                                             &info_ptr->sdf_qos,
    	                                             &info_ptr->neg_qos,
                                                  &app_qos,
                                                  &icause_code) != TRUE)
    {
      ds_bearer_flow_manager_convert_cause_code(&temp_esm_cause,
                                                &icause_code);
      local_esm_cause = temp_esm_cause.esm_cause;
      local_err = DS_LOCAL_ERR_VAL_28;
      goto func_exit;    
    }
  }/* sdf_qos.valid */

  /*----------------------------------------------------------------------
    If APN AMBR is passed in default bearer activation, store it in
    the flow manager
  ----------------------------------------------------------------------*/
  if (info_ptr->apn_ambr.valid == TRUE)
  {
    pdn_flow_manager_p = ds_pdn_flow_mgr_get_entry(pdn_mgr_p->index);
    if(!pdn_flow_manager_p)
    {
      local_err = DS_LOCAL_ERR_VAL_29;
      local_esm_cause = NAS_ESM_INSUFFICIENT_RESOURCES;
      goto func_exit;    
    }

    memscpy(&pdn_flow_manager_p->eps_pdn_ambr, sizeof(apn_ambr_T),
           (void *)&(info_ptr->apn_ambr), sizeof(apn_ambr_T));

  }/* apn_ambr */

  /*----------------------------------------------------------------------
    Retrieve the protocol config options from CM struct mode_info_p
  ----------------------------------------------------------------------*/
  if((SYS_RAT_LTE_NB1_RADIO_ACCESS == ds_pdn_mgr_get_active_rat()
#ifdef FEATURE_NBIOT_NTN
  	|| SYS_RAT_LTE_NB1_NTN_RADIO_ACCESS == ds_pdn_mgr_get_active_rat()
#endif /* FEATURE_NBIOT_NTN */
       ) && TRUE == info_ptr->ext_protocol_conf_opt.valid)
  {
    ext_protocol_conf_opt_p = &(info_ptr->ext_protocol_conf_opt);
    ds_proto_config_options.valid = ds_pdn_pco_epco_hdlr_convert_nas_epco_to_ds
      (ext_protocol_conf_opt_p,&ds_proto_config_options);
  }
  else
  {
    protocol_config_options_p = &(info_ptr->protocol_config_options);
    ds_proto_config_options.valid = ds_pdn_pco_epco_hdlr_convert_nas_pco_to_ds
      (protocol_config_options_p,&ds_proto_config_options);
  }

  /*----------------------------------------------------------------------
    Configure the PDP IP Proto stack
  ----------------------------------------------------------------------*/
  if( ds_pdn_mgr_setup_pdp_proto_stack(pdn_mgr_p,
                                       &ds_pdn_addr,
                                       &ds_proto_config_options) == FALSE )
  {
    local_err = DS_LOCAL_ERR_VAL_30;
    goto func_exit; 
  }

  if(local_err == DS_LOCAL_ERR_VAL_NONE)
  {
    /*-------------------------------------------------------------------------
     Associate the APN Index for APN Rate Control if applicable
    -------------------------------------------------------------------------*/
    ds_pdn_apn_rate_ctrl_assign_apn_index_to_bearer((byte*)decoded_apn);

    /*-------------------------------------------------------------------------
     Fill in compression parameters.
    -------------------------------------------------------------------------*/
    accept_respone.h_comp = ds_pdn_mgr_conv_header_comp_from_pdp_t_to_sys_t(
                                             &profile_ptr->context.h_comp);
    accept_respone.d_comp = ds_pdn_mgr_conv_data_comp_from_pdp_t_to_sys_t(
                                             &profile_ptr->context.d_comp);

   /*-------------------------------------------------------------------------
   If APN is throttled, clear the throttling 
   -------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_CC_SP_THROTTLING
    if(ds_3gpp_throt_sm_is_cc_sp_throttling_enabled() == TRUE)
    {
      if(decode_apn_result == FALSE)
      {
        (void)strlcpy((char *)decoded_apn,
          (char *)pdn_mgr_p->pdp_profile->context.apn,
           DS_PROFILE_APN_STRING_LEN+1);
      }
      ds_3gpp_pdn_clear_cc_sp_throt_state_for_apn_on_current_plmn((byte *)decoded_apn);
    }
#endif /* FEATURE_DATA_CC_SP_THROTTLING */
  }

  if(!ds_eps_bearer_mgr_default_bearer_activate_ind(info_ptr, bearer_mgr_p))
  {    
    local_err = DS_LOCAL_ERR_VAL_31;
    goto func_exit; 
  }

  bearer_mgr_p->call_was_connected = TRUE;

  
  accept_respone.eps_bearer_id =  bearer_mgr_p->bearer_id;  
  accept_respone.message_header.message_id = 
                                          EMM_ACT_DEFAULT_BEARER_CONTEXT_ACCEPT;
  accept_respone.message_header.message_set = MS_MM_DS;      
  accept_respone.message_header.message_len_lsb =
    (sizeof( emm_act_default_bearer_context_accept_T ) - sizeof(IMH_T)) % 256;
  accept_respone.message_header.message_len_msb =
    (sizeof( emm_act_default_bearer_context_accept_T ) - sizeof(IMH_T)) / 256;
  
  /* Fill the same ESM header that we received from NAS */
  accept_respone.esm_hdr = info_ptr->esm_hdr;
  
  ds_pdn_mgr_ds_to_nas_send_msg((void*)&accept_respone);  
  DS_MSG1(MSG_LEGACY_HIGH,"Bearer activate accepted for bearer %d", 
                          bearer_mgr_p->index);   

  if(ds_apn_table_get_apn_ip_support_info(profile_ptr->context.apn,&dbg_ip_type))
  {
    DS_MSG1(MSG_LEGACY_HIGH,"Ip support for PDN in APN table %d",dbg_ip_type);
      DS_ASSERT(dbg_ip_type != DS_IP_TYPE_INVALID);
  }
    
  return TRUE;

func_exit:
  if(local_err != DS_LOCAL_ERR_VAL_NONE)
  {
    DS_MSG1(MSG_LEGACY_ERROR,"Default bearer activation failed with err %d",
           local_err);
    if(local_esm_cause != NAS_ESM_NO_FAILURE)
    {
      info_ptr->esm_cause.valid = TRUE;
      info_ptr->esm_cause.esm_cause = local_esm_cause;
    }
 
    /* In case of bearer reject, store the bearer ID passed by NAS in both
       reject message and bearer ptr. This is because bearer id is stored
       once activation is accepted. So, once reject is sent, NAS will send
       bearer deactivation, and this bearer ID will help to find out the
       correct bearer meant for cleanup.*/
    reject_respone.eps_bearer_id = info_ptr->eps_bearer_id;

    if(ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p))
    {
      bearer_mgr_p->bearer_id = info_ptr->eps_bearer_id;
    }
 
    if(info_ptr->esm_cause.valid)
    {
      reject_respone.esm_cause.valid = TRUE;
      reject_respone.esm_cause.esm_cause = info_ptr->esm_cause.esm_cause;
    }
          
    reject_respone.message_header.message_id = EMM_ACT_DEFAULT_BEARER_CONTEXT_REJ;
    reject_respone.message_header.message_set = MS_MM_DS;      
    reject_respone.message_header.message_len_lsb =
      (sizeof( emm_act_default_bearer_context_rej_T ) - sizeof(IMH_T)) % 256;
    reject_respone.message_header.message_len_msb =
      (sizeof( emm_act_default_bearer_context_rej_T ) - sizeof(IMH_T)) / 256;
    
    /* Fill the same ESM header that we received from NAS */
    reject_respone.esm_hdr = info_ptr->esm_hdr;
    
    ds_pdn_mgr_ds_to_nas_send_msg((void*)&reject_respone);
  }

  if(ds_apn_table_get_apn_ip_support_info(profile_ptr->context.apn,&dbg_ip_type))
      DS_ASSERT(dbg_ip_type != DS_IP_TYPE_INVALID);
  
  return FALSE;
} /* ds_eps_pdn_mgr_default_bearer_activate_ind */

/*===========================================================================
FUNCTION  DS_EPS_PDN_MGR_DEFAULT_BEARER_ACTIVATE_IND

DESCRIPTION
  This function accepts or rejects the the EPS dedicated bearer activate request.

PARAMETERS
  info_ptr:  Dedicated bearer activat information

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_eps_pdn_mgr_dedicated_bearer_activate_ind
(
  emm_act_dedicated_bearer_context_request_ind_s_type   *info_ptr
)
{
  ds_pdn_mgr_s                  *pdn_mgr_p = NULL;
  ds_bearer_mgr_s               *def_bearer_mgr_p = NULL;
  ds_bearer_mgr_s               *bearer_mgr_p = NULL;
  byte                          *current_apn = NULL;
  emm_act_dedicated_bearer_context_accept_T
                                 accept_response = {{0}};
  emm_act_dedicated_bearer_context_rej_T
                                 reject_response = {{0}};
  ds_local_err_val_e_type        local_err = DS_LOCAL_ERR_VAL_NONE;

  if ( info_ptr == NULL )
  {
    return;
  }

  if(info_ptr->esm_hdr.lbi == ESM_INVALID_BEARER_ID)
  {
    local_err = DS_LOCAL_ERR_VAL_0;
    reject_response.esm_cause.valid = TRUE;
    reject_response.esm_cause.esm_cause = NAS_ESM_UNKNOWN_EPS_BEARER_CONTEXT;
    goto func_exit;
  }
  
  def_bearer_mgr_p = 
                  ds_bearer_mgr_get_bearer_from_eps_id(info_ptr->esm_hdr.lbi);
  if(!ds_bearer_mgr_validate_bearer_ptr(def_bearer_mgr_p))
  {
    local_err = DS_LOCAL_ERR_VAL_1;
    reject_response.esm_cause.valid = TRUE;
    reject_response.esm_cause.esm_cause = NAS_ESM_UNKNOWN_EPS_BEARER_CONTEXT;
    goto func_exit;   
  }
  
  pdn_mgr_p = def_bearer_mgr_p->pdn_mgr_ptr;
  if(!ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p))
  {
    local_err = DS_LOCAL_ERR_VAL_2;
    goto func_exit;
  }

  /*-------------------------------------------------------------------------
    Ensure that the PDN state is correct. If the PDN state is in GOING_DOWN,
    then we have to reject the bearer activation.
  -------------------------------------------------------------------------*/
  if(pdn_mgr_p->state == DS_PDN_MGR_STATE_GOING_DOWN)
  {  
    local_err = DS_LOCAL_ERR_VAL_3;
    goto func_exit;
  }

  /*-------------------------------------------------------------------
      Verify if the dedicated bearer exists.
  --------------------------------------------------------------------*/
  bearer_mgr_p = ds_bearer_mgr_get_bearer_from_eps_id(info_ptr->eps_bearer_id);
  if(ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p))
  {
    local_err = DS_LOCAL_ERR_VAL_4;
    goto func_exit;   
  }

  /*-----------------------------------------------------------------------
    Allocate a bearer context from the free pool of bearers. If no bearer 
    is available, we should fail the call.
  -----------------------------------------------------------------------*/
  bearer_mgr_p = ds_bearer_mgr_allocate_new_bearer();
  
  if(!ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p))
  {
    local_err = DS_LOCAL_ERR_VAL_5;
    reject_response.esm_cause.valid = TRUE;
    reject_response.esm_cause.esm_cause = NAS_ESM_INSUFFICIENT_RESOURCES;
    goto func_exit;
  }
  
  if(!ds_pdn_mgr_store_bearer_context(pdn_mgr_p,bearer_mgr_p))
  {
    local_err = DS_LOCAL_ERR_VAL_6;
    reject_response.esm_cause.valid = TRUE;
    reject_response.esm_cause.esm_cause = NAS_ESM_INSUFFICIENT_RESOURCES;
    goto func_exit;
  }

  bearer_mgr_p->is_default = FALSE;

  /*-------------------------------------------------------------------------
    Invoke the flow manager call activation function to handle bearer
    activation indication
  -------------------------------------------------------------------------*/
  if(FALSE == ds_pdn_flow_manager_bearer_allocation_ind(
                                            pdn_mgr_p,
                                            bearer_mgr_p,
                                            info_ptr,
                                            &reject_response.esm_cause))
  {
    local_err = DS_LOCAL_ERR_VAL_7;
    goto func_exit;
  }

  ds_pdn_mgr_update_pco(bearer_mgr_p,
                        &info_ptr->protocol_config_options,
                        &info_ptr->ext_protocol_conf_opt);

  /*----------------------------------------------------------------------
   Associate the APN Index for APN Rate Control if applicable
  ----------------------------------------------------------------------*/
  current_apn = (byte *)pdn_mgr_p->pdp_profile->context.apn;
  if (strlen((const char*)(current_apn)) == 0)
  {
    (void)ds_apn_table_get_resolved_apn_name(pdn_mgr_p,(char **)&(current_apn));
  }

  ds_pdn_apn_rate_ctrl_assign_apn_index_to_bearer(current_apn);

  /*-------------------------------------------------------------------------
   Fill in compression parameters.
  -------------------------------------------------------------------------*/
  accept_response.h_comp = ds_pdn_mgr_conv_header_comp_from_pdp_t_to_sys_t(
                                  &pdn_mgr_p->pdp_profile->context.h_comp);
  accept_response.d_comp = ds_pdn_mgr_conv_data_comp_from_pdp_t_to_sys_t(
                                  &pdn_mgr_p->pdp_profile->context.d_comp);

  if(!ds_eps_bearer_mgr_dedicated_bearer_activate_ind(info_ptr, bearer_mgr_p))
  {    
    local_err = DS_LOCAL_ERR_VAL_9;
    goto func_exit; 
  }

  bearer_mgr_p->call_was_connected = TRUE;
  
  accept_response.eps_bearer_id =  bearer_mgr_p->bearer_id;  
  accept_response.message_header.message_id = 
                                 EMM_ACT_DEDICATED_BEARER_CONTEXT_ACCEPT;
  accept_response.message_header.message_set = MS_MM_DS;      
  accept_response.message_header.message_len_lsb =
  (sizeof( emm_act_dedicated_bearer_context_accept_T ) - sizeof(IMH_T)) % 256;
  accept_response.message_header.message_len_msb =
  (sizeof( emm_act_dedicated_bearer_context_accept_T ) - sizeof(IMH_T)) / 256;
  
  /* Fill the same ESM header that we received from NAS */
  accept_response.esm_hdr = info_ptr->esm_hdr;
  
  ds_pdn_mgr_ds_to_nas_send_msg((void*)&accept_response);  
  DS_MSG1(MSG_LEGACY_HIGH,"Bearer activate accepted for bearer %d", 
                          bearer_mgr_p->index);   
  return;  

func_exit :
  if(local_err != DS_LOCAL_ERR_VAL_NONE)
  {
    DS_ERROR_LOG_3("Dedicated bearer activation for bearerd id =d, lbi =d, "
                   "failed with error =d",
                   info_ptr->eps_bearer_id, info_ptr->esm_hdr.lbi,
                   local_err);
    
    if(ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p))
    {      
      //Deallocate bearer and unbind from PDN.
      pdn_mgr_p->num_active_bearer_context--;
      ds_pdn_mgr_remove_bearer_context(pdn_mgr_p,bearer_mgr_p);
      ds_bearer_mgr_free_dyn_mem(bearer_mgr_p);
    }
  }

  reject_response.eps_bearer_id =  info_ptr->eps_bearer_id;  
  reject_response.message_header.message_id = 
                                 EMM_ACT_DEDICATED_BEARER_CONTEXT_REJ;
  reject_response.message_header.message_set = MS_MM_DS;      
  reject_response.message_header.message_len_lsb =
  (sizeof( emm_act_dedicated_bearer_context_rej_T ) - sizeof(IMH_T)) % 256;
  reject_response.message_header.message_len_msb =
  (sizeof( emm_act_dedicated_bearer_context_rej_T ) - sizeof(IMH_T)) / 256;
  
  /* Fill the same ESM header that we received from NAS */
  reject_response.esm_hdr = info_ptr->esm_hdr;
  
  ds_pdn_mgr_ds_to_nas_send_msg((void*)&reject_response);  
  DS_MSG1(MSG_LEGACY_HIGH,"Bearer activate rejected for bearer %d", 
                          info_ptr->eps_bearer_id);   
  return;
}

/*===========================================================================
FUNCTION DS_EPS_PDN_MGR_ATTACH_COMPLETE_IND_HDLR

DESCRIPTION
  This function is used to handle the message from NAS. This message is sent
  when NAS gets the L2 ack for attach complete message. This is the message
  that marks the completion of attach procedure.

  Clients that are interested in the completion of attach will be notified at this point.

  Note: This message has no payload.

PARAMETERS
  msg_ptr - Pointer to the payload

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_eps_pdn_mgr_attach_complete_ind_hdlr
(
  emm_attach_complete_ind_type   *msg_p
)
{
  uint8              bearer_cntx_inst;
  ds_bearer_mgr_s   *bearer_mgr_p = NULL;
  ds_pdn_mgr_s      *pdn_mgr_p = NULL;
  ds_pdn_apm_ind_pay_load     apm_pay_load = {0};
  
  if(msg_p == NULL)
  {
    DS_ERROR_LOG_0("NULL info");
    return;
  }
  
  /*--------------------------------------------------------------------------- 
    Check if DS requested attach with PDN and NW accepted w/o PDN
  ----------------------------------------------------------------------------*/
  if (FALSE == ds_eps_pdn_attach_sm.attach_wo_pdn_supported && 
      TRUE == msg_p->attach_without_pdn)
  {
    DS_ERROR_LOG_0("Clean up all bearers");
    for( bearer_cntx_inst=0;\
         bearer_cntx_inst<DS_3GPP_MAX_BEARER_CONTEXT;\
         bearer_cntx_inst++ )
    {
      bearer_mgr_p = ds_bearer_mgr_get_instance_ptr(bearer_cntx_inst);
    
      if(!ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p))
      {
        continue;
      }
    
      pdn_mgr_p = bearer_mgr_p->pdn_mgr_ptr;
      bearer_mgr_p->state = DS_BEARER_MGR_STATE_DOWN;
      ds_bearer_mgr_local_cleanup(bearer_mgr_p);
    }
    
  }

  /*--------------------------------------------------------------------------- 
    Set ota_attach_performed flag to TRUE
  ----------------------------------------------------------------------------*/
  ds_eps_pdn_attach_sm.ota_attach_performed = TRUE;
  ds_eps_pdn_attach_sm.cached_seq_num = 0;
  ds_eps_pdn_attach_sm.cached_paging_response = FALSE;
  ds_eps_pdn_attach_sm.cached_attach_wo_pdn_supported = FALSE;
  ds_eps_pdn_attach_sm.seq_num = 0;
  ds_eps_pdn_attach_sm.responding_to_page = FALSE;
  ds_eps_pdn_attach_sm.is_gw_originated = FALSE;
  ds_eps_pdn_attach_sm.attach_wo_pdn_supported = msg_p->attach_without_pdn;
  memset((void*)&ds_eps_pdn_attach_sm.prev_attach_status,
         0,
         sizeof(ds_eps_prev_attach_status_type));

  ds_pdn_hdlr_set_attach_complete_flag(TRUE);
  if ( TRUE == msg_p->attach_without_pdn)
  {
    ds_pdn_hdlr_set_attach_initiated_flag(FALSE);
  }
  
  ds_eps_pdn_attach_sm_set_state(DS_EPS_ATTACHED);

  /*---------------------------------------------------------------------
    Notify APM module for LTE ATTACH COMPLETED
  ---------------------------------------------------------------------*/
  (void)ds_pdn_apm_ind(DS_PDN_APM_ATTACH_COMPLETE_IND,&apm_pay_load);

  
  ds_3gpp_pdn_throt_advertise_blocked_apns_current_plmn();
  ds_3gpp_pdn_throt_sm_advertise_blocked_apns_to_mppm();
  
  /* MO Exception handling */
  /* Trigger instance up cmd if attach was due to force attach from MO Exception */
  if (ds_pdn_cp_data_is_force_attach_in_progress() == TRUE) 
  {
    ds_pdn_cp_data_resume_inst_up_for_force_attach();
  }
  
  /* End MO Exception handling */
  /*----------------------------------------------------------------------
    We have reached steady state - LTE Full Service - reset Target_RAT
  ----------------------------------------------------------------------*/
  DS_MSG0(MSG_LEGACY_HIGH, "Attach complete."); 
  return;
} /* ds_eps_pdn_mgr_attach_complete_ind_hdlr */

/*===========================================================================
FUNCTION  DS_EPS_PDN_MGR_MAP_PDN_REJ_FAILURE_TO_DOWN_REASON

DESCRIPTION
  This function processes the mode information from NAS and determines
  what the mapped ss down reason should be.

PARAMETERS
  mode_info_p:  Mode specific information
  down reason:  populates the down reason mapping to the mode info
                ESM or EMM failures from CM/NAS.

DEPENDENCIES
  None.

RETURN VALUE
  ds_net_down_reason_type

SIDE EFFECTS
  None.

===========================================================================*/
void ds_eps_pdn_mgr_map_pdn_rej_failure_to_down_reason
(
  ds_bearer_deact_params_s            *info_p,
  ds_net_down_reason_type                 *ds_down_reason_p
)
{
  if(info_p == NULL ||ds_down_reason_p == NULL)
  {
    DS_ERROR_LOG_0("Pdn Rej Ind: In Params Null.");
  }
  else
  {
    if( info_p->esm_cause.valid ||
    	   info_p->esm_local_cause.valid )
    {
      ds_eps_bearer_mgr_map_esm_cause_codes_to_ds(ds_down_reason_p,
                                                  info_p->esm_cause,
                                                  info_p->esm_local_cause);
    }
    else if (info_p->emm_failure_cause.cause_type != LTE_NAS_IRAT_NONE)
    {
      (void)ds_eps_bearer_mgr_map_emm_cause_codes_to_ds(ds_down_reason_p,
                                                info_p->emm_failure_cause);
    }
  }

  return;
} /* ds_eps_pdn_cntxt_map_pdn_rej_failure_to_down_reason */

/*===========================================================================
FUNCTION  DS_EPS_PDN_MGR_BEARER_DEACTIVATE_CNF

DESCRIPTION
  This function deactivates bearer.

PARAMETERS
  info_ptr_i:  Bearer deactivation params

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_eps_pdn_mgr_bearer_deactivate_cnf
(
  ds_bearer_deact_params_s   *info_ptr,
  ds_bearer_mgr_s                *bearer_mgr_p,
  ds_pdn_mgr_s                   *pdn_mgr_p  
)
{
  ds_pdn_mgr_inst_type_s         *v4_inst = NULL;
  ds_pdn_mgr_inst_type_s         *v6_inst = NULL;
  ds_pdn_mgr_inst_type_s         *non_ip_inst = NULL;
  boolean                         rel12_plus = FALSE;
  char                           *apn = NULL;
  protocol_cfg_options_T         *protocol_config_options_p = NULL;
  ext_protocol_cfg_options_T     *ext_protocol_conf_opt_p = NULL;
  ds_pdn_protocol_cfg_options_s  ds_proto_config_options = {0};
  uint32                          decode_mask = 0;  
  uint8                           index;
  ds_net_down_reason_type         down_reason=DS_NET_DOWN_REASON_NOT_SPECIFIED;
  uint32                          rat_mask = DS_3GPP_THROT_RAT_MASK_GLOBAL;

  /*------------------------------------------------------------------------ 
    Fix to cache the bearer deactivtion till DS is not completetly registered
    with L2.
   --------------------------------------------------------------------------*/
  if((bearer_mgr_p->state == DS_BEARER_MGR_STATE_PEND_RAB_REG ||
      bearer_mgr_p->state == DS_BEARER_MGR_STATE_UP_DORMANT_REESTAB) &&
     (bearer_mgr_p->eps_bearer != NULL &&
      bearer_mgr_p->eps_bearer->pdcp_reg_req_sent == TRUE))
  {
    DS_MSG0(MSG_LEGACY_HIGH,"PDCP register req is still pending. Cache the deactivate request");
    if(bearer_mgr_p->eps_bearer != NULL)
    {
      bearer_mgr_p->eps_bearer->deact_req_cached = TRUE;
      DS_SYSTEM_HEAP_MEM_ALLOC(bearer_mgr_p->eps_bearer->cached_deact_params,
                             sizeof(ds_bearer_deact_params_s), ds_bearer_deact_params_s *);
      if(bearer_mgr_p->eps_bearer->cached_deact_params != NULL)
      {
        DS_MSG0(MSG_LEGACY_HIGH,"Caching the deactivation params");
        memset(bearer_mgr_p->eps_bearer->cached_deact_params,0,sizeof(ds_bearer_deact_params_s));
        memscpy(bearer_mgr_p->eps_bearer->cached_deact_params, sizeof(ds_bearer_deact_params_s),
                info_ptr, sizeof(ds_bearer_deact_params_s));
      }
    }
    return;
  }

  /*------------------------------------------------------------------------ 
    Before deactivating, check if pdn_contexts apn param change flag is set
    If set, make a few validations and make a decision, if the bearer and PDN
    should continue to remain in PENDING_REDIAL mode or move it to GOING_DOWN
    Basically if the validation fails, a redial on the same PDN is not
    possible and hence move the state to GOING DOWN.   
    The redial mechanism is currently only supported for default bearers
   --------------------------------------------------------------------------*/    
  if(bearer_mgr_p->is_default)
  {
    if( pdn_mgr_p->lte_params &&
        pdn_mgr_p->lte_params->apn_param_chg_teardown == TRUE )
    {
      ds_eps_pdn_move_bearer_from_pending_redial_to_down_if_needed(
                                                          bearer_mgr_p);
    }
  }

  if( bearer_mgr_p->state == DS_BEARER_MGR_STATE_DOWN ||
      bearer_mgr_p->state == DS_BEARER_MGR_STATE_PEND_REDIAL )
  {
    // to reset attach SM or to process cached attach request
     ds_eps_bearer_mgr_deactivate_last_bearer_if_needed();
    
     /*-------------------------------------------------------------------------- 
       If pdn context is in PENDING REDIAL state and if apn param chg teardown is
       set to TRUE, send out a PDN connectivity request out.
      
       We need not send out redial request in case of dedicated bearers since
       they have to be torn down
       --------------------------------------------------------------------------*/
     if (ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p) &&
         ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p) &&
         pdn_mgr_p->lte_params != NULL)
     {
       if((pdn_mgr_p->state == DS_PDN_MGR_STATE_PEND_REDIAL) && 
          (pdn_mgr_p->lte_params->apn_param_chg_teardown) &&
          (bearer_mgr_p->is_default == TRUE))
       {
         DS_MSG0(MSG_LEGACY_HIGH,
                  "Sending out PDN connectivity req, Apn param chg");
         ds_eps_pdn_handle_apn_param_chg_redial(pdn_mgr_p,bearer_mgr_p);
       }
    
       if((pdn_mgr_p->state == DS_PDN_MGR_STATE_PEND_REDIAL) &&
           pdn_mgr_p->redial_reason == DS_PDN_MGR_REDIAL_OP_PCO_CHG)
       {
         DS_MSG0(MSG_LEGACY_HIGH,
                  "Redial because of OP PCO chg");
         pdn_mgr_p->redial_reason = DS_PDN_MGR_NO_REDIAL;
         pdn_mgr_p->state = DS_PDN_MGR_STATE_COMING_UP;
    
         // Need to add OP PCO change handling here
       }
     }

    return;
  }

  if (ds_pdn_nv_manager_is_rel_12_or_above_supported())
  {
    rel12_plus = TRUE;
  }

  apn = pdn_mgr_p->pdp_profile->context.apn;
  if(*apn == '\0')
  {
    (void)ds_apn_table_get_resolved_apn_name(pdn_mgr_p, &apn);
  }

    /*----------------------------------------------------------------------
    Start the PDN Wait Timer as the default bearer is being deactivated
    This needs to be done only for non-attach PDNs.
  -----------------------------------------------------------------------*/
  if(ds_3gpp_pdn_limit_is_enabled() && 
     pdn_mgr_p->ue_initiated_tear_down && 
     bearer_mgr_p->is_default)
  {
    if((index = ds_3gpp_pdn_limit_tbl_find_entry((byte *)apn)) 
         != DS_3GPP_PDN_LIMIT_TBL_INVALID_ENTRY )
    {
      ds_3gpp_pdn_limit_start_pdn_wait_tmr(index);
    }
  }

  v4_inst = pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV4];
  v6_inst = pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV6];
  non_ip_inst = pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_NON_IP];
 
  /*----------------------------------------------------------------------
      Retrieve the protocol config options from CM struct mode_info_p
    ----------------------------------------------------------------------*/
  if( TRUE == info_ptr->esm_cause.valid )
  {
    if((SYS_RAT_LTE_NB1_RADIO_ACCESS == ds_pdn_mgr_get_active_rat()
#ifdef FEATURE_NBIOT_NTN
        || SYS_RAT_LTE_NB1_NTN_RADIO_ACCESS == ds_pdn_mgr_get_active_rat()
#endif /* FEATURE_NBIOT_NTN */
       ) && TRUE == info_ptr->ext_protocol_conf_opt.valid)
    {
      ext_protocol_conf_opt_p = &(info_ptr->ext_protocol_conf_opt);
      ds_proto_config_options.valid = ds_pdn_pco_epco_hdlr_convert_nas_epco_to_ds
          (ext_protocol_conf_opt_p,&ds_proto_config_options);
    }
    else //pco
    {
      protocol_config_options_p = &(info_ptr->protocol_config_options);
      ds_proto_config_options.valid = ds_pdn_pco_epco_hdlr_convert_nas_pco_to_ds
          (protocol_config_options_p,&ds_proto_config_options);
    }
  }

  /*-------------------------------------------------------------------------
    Get any other protocol specific options that are received and store it
    in the call specific structure. Additon of decode Protocol Params in call end
  -------------------------------------------------------------------------*/
  decode_mask = ds_pdn_mgr_decode_proto_params(pdn_mgr_p,
                                               &ds_proto_config_options);

  // Clean up bearer info
  ds_bearer_mgr_info_cleanup(bearer_mgr_p);

  /*-----------------------------------------------------------------
    We need to check for APN Switching action if this is last standing
    PDN and whether any switching action has been already decided on
    attach reject
   
    If a switching action has been already decided on attach reject,
    then we need not perform additional check for Switch
  ------------------------------------------------------------------*/
  if( FALSE == ds_pdn_mgr_is_any_other_pdn_up(pdn_mgr_p) &&
      bearer_mgr_p->is_default )
  {
    ds_pdn_apn_switch_mgr_populate_switching_payload_per_rat(
       SYS_SYS_MODE_LTE,
       DS_PDN_APM_DETACH_NETWORK_CAUSE,
       pdn_mgr_p->pdp_profile_num,
       pdn_mgr_p->state,
       info_ptr);
  }

  if(ds_pdn_mgr_is_bearer_deactivated_by_network(pdn_mgr_p,
                                                 bearer_mgr_p))

  {
    (void)ds_3gpp_network_override_hdlr_set_overridden_ip
                      ((byte *)pdn_mgr_p->pdp_profile->context.apn,
                       DS_3GPP_PDN_THROT_PDP_UNDEFINED,
                       DS_3GPP_PDN_THROT_NTW_O_RIDE_CAUSE_MAX,
                       DS_3GPP_THROT_RAT_MASK_LTE,
                       info_ptr->reattempt_indicator);
  }

  /* Check the NW cause code to see if more details can be provided as 
     Net down reason */
  if(info_ptr->emm_failure_cause.cause_type != LTE_NAS_IRAT_NONE)
  {
    DS_MSG1(MSG_LEGACY_HIGH,
    	       "Received valid EMM cause in deact_cnf,cause type : %d",
            info_ptr->emm_failure_cause.cause_type);

    DS_IOT_MSG1(MSG_LEGACY_HIGH,
    	       "Received valid EMM cause in deact_cnf,cause type : %d",
            info_ptr->emm_failure_cause.cause_type);
   
    ds_eps_bearer_mgr_map_emm_cause_codes_to_ds(&down_reason,
                                                info_ptr->emm_failure_cause);
  }
  else if ( info_ptr->esm_cause.valid ||
            info_ptr->esm_local_cause.valid )
  {
    DS_MSG2(MSG_LEGACY_HIGH,
    	       "Received valid cause in deact_cnf : ESM cause valid: %d "
            "ESM local cause valid: %d", 
            info_ptr->esm_cause.valid,
            info_ptr->esm_local_cause.valid);

    DS_IOT_MSG2(MSG_LEGACY_HIGH,
            "Received valid cause in deact_cnf : ESM cause valid: %d "
            "ESM local cause valid: %d", 
            info_ptr->esm_cause.valid,
            info_ptr->esm_local_cause.valid);


    ds_eps_bearer_mgr_map_esm_cause_codes_to_ds(&down_reason,
                                                info_ptr->esm_cause,
                                                info_ptr->esm_local_cause);

    if( ds_pdn_nv_manager_is_rel_11_or_above_supported() )
    {
      /* ----------------------------------------------------------------
      New function added as part of Release 11. This functions handles
      the NAS pdp reject with t3396 ie. 
      ------------------------------------------------------------------*/
      if(info_ptr->esm_cause.esm_cause == NAS_ESM_INSUFFICIENT_RESOURCES &&
         pdn_mgr_p->state == DS_PDN_MGR_STATE_UP)
      {
        if (ds_pdn_hdlr_is_ue_in_ac11to15() && 
            ds_pdn_nv_manager_is_rel_11_or_above_supported())
        {
          DS_MSG0(MSG_LEGACY_HIGH, 
                  "UE is AC: 11-15, not blocking APN for attach");
        }
        else
        {
          ds_pdn_throt_spec_lte_handle_nas_reject_with_t3396_ie(info_ptr,
                                                                pdn_mgr_p,
                                                                down_reason);
          bearer_mgr_p->state = DS_BEARER_MGR_STATE_GOING_DOWN;
          ds_3gpp_pdn_throt_advertise_blocked_apns_current_plmn();
          
          ds_3gpp_pdn_cntxt_disable_lte_if_attach_profile_unavailable();
        }
      }
    }
  }
  else if(pdn_mgr_p->net_down_reason != DS_NET_DOWN_REASON_NOT_SPECIFIED)
  {
    DS_MSG1(MSG_LEGACY_HIGH,"APN invalid param change.Down_reason:0x%x",
            down_reason);
    down_reason = pdn_mgr_p->net_down_reason;
  }

#ifdef FEATURE_DATA_REL10
  if(ds_pdn_nv_manager_get_nv_lte_rel_10())
  {
    if(down_reason == DS_NET_DOWN_REASON_UMTS_REATTACH_REQ)
    {
      /*---------------------------------------------------------------------
        EPS bearer context deactivation request with cause #39 �reactivation                                  
        got successfull processed by UE. Clear APN throttling state machine                                     
        if it was globally blocked i.e. unthrottle the APN
      ----------------------------------------------------------------------*/
      DS_MSG_SPRINTF_1(MSG_LEGACY_HIGH,
                       "Trying to unblock potentially blocked APN: %s",
                       (char*)apn);
      /*---------------------------------------------------------------------
        Unblock the APN on Global RAT and LTE RAT if throttling per RAT is
        enabled
      ----------------------------------------------------------------------*/
      if (ds_pdn_nv_manager_get_enable_pdn_throt_per_rat()||
          ds_pdn_nv_manager_is_rel_12_or_above_supported())
      {
        rat_mask |= DS_3GPP_THROT_RAT_MASK_LTE;
      }

      ds_3gpp_pdn_throt_handle_nw_inited_unblock_apn((byte *)apn, rat_mask);
    }
  }
#endif /*FEATURE_DATA_REL10*/  

  // Free back pressure timer
  if(bearer_mgr_p->tmr_hndl_dyn_back_pressure != PS_TIMER_INVALID_HANDLE)
  {
    PS_TIMER_FREE_HANDLE(bearer_mgr_p->tmr_hndl_dyn_back_pressure);
    bearer_mgr_p->tmr_hndl_dyn_back_pressure = PS_TIMER_INVALID_HANDLE;
  }

  if(bearer_mgr_p->is_default)
  {
    bearer_mgr_p->state = DS_BEARER_MGR_STATE_PEND_REDIAL;
    ds_pdn_mgr_bearer_down_ind(pdn_mgr_p, bearer_mgr_p, down_reason);    	                          
  }
  else
  {
    pdn_mgr_p->net_down_reason = down_reason;
    bearer_mgr_p->state = DS_BEARER_MGR_STATE_DOWN;
    ds_eps_bearer_mgr_dereg_wmk(bearer_mgr_p);
  }
 
  return;
}

/*===========================================================================
FUNCTION  DS_EPS_PDN_MGR_BEARER_DEACTIVATE_IND_HDLR

DESCRIPTION
  This function handles the EPS bearer deactivate request.

PARAMETERS
  info_ptr:  Deactivation params

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_eps_pdn_mgr_bearer_deactivate_ind_hdlr
(
  emm_deact_bearer_context_request_ind_s_type   *deact_req
)
{
  ds_bearer_mgr_s               *bearer_mgr_p = NULL;
  ds_pdn_mgr_s                  *pdn_mgr_p = NULL;
  ds_bearer_deact_params_s   info = {0};
  
  bearer_mgr_p = ds_bearer_mgr_get_bearer_from_eps_id(deact_req->eps_bearer_id);
  if(!ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p))
  {
    return;
  }
  
  pdn_mgr_p = bearer_mgr_p->pdn_mgr_ptr;
  if(!ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p))
  {
    return;
  }
  
  DS_MSG3(MSG_LEGACY_HIGH,"Bearer deactivation, PDN %d , bearer %d , state %d",
         pdn_mgr_p->index, bearer_mgr_p->index, bearer_mgr_p->state);
  
  // Remove the pending flow which was sent to NW for modifi req to delete flows.
  ds_bearer_flow_manager_check_sdf_id(deact_req->esm_hdr.sdf_id);

  /* Copy the deact params */
  info.eps_bearer_id = deact_req->eps_bearer_id;
  info.esm_cause= deact_req->esm_cause;
  info.esm_local_cause = deact_req->esm_local_cause;
  info.backoff_timer = deact_req->backoff_timer;
  info.protocol_config_options = deact_req->protocol_config_options;
  info.ext_protocol_conf_opt = deact_req->ext_protocol_conf_opt; 

  /* Deactivate the bearer */
  ds_eps_pdn_mgr_bearer_deactivate_cnf(&info, bearer_mgr_p, pdn_mgr_p);

  return;
}

/*===========================================================================
FUNCTION DS_EPS_PDN_MGR_PDN_CONN_REJ_IND_HDLR

DESCRIPTION
  This function is used to handle the PDN Conn Reject indication.
  It calls the bearer deactivate function which does the clean up of the
  bearer and intiates the clean up of the PDN context.

  This function is applicable only to EPS mode of operation.

PARAMETERS
  

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_eps_pdn_mgr_pdn_conn_rej_ind_hdlr
(
  emm_pdn_connectivity_reject_ind_s_type  *rej_info_p
)
{
  ds_bearer_mgr_s               *bearer_mgr_p = NULL;
  ds_pdn_mgr_s                  *pdn_mgr_p = NULL;
  ds_bearer_deact_params_s   info = {0};
  ds_net_down_reason_type        down_reason = 
                                      DS_NET_DOWN_REASON_NOT_SPECIFIED;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy the deact params */
  info.esm_cause= rej_info_p->esm_cause;
  info.esm_local_cause = rej_info_p->esm_local_cause;
  info.emm_failure_cause = rej_info_p->emm_failure_cause;
  info.backoff_timer = rej_info_p->backoff_timer;
  info.reattempt_indicator = rej_info_p->reattempt_indicator;
  info.pdn_reject_ind = rej_info_p->pdn_reject_ind;
  info.protocol_config_options = rej_info_p->protocol_config_options;
  info.ext_protocol_conf_opt = rej_info_p->ext_protocol_conf_opt;

  ds_eps_pdn_mgr_map_pdn_rej_failure_to_down_reason(&info,
                                                    &down_reason);

  /*----------------------------------------------------------------------
     In case of Attach failure, reject, error, etc, we will come here.
     We need to clean up the PDN here for the MO exception
     call if FORCE ATTACH is called.
  ----------------------------------------------------------------------*/
  ds_pdn_cp_data_cleanup_inst_after_force_attach_failure(down_reason);

  if (TRUE == ds_eps_pdn_attach_sm.attach_wo_pdn_supported && 
      DS_EPS_ATTACHED != ds_eps_pdn_attach_sm.attach_state )
  {
    ds_eps_attach_hdlr_attach_wo_pdn_rej_hdlr();
    return;
  }
  
  bearer_mgr_p = 
  	     ds_bearer_mgr_get_bearer_from_sdf_id(rej_info_p->esm_hdr.sdf_id);
  if(!ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p))
  {
    DS_ERROR_LOG_1("Bearer not found for SDF ID =d", 
    	              rej_info_p->esm_hdr.sdf_id);
    return;
  }

  /*-------------------------------------------------------------------------
    Get the PDN context from the bearer context
  -------------------------------------------------------------------------*/
  pdn_mgr_p = bearer_mgr_p->pdn_mgr_ptr;
  if(!ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p))
  {
    return;
  }
  pdn_mgr_p->lte_params->apn_param_chg_teardown = FALSE;

  /*----------------------------------------------------------------------
    Invoke PDN Throttle Rules Module to run the PDN Throttle Rules
    and determine the kind of PDN Throttling that needs to be performed 
  ----------------------------------------------------------------------*/
  ds_pdn_throt_rule_notify(pdn_mgr_p, &info, 
                           SYS_SYS_MODE_LTE, down_reason);

  /*-----------------------------------------------------------------
      We need to check for APN Switching action if this is last standing
      PDN
    ------------------------------------------------------------------*/
  if ( (FALSE == ds_pdn_mgr_is_any_other_pdn_up(pdn_mgr_p)) &&
       (TRUE == pdn_mgr_p->is_default) )
  {
    ds_pdn_apn_switch_mgr_populate_switching_payload_per_rat(
        pdn_mgr_p->nw_mode, 
        DS_PDN_APM_REJECT_NETWORK_CAUSE,
        pdn_mgr_p->pdp_profile_num,
        pdn_mgr_p->state,
        &info);
  }

  if((!rej_info_p->esm_local_cause.valid) ||
     (rej_info_p->esm_local_cause.local_cause != EMM_DETACHED))
  {
    DS_ERROR_LOG_0("DEBUG_ATTACH: older cache sequence no reset");
    ds_eps_pdn_attach_sm.cached_paging_response = FALSE;
  }
  
  /* Deactivate the bearer */
  ds_eps_pdn_mgr_bearer_deactivate_cnf(&info, bearer_mgr_p, pdn_mgr_p);

  return;
} /* ds_eps_pdn_mgr_pdn_conn_rej_ind_hdlr */

/*===========================================================================
FUNCTION DS_EPS_PDN_MGR_PDN_CONN_FAIL_IND_HDLR

DESCRIPTION
  This function is used to handle the PDN Conn failure indication.
  It calls the bearer deactivate function which does the clean up of the
  bearer and intiates the clean up of the PDN context.

  This function is applicable only to EPS mode of operation.

PARAMETERS
  

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_eps_pdn_mgr_pdn_conn_fail_ind_hdlr
(
  emm_pdn_connectivity_failure_ind_s_type  *fail_info_p
)
{
  ds_bearer_mgr_s               *bearer_mgr_p = NULL;
  ds_pdn_mgr_s                  *pdn_mgr_p = NULL;
  ds_bearer_deact_params_s   info = {0};
  ds_net_down_reason_type        down_reason = 
                                      DS_NET_DOWN_REASON_NOT_SPECIFIED;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*----------------------------------------------------------------------
     In case of Attach failure, reject, error, etc, we will come here.
     We need to clean up the PDN here for the MO exception
     call if FORCE ATTACH is called.
  ----------------------------------------------------------------------*/
  ds_pdn_cp_data_cleanup_inst_after_force_attach_failure(down_reason);

  if (TRUE == ds_eps_pdn_attach_sm.attach_wo_pdn_supported && 
      DS_EPS_ATTACHED != ds_eps_pdn_attach_sm.attach_state )
  {
    ds_eps_attach_hdlr_attach_wo_pdn_rej_hdlr();
    return;
  }
  
  bearer_mgr_p = 
  	     ds_bearer_mgr_get_bearer_from_sdf_id(fail_info_p->esm_hdr.sdf_id);
  if(!ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p))
  {
    DS_ERROR_LOG_1("Bearer not found for SDF ID =d", 
    	              fail_info_p->esm_hdr.sdf_id);
    return;
  }

  /*-------------------------------------------------------------------------
    Get the PDN context from the bearer context
  -------------------------------------------------------------------------*/
  pdn_mgr_p = bearer_mgr_p->pdn_mgr_ptr;
  if(!ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p))
  {
    return;
  }
  
  /* Deactivate the bearer */
  ds_eps_pdn_mgr_bearer_deactivate_cnf(&info, bearer_mgr_p, pdn_mgr_p);

  return;
} /* ds_eps_pdn_mgr_pdn_conn_fail_ind_hdlr */

/*===========================================================================
FUNCTION      DS_EPS_PDN_MGR_GET_LTE_ATTACH_PARAMS

DESCRIPTION
  Updates the Attach profile APN and PDP type info when an IOCTL query
  is received if ota_attach_performed is TRUE.
  Returns Failure if
  1. Input parameters are invalid.
  2. Default PDN context is not UP.

PARAMETERS
  ioctl_name[in]  -      Operation name
  arg_val_ptr[in] -  Pointer to operation specific argument
  ps_errno [out]    -    Specifies type of error in case of error.
                             DS_EFAULT if parameter is invalid
                             DS_EINVAL if operation is invalid
                             DS_NOMEMORY if memory allocation fails

DEPENDENCIES
  None.

RETURN VALUE
  0  - Success
 -1 - Failure

SIDE EFFECTS
  None.
===========================================================================*/
int ds_eps_pdn_mgr_get_lte_attach_params
(
  void                    *arg_val_ptr,
  int16                   *ps_errno
)
{
  ds_pdn_mgr_s						                         		 *pdn_mgr_p = NULL;
  ds_sys_ioctl_3gpp_lte_attach_pdn_params_type	 *apn_info = NULL;
  byte											                               *apn_ptr = NULL;
/*-------------------------------------------------------------------------*/
	
  if ((arg_val_ptr == NULL) || (ps_errno == NULL))
  {
    if(ps_errno != NULL)
    {
      *ps_errno = DS_EFAULT;
    }
    
    DS_ERROR_LOG_0("Null Pointer passed");
    return -1;
  }
  
  memset(arg_val_ptr, 0, sizeof(ds_sys_ioctl_3gpp_lte_attach_pdn_params_type));
  apn_info = (ds_sys_ioctl_3gpp_lte_attach_pdn_params_type*)arg_val_ptr;
  pdn_mgr_p = ds_pdn_mgr_get_def_pdn();

  if(!ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p))
  {
    *ps_errno = DS_EINVAL;
     return -1;
  }

  if( pdn_mgr_p->state != DS_PDN_MGR_STATE_UP)
  {
    DS_ERROR_LOG_0("LTE Attach not complete,PDN not in up state");
    *ps_errno = DS_EINVAL;
    return -1;
  }
	
  if(strlen((char*)pdn_mgr_p->pdp_profile->context.apn) != 0)
  {
    memscpy((char*)apn_info->apn_string,
            strlen((char*)pdn_mgr_p->pdp_profile->context.apn),
         			(char*)pdn_mgr_p->pdp_profile->context.apn,
         			strlen((char*)pdn_mgr_p->pdp_profile->context.apn));
  }
  else if(ds_apn_table_get_resolved_apn_name(pdn_mgr_p, 
                                             (char**)&apn_ptr) == FALSE)
  {
	 /*--------------------------------------------------------------------- 
	   This is the case when both APN/Resolved APN are NULL.
	 -----------------------------------------------------------------------*/
   	DS_ERROR_LOG_0("APN/Resolved APN unavailable in the APN table");
    memset(apn_info->apn_string,0,DS_PROFILE_APN_STRING_LEN+1);
  }
  else
  {
    memscpy((char *)apn_info->apn_string, DS_PROFILE_APN_STRING_LEN+1,
		          (char *)apn_ptr, DS_PROFILE_APN_STRING_LEN+1);
  }
	  
  switch(ds_apn_table_get_pdn_pdp_type(pdn_mgr_p))
 	{
    case DS_PROFILE_PDP_IPV4:
      apn_info->ip_type = DS_SYS_IOCTL_IP_SUPPORT_TYPE_IPV4;
      break;
      
    case DS_PROFILE_PDP_IPV6:
      apn_info->ip_type = DS_SYS_IOCTL_IP_SUPPORT_TYPE_IPV6;
    		break;
    		
    case DS_PROFILE_PDP_IPV4V6:
      apn_info->ip_type = DS_SYS_IOCTL_IP_SUPPORT_TYPE_IPV4IPV6;
      break;
      
    case DS_PROFILE_PDP_NON_IP:
      apn_info->ip_type = DS_SYS_IOCTL_IP_SUPPORT_TYPE_NON_IP;
      break;
      
    default:
      DS_ERROR_LOG_0("IP type is not valid");
    		*ps_errno = DS_EINVAL;
    		return -1;
  }
	
  apn_info->apn_string_len = strlen((char *)apn_info->apn_string);
  apn_info->ota_attach_performed = ds_eps_pdn_attach_sm.ota_attach_performed;
  *ps_errno = 0; /* No error */
   return 0;
}/* ds_eps_pdn_mgr_get_lte_attach_params */

/*===========================================================================
FUNCTION DS_EPS_PDN_MGR_APN_PARAM_CHANGE_BEARER_TEARDOWN

DESCRIPTION
  This function tears down the bearer and puts the bearer and PDN context
  in an appropriate state

PARAMETERS 
  bearer_mgr_p
 
DEPENDENCIES
  None.

RETURN VALUE
  void 
 
SIDE EFFECTS
  None.
===========================================================================*/
void ds_eps_pdn_mgr_apn_param_change_bearer_teardown
(
   ds_bearer_mgr_s *bearer_mgr_p
)
{
   ds_pdn_mgr_s *pdn_mgr_p = NULL;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
 
   if (!ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p))
   {
     return;
   }

   // Prepare for tear down
   ds_bearer_mgr_unset_ul_data_bit_mask(bearer_mgr_p);
   bearer_mgr_p->rab_reestab_retry_cnt = 0;

   /*------------------------------------------------------------------------ 
     Disable the flow and this will ensure that no data retries will be made
     in the whole process of disconnecting and reconnecting the PDN.
     Flow will be enabled when a bearer is reestablished.
   -------------------------------------------------------------------------*/
   ds_bearer_mgr_disable_tx_flow(bearer_mgr_p, DS_BEARER_CONTROL_MASK);
   
   /*-------------------------------------------------------------------------- 
     Send out a PDN disconnect only on the default bearer, The dedicated
     bearers will be torn down with this anyways.
     ------------------------------------------------------------------------*/
   if (bearer_mgr_p->is_default)
   {
     pdn_mgr_p = bearer_mgr_p->pdn_mgr_ptr;
     if(!ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p))
     {
       return;
     }
     
     ds_eps_bearer_mgr_tear_down_pdn_connection(pdn_mgr_p, bearer_mgr_p);
     
     /*---------------------------------------------------------------------------- 
        Check if there is any other normal PDN active but of a different profile
        number. This is to take care of the scenario where there are two PDN's to the
        same profile but of different PDP types.
        For v6 instance type, send stop event to IP6 SM.
       ------------------------------------------------------------------------------*/
     if (ds_pdn_mgr_is_any_other_normal_pdn_diff_profile_num_active(pdn_mgr_p))
     {

       bearer_mgr_p->state = DS_BEARER_MGR_STATE_PEND_REDIAL;
       pdn_mgr_p->state =  DS_PDN_MGR_STATE_PEND_REDIAL;
       DS_MSG1(MSG_LEGACY_HIGH, "Moving PDN %d to pending redial.",
       	       pdn_mgr_p->index);
     }
   }
   return;
}/* ds_eps_pdn_mgr_apn_param_change_bearer_teardown */

/*===========================================================================
FUNCTION DS_EPS_PDN_MGR_APN_PARAM_CHANGE_CMD_HDLR

DESCRIPTION
  This function sends out a pdn disconnect request and reconnects with
  the new changed parameters.

PARAMETERS 
  uint8 prof_num
 
DEPENDENCIES
  None.

RETURN VALUE
  void 
 
SIDE EFFECTS
  None.
===========================================================================*/
void ds_eps_apn_param_change_cmd_hdlr
(
   uint8             prof_num
)
{
  ds_pdn_mgr_s      *pdn_mgr_p = NULL;
  ds_bearer_mgr_s   *bearer_mgr_p = NULL;
  uint8              pdn_index = 0;
  uint8              bearer_index = 0;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
 
  /*-------------------------------------------------------------------- 
    For every PDN context that is valid, check if it is the pdn context
    associated with the profile number, If it is then we need to teardown
    the bearers associated with the PDN context
   
    If APN param change for a particular PDN is set, don't process the
    command.
    --------------------------------------------------------------------*/
  for(pdn_index = 0; pdn_index < DS_3GPP_MAX_PDN_CONTEXT; pdn_index++)
  {
    pdn_mgr_p = ds_pdn_mgr_get_pdn_ptr_from_index(pdn_index);
    
    if(!ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p) ||
    	   pdn_mgr_p->lte_params == NULL ||
     	  pdn_mgr_p->lte_params->apn_param_chg_teardown == TRUE)
    {
      continue;
    }
  
    /*-------------------------------------------------------------------- 
      Only if the profile number matches and if the corresponding pdn
      context is in UP state, process the APN param change cmd request.
      -------------------------------------------------------------------*/
    if( pdn_mgr_p->pdp_profile_num == prof_num && 
    	   pdn_mgr_p->state == DS_PDN_MGR_STATE_UP )
    {
      DS_MSG1(MSG_LEGACY_HIGH, "PDN with profile number:%d found", prof_num);
  
      pdn_mgr_p->lte_params->apn_param_chg_teardown = TRUE;
      /*----------------------------------------------------------------------------- 
        Tear down all the bearers associated with the PDN context.
        -----------------------------------------------------------------------------*/
      for (bearer_index=0;bearer_index < DS_3GPP_MAX_BEARER_CONTEXT_PER_PDN;
            bearer_index++)
      {
        bearer_mgr_p = pdn_mgr_p->bearer_ptr_tbl[bearer_index];
        if (!ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p))
        {
          continue;
        }
  
        ds_eps_pdn_mgr_apn_param_change_bearer_teardown(bearer_mgr_p);
      }
    }
 	}
  return;
}/* ds_eps_apn_param_change_cmd_hdlr */

/*===========================================================================
FUNCTION DS_EPS_PDN_CNTXT_UPDATE_PARAM_FOR_NEW_IP_TYPE

DESCRIPTION
  This function is used to update the APM cause code based on the ip support
  provided

PARAMETERS 
  ind_payload  : APM Payload to be filled
  IP support   : APN type for the PDN being brought up
  Pdn_type_val : Network returns PDP_TYPE
 
DEPENDENCIES
  None.

RETURN VALUE
  None
 
SIDE EFFECTS
  None.
===========================================================================*/
void ds_eps_pdn_cntxt_update_param_for_new_ip_type
(
  ds_apn_ip_support_type_e     ip_support,
  ds_pdn_mgr_s                 *pdn_mgr_p,
  ds_pdn_apm_ind_pay_load      *ind_payload
)
{
  ds_3gpp_throt_rat_e_type         rat_type = DS_3GPP_THROT_RAT_GLOBAL;
                                    /* RAT on which throttling needs to
                                       be performed */
  ds_3gpp_throttle_sm_cb_data_type cb_data;
  ds_3gpp_pdn_throt_ident_info_type throt_ident_info;
/*-------------------------------------------------------------------------*/

  /*-----------------------------------------------------------------------
    Populate the Callback data that needs to be passed when 
    PDN Throttle Start, Stop or Get Timer value callback functions are 
    invoked
  -----------------------------------------------------------------------*/
  ds_3gpp_throt_fill_cb_data (&cb_data, NULL, NULL, (void*)pdn_mgr_p);

  if (ds_pdn_nv_manager_get_enable_pdn_throt_per_rat())
  {
    rat_type = DS_3GPP_THROT_RAT_LTE;
  }


  /*-----------------------------------------------------------------------
    Populate the throttle identifier info. This info will be used to 
    fetch the appropriate PDN Throttle SM Handle and then perform 
    PDN Throttling
   -----------------------------------------------------------------------*/
  ds_3gpp_pdn_throt_fill_ident_info_ex
  (
     &throt_ident_info, 
     (byte *)pdn_mgr_p->pdp_profile->context.apn,
     FALSE,
     rat_type,
     DS_3GPP_PDN_THROT_PROC_GLOBAL,
     DS_3GPP_PDN_THROT_TYPE_DEFAULT,
     DS_PROFILE_PDP_IPV4V6,
     FALSE,
     DS_NET_DOWN_REASON_NOT_SPECIFIED,
     pdn_mgr_p,
     &cb_data
  );

  switch(ip_support) 
  {
    case DS_IPV4_ONLY:
    {
      ds_pdn_apm_set_invalid_ipv6_addr(ind_payload,
                                   DS_NET_DOWN_REASON_IP_V4_ONLY_ALLOWED);

      throt_ident_info.pdp_type = DS_PROFILE_PDP_IPV6;
      throt_ident_info.cause_code = DS_NET_DOWN_REASON_IP_V4_ONLY_ALLOWED;

      ds_3gpp_pdn_throt_perform_reg_throttling (&throt_ident_info);
      break;
    }

    case DS_IPV4_SINGLE_BEARER:
    {
      ds_pdn_apm_set_invalid_ipv6_addr(ind_payload,
                                    DS_NET_DOWN_REASON_SINGLE_ADDR_BEARER_ONLY);
      break;
    }

    case DS_IPV6_ONLY:
    {
      ds_pdn_apm_set_invalid_ipv4_addr(ind_payload,
                                        DS_NET_DOWN_REASON_IP_V6_ONLY_ALLOWED);

      throt_ident_info.pdp_type = DS_PROFILE_PDP_IPV4;
      throt_ident_info.cause_code = DS_NET_DOWN_REASON_IP_V6_ONLY_ALLOWED;

      ds_3gpp_pdn_throt_perform_reg_throttling (&throt_ident_info);
      break;
    }

    case DS_IPV6_SINGLE_BEARER:
    {
      ds_pdn_apm_set_invalid_ipv4_addr(ind_payload,
                                    DS_NET_DOWN_REASON_SINGLE_ADDR_BEARER_ONLY);
      break;
    }

    default:
      DS_MSG0(MSG_LEGACY_ERROR,"Invalid combination , error Network sends IPV4V6 "
                        "with cause code after IPV4V6 is requested ");
      break;
  }

}/* ds_eps_pdn_cntxt_update_apm_cause_code */

/*===========================================================================
FUNCTION DS_EPS_PDN_CNTX_RESET_ATTACH_APN_IND_HDLR

DESCRIPTION
  This function is used to notify APM to reset Attach APN to Class 1 APN
  on successful PLMN blk cback from CM

PARAMETERS
  client_data_ptr: Current active PS subs

DEPENDENCIES
  None.

RETURN VALUE
  None
 
SIDE EFFECTS
  None.
===========================================================================*/
void ds_eps_pdn_cntxt_sys_sel_pref_no_err_cmd_hdlr
(
  void                           *client_data_ptr
)
{
  ds_pdn_apm_ind_pay_load         dsd_apm_pay_load;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset(&dsd_apm_pay_load, 0, sizeof(ds_pdn_apm_ind_pay_load));
  ds_pdn_apm_ind(DS_PDN_APM_RESET_ATTACH_APN_IND, &dsd_apm_pay_load);
  return;
}


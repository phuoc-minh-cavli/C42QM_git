/*===========================================================================
                      DS_PDN_REDIAL_HDLR.C
DESCRIPTION
This file defines PDN redial handler APIs.

Copyright (c) 2018 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/3gpp/pdnmgr/src/ds_pdn_redial_hdlr.c#3 $
  $DateTime: 2020/07/07 01:39:47 $$Author: pwbldsvc $

when           who    what, where, why
--------    ---    ----------------------------------------------------------
08/09/17    sd      First checkin
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "ds_pdn_defs.h"
#include "ds_pdn_redial_hdlr.h"
#include "ds_pdn_hdlr.h"
#include "ds_pdn_nv_manager.h"
#include "ds_pdn_auth_mgr.h"
#include "ds_pdn_apn_switch_mgr.h"
#include "ds_gprs_bearer_manager.h"
#include "ds_3gpp_dam.h"
#include "ds_3gpp_pdn_throttle_sm.h"

/*===========================================================================

                         LOCAL VARIABLES

===========================================================================*/
#define REASON_FROM_DS_CAUSE_CODE(code) (0x0000FFFF & code)

/* dual ip fallback emm or sm cause codes */
#define DS_PDN_DUAL_IP_FALLBACK_CAUSE_CODE_MAX (8)

/*---------------------------------------------------------------------------
  Dual IP fallback cause codes: Filled in EFS file.
  We will read this NV item on first network reject and set the read flag.
  In consecutive reject we will use the read value stored in array.
---------------------------------------------------------------------------*/
static uint16 dual_ip_fallback_codes[DS_PDN_DUAL_IP_FALLBACK_CAUSE_CODE_MAX] = {0};
static boolean fallback_cause_code_read = FALSE;

/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/
/*===========================================================================
FUNCTION DS_PDN_MGR_DUAL_IP_REDIAL_REQUIRED

DESCRIPTION 
  This function is used to determine whether to redialing based on dual IP cause
  codes. Cause codes are set in NV.

PARAMETERS
  down_reason   - Cause code(reason) returned by NW.
                  This is just the reason part of net_down_reason_type

DEPENDENCIES
  None.

RETURN VALUE
  TRUE: Redial required
  FALSE: Not a dual IP redial cause code
   
SIDE EFFECTS 
  None.
===========================================================================*/
boolean ds_pdn_mgr_dual_ip_redial_required
(
  ds_pdn_mgr_s                *pdn_mgr_p,
  uint16                       down_reason
)
{
  int cause_code_counter = 0;
  
  /*-------------------------------------------------------------------------
    Read the Dual IP fallback cause codes filled in NV.
    No redial if APN REJECT THROTTLING IS SET
  -------------------------------------------------------------------------*/
  if((ds_pdn_hdlr_get_current_network_mode() == SYS_SYS_MODE_LTE &&
      ds_eps_pdn_get_lte_attach_dual_ip_fb_status() == FALSE) ||
     (ds_3gpp_dam_get_pdn_blocked_status()))
  {
    return FALSE;
  }

  if(fallback_cause_code_read == FALSE)
  {
    if(ds_pdn_nv_manager_read_efs_nv(
                 "/nv/item_files/modem/data/3gpp/dual_ip_fallback_cause_codes", 
                      (unsigned char *) &dual_ip_fallback_codes,
                      (sizeof(uint16) * DS_PDN_DUAL_IP_FALLBACK_CAUSE_CODE_MAX))
                                                                     != NV_DONE_S)
    {
      DS_ERROR_LOG_0("Cannot read EFS item DS_3GPP_DUAL_IP_FALLBACK_CAUSE_CODES");
      /* write default values */
      dual_ip_fallback_codes[0] = 
        REASON_FROM_DS_CAUSE_CODE(DS_NET_DOWN_REASON_UNKNOWN_PDP); /* 28 */
      for(cause_code_counter = 1; 
          cause_code_counter < DS_PDN_DUAL_IP_FALLBACK_CAUSE_CODE_MAX;
          cause_code_counter++)
      {
        dual_ip_fallback_codes[cause_code_counter] = 
            REASON_FROM_DS_CAUSE_CODE(DS_NET_DOWN_REASON_MAX);
      }
    }
    fallback_cause_code_read = TRUE;
  }
  
  for(cause_code_counter = 0; 
      cause_code_counter < DS_PDN_DUAL_IP_FALLBACK_CAUSE_CODE_MAX;
      cause_code_counter++)
  {
      if(dual_ip_fallback_codes[cause_code_counter] 
         == down_reason)
    {
      DS_MSG2(MSG_LEGACY_HIGH,
      	       "ds_3gpp_pdn_cntxt_dual_ip_redial_required: Redial for"
              "cause code %d set at index %d",
              down_reason, cause_code_counter);
      return TRUE;
    }
  }
  return FALSE;
}/* ds_pdn_mgr_dual_ip_redial_required */

/*===========================================================================
FUNCTION DS_PDN_MGR_PERFORM_DUAL_IP_REDIAL_CHECK

DESCRIPTION 
  This function is used to determine if dual ip redial check is necessary

PARAMETERS
  IN
    pdn_mgr_p      - Pointer to the pdn context we are working on
    bearer_mgr_p  - Pointer to the bearer context we are working on
    down_reason        - Netdown reason for current call end.
                         (since we may redial for specific cause codes)

DEPENDENCIES
  None.

RETURN VALUE 
  boolean 
   
SIDE EFFECTS 
  None.
===========================================================================*/
boolean ds_pdn_mgr_perform_dual_ip_redial_check
(
  ds_pdn_mgr_s           *pdn_mgr_p,
  ds_bearer_mgr_s        *bearer_mgr_p
)
{
  boolean                   ret_val = FALSE;
  sys_sys_mode_e_type       mode = SYS_SYS_MODE_NONE;
#ifdef FEATURE_DATA_LTE
  boolean                   is_apn_throttled  = FALSE;
#endif /* FEATURE_DATA_LTE */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - -*/

  mode = ds_pdn_hdlr_get_current_network_mode();

  if((bearer_mgr_p->state == DS_BEARER_MGR_STATE_PEND_REDIAL) &&
     (pdn_mgr_p->pdp_profile->context.pdp_type == DS_PROFILE_PDP_IPV4V6) &&
     (bearer_mgr_p->call_was_connected == FALSE))
  {
#ifdef FEATURE_DATA_LTE
    if(mode == SYS_SYS_MODE_LTE)
    {
      is_apn_throttled = ds_3gpp_dam_is_apn_reject_sm_throttled(pdn_mgr_p);

      /*----------------------------------------------------------- 
        If apn_throttled and state is ATTACHED, then no redial
        -----------------------------------------------------------*/
      if(!is_apn_throttled && 
      	   (ds_eps_pdn_attach_sm_get_state() == DS_EPS_ATTACHED))
      {
        ret_val = TRUE;
      }

    }
    else
#endif
    {
      ret_val = TRUE;
    }

  }

  DS_MSG1(MSG_LEGACY_HIGH, "ds_3gpp_pdn_cntxt_perform_dual_ip_redial_check "
                           "returns:%d",ret_val);
  return ret_val;
}/* ds_pdn_mgr_perform_dual_ip_redial_check */


/*===========================================================================
FUNCTION DS_PDN_MGR_NEEDS_REDIAL

DESCRIPTION 
  This function is used to determine if redialing is required 

PARAMETERS
  IN
    pdn_mgr_p      - Pointer to the pdn context we are working on
    bearer_mgr_p  - Pointer to the bearer context we are working on
    down_reason        - Netdown reason for current call end.
                         (since we may redial for specific cause codes)
    call_was_connected - Whether the call was connected or not

DEPENDENCIES
  None.

RETURN VALUE
   DS_3GPP_NO_REDIAL if there is no need to redial
   DS_3GPP_REDIAL_DUAL_IP_FALLBACK in case we are redialing because of
   Dual IP fallback for UMTS
   DS_3GPP_REDIAL_AUTH_FALLBACK in case we are redialing because of
   authentication fallback
    
SIDE EFFECTS 
  None.
===========================================================================*/
ds_pdn_mgr_redial_e_type ds_pdn_mgr_needs_redial
(
  ds_pdn_mgr_s             *pdn_mgr_p,
  ds_bearer_mgr_s          *bearer_mgr_p,
  ds_net_down_reason_type   down_reason
)
{
  ds_profile_auth_enum_type         requested_auth_type;
  ds_pdn_network_params_info_type  *ppp_info_p = NULL;

  /*-------------------------------------------
       Redial occurs for default bearers only
   -------------------------------------------*/
  if(!bearer_mgr_p->is_default)
  {
    return DS_PDN_MGR_NO_REDIAL;
  }

  ppp_info_p = &(pdn_mgr_p->info);
  
  /*---------------------------------------------------------------------------- 
  Redial occurrs in one of the following cases  
  Case (a) UE requests auth type PAP and CHAP. 
           If NW sends call end because of AUTH failure, UE retries with
           PAP and NO Auth (initial try with CHAP) 

  Case (b) UE requests for dual ip type (V4V6) and NW rejects with error codes:
           28 (Unknown address or PDP type),
           30 (Activation rejected by GGSN, PDN-GW or S-GW)
           31 (Activation rejected, unspecified)
           33 (Requested service option not subscribed).
           In these cases, it is assumed that the NW does not understand
           IP type V4V6.
           If call mode is UMTS/LTE
           Redial if the original call was made in UMTS/LTE mode and if it is
           still the preferred mode           
           If RPM is enabled, we assume that apn is being throttled and
           we do not redial.   
     -------------------------------------------------------------------------------*/
  
  /*------------------------------------------- 
    Case (a)
   -------------------------------------------*/
  if((ppp_info_p->valid_fields & NET_CFG_PRM_AUTH_PREF_MASK)||
     (ppp_info_p->valid_fields & NET_CFG_PRM_AUTH_USERNAME_MASK)||
     (ppp_info_p->valid_fields & NET_CFG_PRM_AUTH_PASSWORD_MASK) )
  {
    requested_auth_type = (ds_profile_auth_enum_type) 
                                 pdn_mgr_p->info.auth_info.auth_type;
  }
  else
  {
    requested_auth_type = pdn_mgr_p->pdp_profile->auth.auth_type;
  } 
  if( TRUE == ds_pdn_auth_mgr_need_redial(pdn_mgr_p->pdp_profile->context.apn,
                                   requested_auth_type,
                                   down_reason,
                                   bearer_mgr_p->call_was_connected) )
  {
    DS_MSG0(MSG_LEGACY_HIGH, "in if");
    return DS_PDN_MGR_REDIAL_AUTH_FALLBACK;
  }

  /*------------------------------------------- 
    Case (b)
   -------------------------------------------*/
  if((ds_pdn_mgr_perform_dual_ip_redial_check(pdn_mgr_p,bearer_mgr_p)) &&
      ds_pdn_mgr_dual_ip_redial_required(pdn_mgr_p,
                               REASON_FROM_DS_CAUSE_CODE(down_reason)))
  {
    return DS_PDN_MGR_REDIAL_DUAL_IP_FALLBACK;
  }
 
  return DS_PDN_MGR_NO_REDIAL;
}/* ds_pdn_mgr_needs_redial */

/*===========================================================================
FUNCTION DS_PDN_MGR_SETUP_REDIAL

DESCRIPTION
  This function checks with the 3gpp bearers if redialing is required.
  If so, sets up the pdn context for redial.
  Otherwise, it cleans up the bearer and returns

PARAMETERS   
  pdn_mgr_p         - pdn context that we are workin on
  bearer_mgr_p     - bearer context
  down_reason        - reason for bearer deactivation
    
DEPENDENCIES
  Should always be called from within a critical section 
  ds3gpp_global_crit_sect

RETURN VALUE
  None  
 
SIDE EFFECTS
  None
===========================================================================*/
void ds_pdn_mgr_setup_redial
( 
  ds_pdn_mgr_s            *pdn_mgr_p, 
  ds_bearer_mgr_s         *bearer_mgr_p,
  ds_net_down_reason_type  down_reason
)
{
  ds_pdn_mgr_inst_type_s              *v4_inst = NULL;
  ds_pdn_mgr_inst_type_s              *v6_inst = NULL;
  ds_pdn_mgr_s                        *new_pdn_cntx_p;
  ds_pdn_mgr_redial_e_type             needs_redial_res;
  boolean                              redial_not_performed;
  boolean                              v4_in_use = FALSE;
  boolean                              v6_in_use = FALSE;
  boolean                              v4_throttled = FALSE;
  boolean                              v6_throttled = FALSE;
  boolean                              redial_v4;
  boolean                              redial_v6;
  ds_apn_ip_support_type_e           dbg_ip_type = DS_IP_TYPE_INVALID;
  /*------------------------------------------------------------------*/

  needs_redial_res = ds_pdn_mgr_needs_redial(pdn_mgr_p, bearer_mgr_p,
                                             down_reason);  
  DS_MSG2(MSG_LEGACY_HIGH, "Redial type %d for bearer %d",
  	       needs_redial_res, bearer_mgr_p->index);
  redial_not_performed = TRUE;
 
  switch (needs_redial_res)
  {
    case DS_PDN_MGR_REDIAL_DUAL_IP_FALLBACK:
      v4_inst = pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV4];
      v6_inst = pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV6];

      if(v4_inst ==  NULL || v6_inst == NULL)
      {
        DS_ERROR_LOG_0("NULL instance for dual IP.");
        break;
      }
      
      v4_in_use = v4_inst->in_use;
      v6_in_use = v6_inst->in_use;

      v4_throttled = v4_in_use &&
                     ds_3gpp_pdn_throt_is_apn_throttled(
                                      (byte *)pdn_mgr_p->pdp_profile->context.apn, 
                                      DS_PDN_INSTANCE_IPV4,
                                      pdn_mgr_p, TRUE);
      v6_throttled = v6_in_use &&
                     ds_3gpp_pdn_throt_is_apn_throttled(
                                      (byte *)pdn_mgr_p->pdp_profile->context.apn, 
                                      DS_PDN_INSTANCE_IPV6,
                                      pdn_mgr_p, TRUE);

      redial_v4 = v4_in_use && !v4_throttled;
      redial_v6 = v6_in_use && !v6_throttled;
    
      if (redial_v4 && redial_v6)
      {
         DS_MSG0(MSG_LEGACY_HIGH,  
         	       "ds_3gpp_pdn_cntxt_redial_hdlr: Dissociate ifaces");

        /*---------------------------------------------------------------------- 
            We use the recorded subs id from the old PDN Context
           -----------------------------------------------------------------------*/
        new_pdn_cntx_p = ds_pdn_mgr_allocate_new_pdn_context();
         if(!ds_pdn_mgr_validate_pdn_ptr(new_pdn_cntx_p))
         {
           break;
         }

         ds_pdn_mgr_setup_pdn_with_ds_instance(new_pdn_cntx_p, pdn_mgr_p,
         	                                     v6_inst);
         pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV6] = NULL;         

         /*------------------------------------------------------------------ 
           Update PDP type
          ------------------------------------------------------------------*/
         pdn_mgr_p->pdp_profile->context.pdp_type = DS_PROFILE_PDP_IPV4;
                   
         /*----------------------------------------------------------------- 
           Update PDN state
         ---------------------------------------------------------------*/
         pdn_mgr_p->state = DS_PDN_MGR_STATE_COMING_UP;
         new_pdn_cntx_p->state = DS_PDN_MGR_STATE_COMING_UP;

         ds_pdn_mgr_resend_pdn_conn_req(pdn_mgr_p, DS_PDN_INSTANCE_IPV4);
         ds_pdn_mgr_resend_pdn_conn_req(new_pdn_cntx_p, DS_PDN_INSTANCE_IPV6);

         redial_not_performed = FALSE;
      }
      else if(redial_v4)
      {
         /*---------------------------------------------------------- 
              Tear down v6 iface
            ---------------------------------------------------------*/
         DS_MSG0(MSG_LEGACY_HIGH,  
         	       "ds_3gpp_pdn_cntxt_redial_hdlr: Tearing down V6 iface");
         
         ds_pdn_mgr_tear_down_instance(pdn_mgr_p, DS_PDN_INSTANCE_IPV6,
         	                             down_reason);
         pdn_mgr_p->pdp_profile->context.pdp_type = DS_PROFILE_PDP_IPV4;
         (void)ds_apn_table_update_apn_table(pdn_mgr_p->pdp_profile->context.apn,
                                            DS_IPV4_SINGLE_BEARER, pdn_mgr_p); 

         ds_pdn_mgr_resend_pdn_conn_req(pdn_mgr_p, DS_PDN_INSTANCE_IPV4);

         redial_not_performed = FALSE;
      }
      else if(redial_v6)
      {
        /*---------------------------------------------------------- 
             Tear down v6 iface
           ---------------------------------------------------------*/
        DS_MSG0(MSG_LEGACY_HIGH,  
                "ds_3gpp_pdn_cntxt_redial_hdlr: Tearing down V4 iface");
        
        ds_pdn_mgr_tear_down_instance(pdn_mgr_p, DS_PDN_INSTANCE_IPV4,
                                      down_reason);
        pdn_mgr_p->pdp_profile->context.pdp_type = DS_PROFILE_PDP_IPV6;
        (void)ds_apn_table_update_apn_table(pdn_mgr_p->pdp_profile->context.apn,
                                           DS_IPV6_SINGLE_BEARER, pdn_mgr_p); 
        
        ds_pdn_mgr_resend_pdn_conn_req(pdn_mgr_p, DS_PDN_INSTANCE_IPV6);
        
        redial_not_performed = FALSE;
      } 
      break;
  
    case DS_PDN_MGR_REDIAL_AUTH_FALLBACK:
      v4_inst = pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV4];
      v6_inst = pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV6];

      if ((v4_inst && v4_inst->in_use) ||
          (v6_inst && v6_inst->in_use))
      {
#ifdef FEATURE_DATA_LTE          
        if(ds_pdn_hdlr_get_current_network_mode() != SYS_SYS_MODE_LTE ||
           ds_eps_pdn_attach_sm_get_state() == DS_EPS_ATTACHED)
#endif
        {
          /*------------------------------------------------------------------ 
            In all scenarios other than LTE attach, we redial explicitly.
            ------------------------------------------------------------------*/
          if(v4_inst)
          {
            ds_pdn_mgr_resend_pdn_conn_req(pdn_mgr_p, DS_PDN_INSTANCE_IPV4);
          }
          else
          {
            ds_pdn_mgr_resend_pdn_conn_req(pdn_mgr_p, DS_PDN_INSTANCE_IPV6);
          }
          redial_not_performed = FALSE;
        }
      }
      break;

    case DS_PDN_MGR_NO_REDIAL:  
    default:
      break;

  }

  if(redial_not_performed)
  {
    bearer_mgr_p->state = DS_BEARER_MGR_STATE_DOWN;
    pdn_mgr_p->net_down_reason = down_reason;
    if(pdn_mgr_p->nw_mode == SYS_SYS_MODE_LTE)
    {
      ds_eps_bearer_mgr_dereg_wmk(bearer_mgr_p);
    }
    else if(pdn_mgr_p->nw_mode == SYS_SYS_MODE_GSM)
    {
      ds_gprs_mgr_go_null_hdlr(bearer_mgr_p, pdn_mgr_p);
    }
  }
  else
  {
    /*-------------------------------------------------------------------------
      Since we are performing PDN redial we need to reset APN Switchng payload
      to ensure in case of PDN redial reject we propagate consistent info
      to both APM and AP
    -------------------------------------------------------------------------*/
    ds_pdn_apn_switch_mgr_reset_switching_action_payload();
  }

  if(ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p) && (pdn_mgr_p->nw_mode == SYS_SYS_MODE_LTE) &&
     ds_apn_table_get_apn_ip_support_info(pdn_mgr_p->pdp_profile->context.apn,
                                          &dbg_ip_type))
  {
      DS_ASSERT(dbg_ip_type != DS_IP_TYPE_INVALID);
  }
  
}/* ds_pdn_mgr_setup_redial */
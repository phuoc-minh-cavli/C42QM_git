/*===========================================================================
                      DS_PDN_MANAGER.C
DESCRIPTION
This file defines PDN manager APIs.

Copyright (c) 2019 - 2020 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/3gpp/pdnmgr/src/ds_pdn_manager.c#44 $
  $DateTime: 2023/07/13 23:13:32 $$Author: pwbldsvc $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
11/01/22    pk     Fix to send service req post barring only if have pending data 
04/01/22    pk     Fix to avoid MO data barring in TLB modes
07/23/19    ds     Fix to limit MTU between 1280 and IPv6 default MTU value.
08/09/17    sd     First checkin
===========================================================================*/

#include "ds_pdn_defs.h"
#include "ds_pdn_manager.h"
#include "ds_eps_pdn_manager.h"
#include "ds_eps_attach_hdlr.h"
#include "ds_flow.h"
#include "ds_pdn_apn_switch_mgr.h"
#include "ds_pdn_auth_mgr.h"
#include "ds_pdn_cp_data_hdlr.h"
#include "ds_pdn_cp_data_rohc.h"
#include "ds_pdn_call_hdlr.h"
#include "ds_pdn_apn_rate_ctrl.h"
#include "ds_pdn_pco_epco_hdlr.h"
#include "ds_pdn_redial_hdlr.h"
#include "ds_pdn_v6_hdlr.h"
#include "ds_pdn_pco.h"
#include "ds_pdn_profile_validator.h"
#include "ds_pdn_roaming_hdlr.h"
#include "ds_pdn_flow_manager.h"
#include "ds_bearer_manager.h"
#include "ds_bearer_flow_manager.h"
#include "ds_gprs_bearer_manager.h"
#include "ds_pdn_nv_manager.h"
#include "ds_fwk.h"
#include "ds_prof.h"
#include "ds3gmmgsdiif.h"
#include "ds_pdn_hdlr.h"
#include "ps_mem.h"
#include "ds_fwki.h"
#include "ds_sys_eventi.h"
#include "dssysmgr.h"
#include "ds_pdn_psm_hdlr.h"
#include "ds_3gpp_rmsm_atif.h"
#include "ds_pdn_rate_ctrl_common.h"
#include "ds_3gpp_pdn_limit_hdlr.h"
#include "ds_3gpp_srv_req_throt_config.h"
#include "ds_3gpp_dam.h"
#include "ds_3gpp_pdn_throt_config.h"
#include "ds_3gpp_srv_req_throt_sm.h"
#include "ds_3gpp_data_retry.h"
#include "ds_3gpp_network_override_hdlr.h"
#include "ps_addr_v6.h"
#include "ds_3gpp_pdn_throttle_sm.h"
#include "ds_pdn_call_hdlr.h"
#include "ds_3gpp_pdn_throt_spec.h"
#include "ps_ip6_addr.h"
#include "sys.h"
#include "ds_3gpp_rpm.h"
#include "dsmsgr.h"
#include "cfcm.h"
#include "ds_3gpp_rab_reestab_sm_hdlr.h"
#include "dsm_pool.h"
#include "ds_pdn_clat_hdlr.h"
#include "ds_crit_sect.h"
#include "lte_mac.h"

#ifdef FEATURE_MODE_TLB
#include "ds_eps_tlb.h"
#endif /* FEATURE_MODE_TLB */
#include "ds_3gpp_cc_sp_throt_sm.h"

static ds_pdn_mgr_s *ds_pdn_manager[DS_3GPP_MAX_PDN_CONTEXT] = {NULL};

/* Store the current active rat */
static sys_radio_access_tech_e_type ds_pdn_mgr_active_rat;

static ds_pdn_mgr_irat_info_s ds_pdn_mgr_irat_info;

static boolean ds_pdn_mgr_wwan_tech_suspend;

#ifdef FEATURE_NBIOT_NTN
static boolean ds_pdn_ntn_gnss_failure_status = FALSE;
#endif /* FEATURE_NBIOT_NTN */
boolean lpm_in_progress = FALSE;
boolean sub_not_avail_in_progress = FALSE;

emm_timer_t3448_status_ind_type t3448_timer_ind = {{0}};
uint32  active_barring_mask = 0;

static boolean pdn_mgr_enable_l2_rai = 0;
#define DS_PDN_APN_PARAM_CHG_INTERVAL 5000

/* To identify whether UE is in Level3 thermal mode  */ 
static boolean  ue_is_in_thermal_level3_mode = FALSE;

#ifdef FEATURE_NBIOT_NTN
static boolean is_ntn_restoration_in_progress =FALSE;
#endif /* FEATURE_NBIOT_NTN */

/*===========================================================================
FUNCTION DS_PDN_MGR_INIT

DESCRIPTION
  This function initialises the PDN manager module.

PARAMETERS
  None
    
DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_pdn_mgr_init
(
  void
)
{
  ds_fwk_pdn_operations_s_type reg_ptrs = {NULL};
  int16                        ps_errno;
  int16                        return_val = -1;
  static boolean               is_init = TRUE;

  reg_ptrs.bringup_f_ptr = ds_pdn_mgr_bringup_pdn;
  reg_ptrs.look_up_f_ptr = ds_pdn_mgr_lookup_pdn;
  reg_ptrs.teardown_f_ptr = ds_pdn_mgr_teardown_pdn;
  reg_ptrs.ioctl_f_ptr = ds_pdn_mgr_pdn_ioctl_cback;
  ds_fwk_register_mh(DS_FWK_MH_3GPP, &reg_ptrs);  

  return_val = ds_sys_ioctl_registeri_cback(DS_SYS_TECH_3GPP, 
  	                                         ds_pdn_mgr_sys_ioctl_cback, 
  	                                         &ps_errno);
  if(return_val < 0)
  {
    DS_MSG1(MSG_LEGACY_HIGH, 
    	       "SYS IOCTL CB registration for 3GPP failed: err = %d", 
    	       ps_errno);
  }

  t3448_timer_ind.status = EMM_TIMER_T3448_STATUS_UNKNOWN;

  ds_pdn_nv_manager_init();
  ds_pdn_apm_init(is_init);
  ds_pdn_auth_mgr_init(is_init);
  ds_pdn_roaming_hdlr_powerup_init();
  ds_bearer_mgr_init(is_init);
  ds_pdn_cp_data_power_up_init(is_init);
  ds_pdn_profile_validator_powerup_init(is_init);
  
  ds_3gpp_throt_sm_init();  
  ds_3gpp_pdn_throt_power_up_init();
  ds_3gpp_pdn_limit_powerup_init();
  ds_3gpp_dam_init();
#ifdef FEATURE_DATA_RPM
    ds_3gpp_rpm_init();
#endif /* FEATURE_DATA_RPM */
  ds_3gpp_srv_req_read_throttle_config_from_efs();
  ds_3gpp_pdn_throt_read_config_from_efs();
  ds_3gpp_cc_sp_throt_read_config_from_efs();
  ds_3gpp_srv_req_throt_init();
  ds_3gpp_cc_sp_init();
  ds_3gpp_pdn_throt_sm_per_apn_power_up_init();
  ds_pdn_ccmi_register_with_cfcm_events();
  ds_3gpp_rab_reestab_sm_hdlr_init();
  ds_pdn_mgr_register_mem_event_callbacks();
#ifdef FEATURE_NBIOT_NTN
   ds_pdn_read_ntn_info_from_efs();
 #endif /* FEATURE_NBIOT_NTN */
  if(!is_init)
  {
    ds_pdn_hdlr_cleanup_params();
  }
  /*................................................................
    To avoid multiple re-intialisation of globals in case of
    multiple subscription read
  ..................................................................*/
  is_init = FALSE;
  return;
} /*ds_pdn_mgr_init*/


/*===========================================================================
FUNCTION DS_PDN_MGR_UE_IS_IN_THERMAL_LEVEL3_MODE

DESCRIPTION
  This function returns whether UE is in Level 3 thermal emergency mode

PARAMETERS
  
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - if in Level 3 thermal emergecy mode
  FALSE - otherwise

SIDE EFFECTS
  None
===========================================================================*/
boolean  ds_pdn_mgr_ue_is_in_thermal_level3_mode()
{
  return ue_is_in_thermal_level3_mode;
}

/*===========================================================================
FUNCTION DS_PDN_MGR_VALIDATE_PDN_PTR

DESCRIPTION
  This function checks if the bearer ptr has been allocated dynamic memory

PARAMETERS
  pdn_mgr_p to be validated
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - if valid
  FALSE - otherwise

SIDE EFFECTS
  None
===========================================================================*/
boolean ds_pdn_mgr_validate_pdn_ptr
(
  ds_pdn_mgr_s *pdn_mgr_p
)
{
  if ( pdn_mgr_p != NULL && 
       pdn_mgr_p->cookie == PDN_MGR_VALID_COOKIE )
  {
    return TRUE;
  }

  return FALSE;
} /*ds_pdn_mgr_validate_pdn_ptr*/

/*===========================================================================
FUNCTION DS_PDN_MGR_GET_PDN_PTR_FROM_INDEX

DESCRIPTION
  This function fetches the PDN Manager Pointer corresponding to the
  given PDN index.
 
PARAMETERS
  Bearer Instance

DEPENDENCIES
  None.

RETURN VALUE
  Bearer Manager Pointer.

SIDE EFFECTS
  None.
===========================================================================*/
ds_pdn_mgr_s* ds_pdn_mgr_get_pdn_ptr_from_index
(
  uint8 instance
)
{
  return ((instance < DS_3GPP_MAX_PDN_CONTEXT) ? 
            ds_pdn_manager[instance] : NULL);
} /* ds_pdn_mgr_get_pdn_ptr_from_index */

/*===========================================================================
FUNCTION DS_PDN_MGR_GET_DEFAULT_BEARER

DESCRIPTION
  This function fetches the default bearer corresponding to the
  given PDN.
 
PARAMETERS
  PDN pointer

DEPENDENCIES
  None.

RETURN VALUE
  Bearer pointer.

SIDE EFFECTS
  None.
===========================================================================*/
ds_bearer_mgr_s* ds_pdn_mgr_get_default_bearer
(
  ds_pdn_mgr_s   *pdn_mgr_p
)
{ 
  if(ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p))
  {
    return pdn_mgr_p->bearer_ptr_tbl[DS_3GPP_DEFAULT_BEARER_INDEX];
  }
    
  return NULL;
} /* ds_pdn_mgr_get_default_bearer */

/*===========================================================================
FUNCTION DS_PDN_MGR_GET_DEFAULT_BEARER_STATE

DESCRIPTION
  This function fetches the default bearer state corresponding to the
  given PDN.
 
PARAMETERS
  PDN pointer

DEPENDENCIES
  None.

RETURN VALUE
  Bearer state.

SIDE EFFECTS
  None.
===========================================================================*/
ds_bearer_mgr_state_e ds_pdn_mgr_get_default_bearer_state
(
  ds_pdn_mgr_s   *pdn_mgr_p
)
{
  ds_bearer_mgr_s  *bearer_mgr_p = NULL;

  if(ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p))
  {
    bearer_mgr_p = pdn_mgr_p->bearer_ptr_tbl[DS_3GPP_DEFAULT_BEARER_INDEX];
    if(ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p))
    {
      return bearer_mgr_p->state;
    }      
  }
    
  return DS_BEARER_MGR_STATE_DOWN;
} /* ds_bearer_mgr_get_default_bearer_state */

/*===========================================================================
FUNCTION  DS_PDN_MGR_SET_ACTIVE_RAT

DESCRIPTION
  This function sets the current active rat received from NAS

PARAMETERS
  sys_radio_access_tech_e_type

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_mgr_set_active_rat
(
  sys_radio_access_tech_e_type  active_rat
)
{
  ds_pdn_mgr_active_rat = active_rat;
  DS_MSG1(MSG_LEGACY_HIGH,"Active rat %d",ds_pdn_mgr_active_rat);
} /* ds_pdn_mgr_set_active_rat */

/*===========================================================================
FUNCTION  DS_PDN_MGR_GET_ACTIVE_RAT

DESCRIPTION
  This function returns the active rat

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  sys_radio_access_tech_e_type

SIDE EFFECTS
  None.

===========================================================================*/
sys_radio_access_tech_e_type ds_pdn_mgr_get_active_rat
(
  void
)
{
  return ds_pdn_mgr_active_rat;
} /* ds_pdn_mgr_get_active_rat */

/*===========================================================================
FUNCTION DS_PDN_MGR_GET_DEF_PDN

DESCRIPTION
  This function is used to get to the default PDN context.

PARAMETERS
  None.
  
DEPENDENCIES
  None.

RETURN VALUE
  Pointer to the default PDN context irrespective of the state of the PDN. 
   
SIDE EFFECTS
  None
===========================================================================*/
ds_pdn_mgr_s* ds_pdn_mgr_get_def_pdn
(
  void
)
{
  int            index = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for(index = 0; index < DS_3GPP_MAX_PDN_CONTEXT; index++)
  {
    if( ds_pdn_mgr_validate_pdn_ptr(ds_pdn_manager[index]) &&
        ds_pdn_manager[index]->is_default == TRUE ) 
    {
      return ds_pdn_manager[index];
    }
  }

  return NULL;
}/*ds_pdn_mgr_get_def_pdn*/

/*===========================================================================
FUNCTION DS_PDN_MGR_GET_PDN_FROM_FWK_INDEX

DESCRIPTION
  This function is used to get to the PDN pointer corresponding to FWK index.

PARAMETERS
  FWK index.
  
DEPENDENCIES
  None.

RETURN VALUE
  Pointer to the PDN context.
   
SIDE EFFECTS
  None
===========================================================================*/
ds_pdn_mgr_s* ds_pdn_mgr_get_pdn_from_fwk_index
(
  ds_fwk_index_type fwk_index
)
{
  uint8            index = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(fwk_index == DS_FWK_INST_INVALID)
  {
    return NULL;
  }

  for(index = 0; index < DS_3GPP_MAX_PDN_CONTEXT; index++)
  {
    if( ds_pdn_mgr_validate_pdn_ptr(ds_pdn_manager[index]) &&
        ds_pdn_manager[index]->fw_index == fwk_index ) 
    {
      return ds_pdn_manager[index];
    }
  }

  return NULL;
}/*ds_pdn_mgr_get_pdn_from_fwk_index */

/*===========================================================================
FUNCTION DS_PDN_MGR_DETERMINE_PDP_TYPE

DESCRIPTION
  This functions returns the PDP type to be used based on the current mode
  and roaming conditions.

PARAMETERS
  context_info_p - ptr to profile context
  roaming_status - UE roaming status

DEPENDENCIES
  None.

RETURN VALUE
  ds_umts_pdp_type_enum_type - V4, V6 or V4V6

SIDE EFFECTS
  None.
===========================================================================*/
ds_profile_pdp_type_e ds_pdn_mgr_determine_pdp_type
(
  ds_profile_context_type  *context_info_p,
  boolean                   roaming_status
)
{
  sys_sys_mode_e_type     pref_mode = SYS_SYS_MODE_NO_SRV;
  ds_profile_pdp_type_e   pdp_type_to_use = DS_PROFILE_PDP_MAX;
  ds_profile_roaming_pdp_type_e  roaming_pdp_type =  DS_PROFILE_ROAMING_PDP_MAX;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( context_info_p == NULL )
  {
    return pdp_type_to_use;
  }

  pref_mode = ds_pdn_hdlr_get_current_network_mode();

  /* Set PDP Type to legacy PDP type */
  pdp_type_to_use = context_info_p->pdp_type;

  /*-------------------------------------------------------------------------
	Logic to return pdp_type
	- If UE is in Roaming
	  - If preferred mode is LTE
		- If LTE Roaming PDP type if set
		  return LTE Roaming PDP type
		- Else
		  return legacy PDP type
	  - Else if preferred mode is UMTS
		- If UMTS Roaming PDP type if set
		  return LTE Roaming PDP type
		- Else
		  return legacy PDP type
	- Else (Home)
	  - If Home override PDP type is set
		return Home override PDP type
	  - Else
		return legacy PDP type
  -------------------------------------------------------------------------*/
  if( roaming_status == TRUE )
  {
    switch( pref_mode )
    {
      case SYS_SYS_MODE_LTE:
        if( context_info_p->lte_roaming_pdp_type < DS_PROFILE_ROAMING_PDP_MAX )
        {
          roaming_pdp_type = context_info_p->lte_roaming_pdp_type;
        }
        break;

      case SYS_SYS_MODE_GSM:
        if( context_info_p->umts_roaming_pdp_type < DS_PROFILE_ROAMING_PDP_MAX )
        {
          roaming_pdp_type = context_info_p->umts_roaming_pdp_type;
        }
        break;

      default:
        break;
    }
  }
  else /* Home scenario i.e. roaming_status == FALSE */
  {
    if( context_info_p->override_home_pdp_type < DS_PROFILE_ROAMING_PDP_MAX )
    {
      roaming_pdp_type = context_info_p->override_home_pdp_type;
    }
  }

  switch(roaming_pdp_type)
  {
    case DS_PROFILE_ROAMING_PDP_IPV4:
      pdp_type_to_use = DS_PROFILE_PDP_IPV4;
      break;
    case DS_PROFILE_ROAMING_PDP_IPV6:
      pdp_type_to_use = DS_PROFILE_PDP_IPV6;
      break;
    case DS_PROFILE_ROAMING_PDP_IPV4V6:
      pdp_type_to_use = DS_PROFILE_PDP_IPV4V6;
      break;
    default:
      break;
   }
  DS_MSG3(MSG_LEGACY_HIGH, "roaming_status:%d pref_mode:%d pdp_type_to_use:%d",
                           roaming_status, pref_mode, pdp_type_to_use);
  return pdp_type_to_use;
} /* ds_pdn_mgr_determine_pdp_type */

/*===========================================================================
FUNCTION ds_pdn_mgr_update_null_override_apn

DESCRIPTION
This function is called to check if any override apn is present for NULL apn

PARAMETERS
  apn pointer to store the override apn for NULL apn

DEPENDENCIES
  None.

RETURN VALUE
  override apn length.

SIDE EFFECTS
  None.

===========================================================================*/

uint8 ds_pdn_mgr_update_null_override_apn
(
  char *apn
)
{
  char                        override_apn[DS_PROFILE_APN_STRING_LEN + 1];
  uint8                       req_apn_length = 0;
  sys_sys_mode_e_type         curr_mode = SYS_SYS_MODE_NONE;
/*..................................................................*/
  curr_mode = ds_pdn_hdlr_get_current_network_mode();
  memset(override_apn, '\0', DS_PROFILE_APN_STRING_LEN + 1);

  if(!ds_pdn_nv_manager_get_override_apn_per_rat(override_apn,curr_mode))
  {
      return req_apn_length;
  }
  
  req_apn_length = strlen(override_apn);
  if(req_apn_length > 0 && req_apn_length <= DS_PROFILE_APN_STRING_LEN)
  {
    strlcpy((char *)apn, (char *)override_apn, req_apn_length + 1);
    DS_MSG_SPRINTF_1(MSG_LEGACY_HIGH , 
                     "ds_pdn_mgr_update_null_override_apn apn : %s",
                      apn);
    return req_apn_length;
  }
  return 0;
} /* ds_pdn_mgr_update_null_override_apn */
/*===========================================================================
FUNCTION DS_PDN_MGR_PROCESS_PDP_PROFILE_INFO

DESCRIPTION
  This function Sets the PDP profile to be used for the call instance
  specified. It also gets the profile parameters for the call and stores
  it in the umtsps iface table.

  This function is called by the routing layer before to set this information.
  Subsequent to this, the call origination happens.

PARAMETERS
  *umtspsps_iface_ptr - ptr to umtsps iface instance
  pdp_profile_number - contains the PDP profile information

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_mgr_process_pdp_profile_info
(
  ds_pdn_mgr_s              *pdn_mgr_p,
  ds_apn_ip_support_type_e  *ip_support_p
)
{
  ds_profile_pdp_type_e      pdp_type_to_use;  
  boolean                     roaming_status = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  roaming_status = ds_pdn_roaming_get_status();

  /*-------------------------------------------------------------------------
    Update the PDP type in PDN context based on Home/Roaming conditions
  -------------------------------------------------------------------------*/
  pdp_type_to_use = ds_pdn_mgr_determine_pdp_type(
                      &pdn_mgr_p->pdp_profile->context,
                      roaming_status);
  pdn_mgr_p->pdp_profile->context.pdp_type = pdp_type_to_use;

  /*-------------------------------------------------------------------------
    Fill in the out param - ip_support_p based on the PDP type
  -------------------------------------------------------------------------*/
  if( (pdn_mgr_p->pdp_profile->context.pdp_type == DS_PROFILE_PDP_PPP) ||
      (pdn_mgr_p->pdp_profile->context.pdp_type == DS_PROFILE_PDP_IPV4) )
  {
    *(ip_support_p) = DS_IPV4_SINGLE_BEARER;
  }
  else if(pdn_mgr_p->pdp_profile->context.pdp_type == DS_PROFILE_PDP_IPV6)
  {
    *(ip_support_p) = DS_IPV6_SINGLE_BEARER;
  }
  else if(pdn_mgr_p->pdp_profile->context.pdp_type == DS_PROFILE_PDP_IPV4V6)
  {
    *(ip_support_p) = DS_IPV4V6;
  }
  else if (pdn_mgr_p->pdp_profile->context.pdp_type == DS_PROFILE_PDP_NON_IP)
  {
    *(ip_support_p) = DS_NON_IP;
  }
  else
  {
    DS_ERROR_LOG_1("Unknow PDP type in profile: =d",
                   pdn_mgr_p->pdp_profile->context.pdp_type);
    return FALSE;
  }

  if(strlen((char *)pdn_mgr_p->pdp_profile->context.apn) == 0)
  {
    ds_pdn_mgr_update_null_override_apn(pdn_mgr_p->pdp_profile->context.apn);
  }

  return TRUE;
} /* ds_pdn_mgr_process_pdp_profile_info */

/*===========================================================================
FUNCTION DS_PDN_MGR_GET_PROFILE_ID

DESCRIPTION
  This functions returns the profile ID for this APN.

PARAMETERS
  context_info_p - ptr to profile context

DEPENDENCIES
  None.

RETURN VALUE
  uint8 - Profile number

SIDE EFFECTS
  None.
===========================================================================*/
int8 ds_pdn_mgr_get_profile_id
(
  ds_profile_context_type  context
)
{
 ds_profile_ident_info   profile_ident={{0}};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  memset(profile_ident.apn, 0, DS_PROFILE_APN_STRING_LEN+1);
  memscpy(profile_ident.apn, DS_PROFILE_APN_STRING_LEN, context.apn, 
           strlen(context.apn));
  profile_ident.pdp_type = (ds_profile_pdp_type_e)context.pdp_type;

  return ds_profile_get_profile_id(profile_ident);
}

/*===========================================================================
FUNCTION DS_PDN_MGR_GET_PDN_FROM_PROFILE_ID

DESCRIPTION
  This function returns PDN corresponding to a profile ID

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  PDN pointer

SIDE EFFECTS
  None
===========================================================================*/
ds_pdn_mgr_s* ds_pdn_mgr_get_pdn_from_profile_id
( 
  uint8   profile_id 
)
{
  uint8        pdn_cntxt_inst = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for(pdn_cntxt_inst=0; pdn_cntxt_inst < DS_3GPP_MAX_PDN_CONTEXT;
      pdn_cntxt_inst++)
  {
    if ((ds_pdn_mgr_validate_pdn_ptr(ds_pdn_manager[pdn_cntxt_inst])) &&
        (ds_pdn_manager[pdn_cntxt_inst]->pdp_profile_num == profile_id))
    {
      return ds_pdn_manager[pdn_cntxt_inst];
    }
  }

  return NULL;
}/* ds_pdn_mgr_get_pdn_from_profile_id */

/*===========================================================================
FUNCTION DS_PDN_MGR_FREE_PDN_CONTEXT_DYN_MEM

DESCRIPTION
  This function frees dynamic memory for pdn context. 

PARAMETERS
  PDN reference.

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_mgr_free_pdn_context_dyn_mem
(
  ds_pdn_mgr_s  *pdn_mgr_p
)
{
  if(pdn_mgr_p)
  {    
    
    PS_TIMER_FREE_HANDLE(pdn_mgr_p->pdn_inactivity_timer);        
    PS_TIMER_FREE_HANDLE(pdn_mgr_p->polling_timer);    

    if(pdn_mgr_p->pdp_profile)
    {
      DS_SYSTEM_HEAP_MEM_FREE(pdn_mgr_p->pdp_profile);
    }

    if(pdn_mgr_p->lte_params)
    {
      PS_TIMER_FREE_HANDLE(pdn_mgr_p->lte_params->apn_param_chg_timer);
      DS_SYSTEM_HEAP_MEM_FREE(pdn_mgr_p->lte_params);
    }

    if(pdn_mgr_p->gsm_params)
    {
      DS_SYSTEM_HEAP_MEM_FREE(pdn_mgr_p->gsm_params);
    }

    if(pdn_mgr_p->clat_sm_inst)
    {
      ds_pdn_free_clat_sm(pdn_mgr_p);
    }

    // Reset the cookie
    pdn_mgr_p->cookie = 0;

    ds_pdn_manager[pdn_mgr_p->index] = NULL;
    DS_SYSTEM_HEAP_MEM_FREE(pdn_mgr_p);
  }
}

/*===========================================================================
FUNCTION DS_PDN_MGR_IS_ANY_PDN_GOING_DOWN

DESCRIPTION
  This function returns whether there is a pdn in going down

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  boolean - whether there is a pdn context in going_down state

SIDE EFFECTS
  None
===========================================================================*/
boolean ds_pdn_mgr_is_any_pdn_going_down
( 
   void 
)
{
  uint8             pdn_cntxt_inst = 0;
  ds_pdn_mgr_s     *pdn_mgr_p = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for(pdn_cntxt_inst=0; pdn_cntxt_inst < DS_3GPP_MAX_PDN_CONTEXT;
      pdn_cntxt_inst++)
  {
    pdn_mgr_p = ds_pdn_manager[pdn_cntxt_inst];    
    if ((ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p)) &&
        ((pdn_mgr_p->state == DS_PDN_MGR_STATE_GOING_DOWN) ||
         (pdn_mgr_p->state == DS_PDN_MGR_STATE_UP &&
          pdn_mgr_p->bearer_ptr_tbl[DS_3GPP_DEFAULT_BEARER_INDEX] != NULL &&
          pdn_mgr_p->bearer_ptr_tbl[DS_3GPP_DEFAULT_BEARER_INDEX]->state == 
                                                     DS_BEARER_MGR_STATE_DOWN)))
    {
      DS_MSG1(MSG_LEGACY_HIGH, "PDN instance %d is going down.",
                               pdn_cntxt_inst);
      return TRUE;
    }
  }

  return FALSE;
}/* ds_pdn_mgr_is_any_pdn_going_down */

/*===========================================================================
FUNCTION ds_pdn_mgr_is_any_pdn_up

DESCRIPTION
  This function loops over all PDN contexts to see if there is atleast one more 
  PDN  in UP state. 
 
PARAMETERS


DEPENDENCIES
  None.

RETURN VALUE  
  TRUE, if there is a PDN in UP state.
  FALSE, otherwise

SIDE EFFECTS  
  None.
===========================================================================*/
boolean ds_pdn_mgr_is_any_pdn_up()
{
  ds_pdn_mgr_s       *local_pdn_p = NULL;
  uint8               index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for(index = 0; index < DS_3GPP_MAX_PDN_CONTEXT; index++)
  {
    local_pdn_p = ds_pdn_manager[index];
    if( !ds_pdn_mgr_validate_pdn_ptr(local_pdn_p) )
    {
      continue;
    }
    else
    {
      if(local_pdn_p->state == DS_PDN_MGR_STATE_UP)
      {
        return TRUE;
      }
    }
  } /* Loop over all PDN contexts*/

  return FALSE;
} /* ds_pdn_mgr_is_any_other_pdn_up */

/*===========================================================================
FUNCTION ds_pdn_mgr_is_any_pdn_active

DESCRIPTION
  This function loops over all PDN contexts to see if there is atleast one more 
  PDN  in valid 
 
PARAMETERS


DEPENDENCIES
  None.

RETURN VALUE  
  TRUE, if there is a PDN in UP state.
  FALSE, otherwise

SIDE EFFECTS  
  None.
===========================================================================*/
boolean ds_pdn_mgr_is_any_pdn_active()
{
  ds_pdn_mgr_s       *local_pdn_p = NULL;
  uint8               index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for(index = 0; index < DS_3GPP_MAX_PDN_CONTEXT; index++)
  {
    local_pdn_p = ds_pdn_manager[index];
    if( ds_pdn_mgr_validate_pdn_ptr(local_pdn_p) )
    {
      return TRUE;
    }
  } /* Loop over all PDN contexts*/

  return FALSE;
} /* ds_pdn_mgr_is_any_pdn_active */

/*===========================================================================
FUNCTION ds_pdn_mgr_is_any_other_pdn_up

DESCRIPTION
  This function loops over all PDN contexts to see if there is atleast one more 
  PDN (apart from the one passed in) in UP state. Whether the PDN passed in is 
  UP or not, does not matter. 
 
PARAMETERS
  PDN Context

DEPENDENCIES
  None.

RETURN VALUE  
  TRUE, if there is a PDN in UP state, other than the PDN passed in.
  FALSE, otherwise

SIDE EFFECTS  
  None.
===========================================================================*/
boolean ds_pdn_mgr_is_any_other_pdn_up
(
  ds_pdn_mgr_s  *pdn_mgr_p
)
{
  ds_pdn_mgr_s       *local_pdn_p = NULL;
  uint8               index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for(index = 0; index < DS_3GPP_MAX_PDN_CONTEXT; index++)
  {
    local_pdn_p = ds_pdn_manager[index];
    if( (NULL == local_pdn_p ) ||
    	   (pdn_mgr_p == local_pdn_p) )
    {
      continue;
    }
    else
    {
      if(local_pdn_p->state == DS_PDN_MGR_STATE_UP)
      {
        return TRUE;
      }
    }
  } /* Loop over all PDN contexts*/

  return FALSE;
} /* ds_pdn_mgr_is_any_other_pdn_up */

/*===========================================================================
FUNCTION ds_pdn_mgr_is_any_other_pdn_active

DESCRIPTION
  This function loops over all PDN contexts to see if there is atleast one more 
  PDN (apart from the one passed in) not in DOWN state.
 
PARAMETERS
  PDN Context

DEPENDENCIES
  None.

RETURN VALUE  
  TRUE, if there is a PDN active, other than the PDN passed in.
  FALSE, otherwise

SIDE EFFECTS  
  None.
===========================================================================*/
boolean ds_pdn_mgr_is_any_other_pdn_active
(
  ds_pdn_mgr_s  *pdn_mgr_p
)
{
  ds_pdn_mgr_s       *local_pdn_p = NULL;
  uint8               index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for(index = 0; index < DS_3GPP_MAX_PDN_CONTEXT; index++)
  {
    local_pdn_p = ds_pdn_manager[index];
    if( (NULL == local_pdn_p ) ||
    	   (pdn_mgr_p == local_pdn_p) )
    {
      /* This is either not in use, or is the same PDN as passed in */
      continue;
    }
    else
    {
      DS_MSG2(MSG_LEGACY_HIGH,"PDN %d is in %d state",
                        local_pdn_p->index, 
                        local_pdn_p->state );
      if(local_pdn_p->state != DS_PDN_MGR_STATE_DOWN)
      {
        return TRUE;
      }
    }
  } /* Loop over all PDN contexts*/

  return FALSE;
} /* ds_pdn_mgr_is_any_other_pdn_active */

/*===========================================================================
FUNCTION ds_pdn_mgr_is_any_other_mo_ex_pdn_active

DESCRIPTION
  This function loops over all PDN contexts to see if there is atleast one more 
  PDN (apart from the one passed in) is mo ex capable.
 
PARAMETERS
  PDN Context

DEPENDENCIES
  None.

RETURN VALUE  
  TRUE, if there is a PDN active, other than the PDN passed in.
  FALSE, otherwise

SIDE EFFECTS  
  None.
===========================================================================*/
boolean ds_pdn_mgr_is_any_other_mo_ex_pdn_active
(
  ds_pdn_mgr_s*  pdn_mgr_p
)
{
  ds_pdn_mgr_s       *local_pdn_p = NULL;
  uint8               index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for(index = 0; index < DS_3GPP_MAX_PDN_CONTEXT; index++)
  {
    local_pdn_p = ds_pdn_manager[index];
    if( !ds_pdn_mgr_validate_pdn_ptr(local_pdn_p ) || 
        local_pdn_p == pdn_mgr_p )
    {
      /* This is either not in use, or is the same PDN as passed in */
      continue;
    }
    else
    {
      DS_MSG2(MSG_LEGACY_HIGH,"PDN %d is in %d state",
                        local_pdn_p->index, 
                        local_pdn_p->state );
      if(local_pdn_p->state != DS_PDN_MGR_STATE_DOWN && 
         local_pdn_p->lte_params->mo_exception_data)
      {
        return TRUE;
      }
    }
  } /* Loop over all PDN contexts*/

  return FALSE;
}
/*===========================================================================
FUNCTION ds_pdn_mgr_is_any_other_normal_pdn_diff_profile_num_active

DESCRIPTION
  This function loops over all PDN contexts to see if there is atleast one more 
  PDN (apart from the one passed in) and not the same profile number in UP state.
  Whether the PDN passed in is UP or not, does not matter. 
 
PARAMETERS
  PDN Context

DEPENDENCIES
  None.

RETURN VALUE  
  TRUE, if there is an active PDN other than the PDN passed in.
  FALSE, otherwise

SIDE EFFECTS  
===========================================================================*/
boolean ds_pdn_mgr_is_any_other_normal_pdn_diff_profile_num_active
(
  ds_pdn_mgr_s *pdn_mgr_p
)
{
  ds_pdn_mgr_s       *local_pdn_mgr_p = NULL;
  uint8               pdn_instance;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for(pdn_instance = 0;
      pdn_instance < DS_3GPP_MAX_PDN_CONTEXT;
      pdn_instance++)
  {
    local_pdn_mgr_p = ds_pdn_manager[pdn_instance];
    if( !ds_pdn_mgr_validate_pdn_ptr(local_pdn_mgr_p) ||
    	   (local_pdn_mgr_p == pdn_mgr_p) )
    {
      /* This is either not in use, or is the same PDN as passed in */
      continue;
    }
    else
    {
      DS_MSG2(MSG_LEGACY_HIGH, "PDN %d is in %d state",
              local_pdn_mgr_p->index, local_pdn_mgr_p->state );

      /* This is a PDN context different from the one passed into the func and
         a different profile number*/
      if((local_pdn_mgr_p->state == DS_PDN_MGR_STATE_UP) &&
         (local_pdn_mgr_p->pdp_profile_num != pdn_mgr_p->pdp_profile_num))
      {
        return TRUE;
      }
    }
  } /* Loop over all PDN contexts*/

  return FALSE;
} /* dsPdnContext_is_any_other_normal_pdn_active */

/*===========================================================================
FUNCTION DS_3GPP_PDN_MGR_HANDLE_BEARER_DOWN_IND

DESCRIPTION
  We can reset the Attach SM only if following conditions are met:
  1. Bearer down indication is received for the default bearer
  2. PDN context corresponding to that bearer is the only active PDN context.
  3. Attach SM is in DS_EPS_PROCESS_GET_PDN_CONN_IND or DS_EPS_ATTACHED state.

PARAMETERS
  Bearer context
  Net down reason

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_pdn_mgr_handle_bearer_down_ind
(
  ds_bearer_mgr_s              *bearer_mgr_p,
  ds_net_down_reason_type       down_reason
)
{
  ds_pdn_mgr_s         *pdn_mgr_p           = NULL;
  ds_eps_attach_state_e eps_attach_state    = DS_EPS_ATTACH_STATE_NULL;
  ds_pdn_apm_ext_error_code apm_error_code;
  /*-----------------------------------------------------------------------*/

  DS_MSG0(MSG_LEGACY_HIGH, "ds_3gpp_pdn_mgr_handle_bearer_down_ind ");

  pdn_mgr_p = bearer_mgr_p->pdn_mgr_ptr;

  if (FALSE == bearer_mgr_p->is_default)
  {
    /* No special handling for dedicated bearers */
    return;
  }

  eps_attach_state = ds_eps_pdn_attach_sm_get_state();

  if(TRUE == ds_pdn_mgr_is_any_other_pdn_active(pdn_mgr_p) || 
     (TRUE == ds_eps_attach_hdlr_get_nw_attach_wo_pdn_support() &&
      eps_attach_state == DS_EPS_ATTACHED))
  {
    /* We have other PDN connections active. */
    return;
  }

  /*-------------------------------------------------------------------
    Notify APM module for LTE EMM DETACH OR ATTACH REJECT
  --------------------------------------------------------------------*/
  apm_error_code = ds_pdn_apn_switch_mgr_send_ind_to_apm();

  DS_MSG1(MSG_LEGACY_HIGH,"Posted Indication to APM, Err code: %d", 
                   apm_error_code);

  switch (eps_attach_state)
  {
    case DS_EPS_ATTACHED:
      ds_eps_reset_attach_sm_params();
      break;
      
    case DS_EPS_PROCESSING_GET_PDN_CONN_IND:
      ds_eps_reset_attach_sm_params();
      ds_eps_attach_sm_set_previous_attach_status(TRUE, down_reason);
      break;
      
    default:
     break;
  }

  /*--------------------------------------------------------------------- 
    Reset network override handler when throttle behaviour is to
    BLOCK PLMN
  -----------------------------------------------------------------------*/
  if(ds_3gpp_data_retry_is_enabled())
  {
    ds_3gpp_network_override_hdlr_reset(FALSE);
  }
} /* ds_3gpp_pdn_cntxt_handle_bearer_down_ind */

/*===========================================================================
FUNCTION DS_PDN_MGR_FREE_PDN_CONTEXT

DESCRIPTION
  This function frees dynamic memory for pdn context. 

PARAMETERS
  PDN reference.

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_mgr_free_pdn_context
(
  ds_pdn_mgr_s  *pdn_mgr_p
)
{
  uint8            bearer_index = 0;
  uint8            num_active_bearers = 0;
  ds_bearer_mgr_s *bearer_p = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(pdn_mgr_p)
  {
    for(bearer_index = 0; 
        bearer_index < DS_3GPP_MAX_BEARER_CONTEXT_PER_PDN; 
        bearer_index++ )
    {
      if (pdn_mgr_p->bearer_ptr_tbl[bearer_index] != NULL)
      {     
        bearer_p = pdn_mgr_p->bearer_ptr_tbl[bearer_index];
        num_active_bearers++;
      }     
    }
    DS_ASSERT((num_active_bearers <= 1));

    if(ds_bearer_mgr_validate_bearer_ptr(bearer_p))
    {
      if(pdn_mgr_p->nw_mode == SYS_SYS_MODE_LTE)
      {
        ds_pdn_cp_data_reset_release_assistance_ind(bearer_p->index);
        ds_3gpp_pdn_mgr_handle_bearer_down_ind(bearer_p, 
        	                                       pdn_mgr_p->net_down_reason);
      }

     /*---------------------------------------------------------------------
       Release Bearer Flow Manager associated with the bearer.
     ---------------------------------------------------------------------*/
      ds_pdn_flow_manager_remove_bearer_from_pdn_flow(
                                         pdn_mgr_p, bearer_p);        
      ds_bearer_flow_manager_release_flow(bearer_p);

      if(strlen((char*)pdn_mgr_p->pdp_profile->context.apn) == 0)
      {
        //ds_3gpp_call_hdlr_reset_resolved_apn();
      }

      ds_bearer_mgr_free_dyn_mem(bearer_p);
    }

    ds_flow_mgr_release_flow_manager(pdn_mgr_p);      
    ds_pdn_mgr_free_pdn_context_dyn_mem(pdn_mgr_p);
  }

  return;
}

/*===========================================================================
FUNCTION DS_PDN_MGR_PROCESS_PDN_INACTIVITY_CMD

DESCRIPTION
  This function is called while processing the command enqueued to DS when
  the PDN inactivity timer expires. This command cleans up the PDN context
  irrespective of whether it is the last one standing or otherwise.

PARAMETERS
  pdn_cntx_p - Pointer to the PDN context to be torn down

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_mgr_process_pdn_inactivity_cmd
(
  ds_pdn_mgr_s* pdn_mgr_p
)
{
  ds_bearer_mgr_s         *bearer_mgr_p;
  uint8                    bearer_index;
  boolean                  data_transfer_occured = FALSE;
  uint8                    ignore_inactivity_timer = FALSE;
  int64                    remaining_timer_val = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(!ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p))
  {
    return;
  }

 /*-----------------------------------------------------------------------------
   This timer will be re-started in pooling timer expiry handler, Check if the 
   remaining timer value is non-zero if so do not process inactivity timer expiry 
   specific function
  -----------------------------------------------------------------------------*/
  remaining_timer_val = ps_timer_remaining(pdn_mgr_p->pdn_inactivity_timer);
  if (remaining_timer_val > 0) 
  {
    DS_ERROR_LOG_0("PDN inactivity timer re-started, returning !!!");
    return;
  }

  if(pdn_mgr_p->state == DS_PDN_MGR_STATE_UP)
  {
    /*--------------------------------------------------------------------------
      Loop over all bearers and make them update the data transfer statictics
      This is needed to find out if any data has flown since the last polling
      timer expiry
    --------------------------------------------------------------------------*/
    for(bearer_index = 0;
        bearer_index < DS_3GPP_MAX_BEARER_CONTEXT_PER_PDN;
        bearer_index++)
    {
      bearer_mgr_p = pdn_mgr_p->bearer_ptr_tbl[bearer_index];
      if(bearer_mgr_p == NULL)
      {
        continue;
      }
      
      if(ds_eps_bearer_mgr_get_data_activity(bearer_mgr_p, pdn_mgr_p) == TRUE)
      {
        /*-----------------------------------------------------------------------
          Mark that data transfer has happened in this interval between polling
          timer expiry and inactivity timer expiry.
        -----------------------------------------------------------------------*/
        data_transfer_occured = TRUE;
      }
    }//loop over all bearers

    //See if this is the last PDN and if so check the inactivity timer.
    if (FALSE == ds_pdn_mgr_is_any_other_pdn_up(pdn_mgr_p))
    {
      ignore_inactivity_timer = ds_pdn_nv_manager_get_ignore_inactivity_timer();              
    }

    if ((TRUE == data_transfer_occured) || (TRUE == ignore_inactivity_timer))
    {
      DS_MSG1(MSG_LEGACY_HIGH, "Restarting inactivity timer: %ds",
              pdn_mgr_p->pdp_profile->inactivity_timer_val);
      /*-------------------------------------------------------------------------
        Restart the PDN inactivity timer. We need not restart the polling timer.
        We can let it run freely.
      -------------------------------------------------------------------------*/
      ps_timer_start(pdn_mgr_p->pdn_inactivity_timer, 
                          (pdn_mgr_p->pdp_profile->inactivity_timer_val)*1000);
    }
    else
    {
      DS_MSG1(MSG_LEGACY_HIGH, "Tearing Down PDN %d", pdn_mgr_p->index);     
      pdn_mgr_p->state = DS_PDN_MGR_STATE_GOING_DOWN;
      ds_pdn_mgr_teardown_instances_for_pdn(pdn_mgr_p, 
      	        DS_NET_DOWN_REASON_INTERNAL_PDN_INACTIVITY_TIMER_EXPIRED);
    } // data transfer occured check
  } /* if PDN context is in UP state */

  return;
} /* ds_pdn_mgr_process_pdn_inactivity_cmd */

/*===========================================================================
FUNCTION DS_PDN_MGR_PROCESS_POLLING_TMR_EXP_CMD

DESCRIPTION
  This function is called while processing the command enqueued to DS when
  the polling timer expires. This function queries the lower layers to figure
  out if there has been any data activity since the last polling timer expiry

  If there has been data activity, we restart the PDN inactivity timer
  Otherwise, we let it run.

PARAMETERS
  pdn_mgr_p - Pointer to the PDN 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_pdn_mgr_process_polling_tmr_exp_cmd
(
  ds_pdn_mgr_s* pdn_mgr_p
)
{
  uint8                 bearer_index;
  ds_bearer_mgr_s      *bearer_mgr_p;
  boolean               data_transfer_occured = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(!ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p))
  {
    return;
  }

  if(pdn_mgr_p->state == DS_PDN_MGR_STATE_UP)
  {
    /*--------------------------------------------------------------------------
      Loop over all bearers and make them update the data transfer statictics
    --------------------------------------------------------------------------*/
    for(bearer_index = 0;
        bearer_index < DS_3GPP_MAX_BEARER_CONTEXT_PER_PDN;
        bearer_index++)
    {
      bearer_mgr_p = pdn_mgr_p->bearer_ptr_tbl[bearer_index];
      if(bearer_mgr_p == NULL)
      {
        continue;
      }
      
      if(ds_eps_bearer_mgr_get_data_activity(bearer_mgr_p, pdn_mgr_p) == TRUE)
      {
        /*----------------------------------------------------------------------
          Mark that data transfer has happened in this polling interval
        ----------------------------------------------------------------------*/
        data_transfer_occured = TRUE;
      }
    }//loop over all bearers

    if(data_transfer_occured == TRUE)
    {
      DS_MSG1(MSG_LEGACY_HIGH,"Data transfer occured. restart inactivity timer: %ds",
                        pdn_mgr_p->pdp_profile->inactivity_timer_val);
      /*------------------------------------------------------------------------
        Restart the PDN inactivity timer
      ------------------------------------------------------------------------*/
      ps_timer_start(pdn_mgr_p->pdn_inactivity_timer, 
                          (pdn_mgr_p->pdp_profile->inactivity_timer_val)*1000);
    }

    /*------------------------------------------------------------------------
      Start the polling timer for the next interval
    ------------------------------------------------------------------------*/
    ps_timer_start(pdn_mgr_p->polling_timer, DS_3GPP_POLLING_INTERVAL);
  }//pdn context in UP state

  return;
} /* ds_3gpp_pdn_cntx_process_polling_tmr_exp_cmd */

/*===========================================================================
FUNCTION DS_PDN_MGR_INACTIVITY_TIMER_CB

DESCRIPTION
  This function is called when the PDN inactivity timer expires. This call
  back function simply posts a command to DS. DS then processes the command
  and takes suitable action

PARAMETERS
  unsigned long - Call back data, points to the PDN context

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_mgr_inactivity_timer_cb
(
  void* callback_data
)
{
  ds_command_type  *cmd_ptr   = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
  DS_MSG0(MSG_LEGACY_HIGH,"ds_pdn_mgr_inactivity_timer_cb");

  /*
   * This function is executed in the timer call back.
   * So all we do here is to put a command to DS and exit
   */
  cmd_ptr = ds_allocate_cmd_buf(DS_CMD_PAYLOAD_SIZE_ZERO);
  if (cmd_ptr == NULL)
  {
    ASSERT(0);
    return;
  }

  cmd_ptr->hdr.cmd_id = DS_CMD_PDN_INACTIVITY_TMR_EXP;
  cmd_ptr->cmd_payload_ptr = callback_data ;

  ds_put_cmd(cmd_ptr);
} /* ds_pdn_mgr_inactivity_timer_cb */

/*===========================================================================
FUNCTION DS_PDN_MGR_POLLING_TIMER_CB

DESCRIPTION
  This function is called when the PDN inactivity timer expires. This call
  back function simply posts a command to DS. DS then processes the command
  and takes suitable action

PARAMETERS
  unsigned long - Call back data, points to the PDN context

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_mgr_polling_timer_cb(void* callback_data)
{
  ds_command_type  *cmd_ptr   = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_MSG0(MSG_LEGACY_HIGH,"ds_pdn_mgr_polling_timer_cb");

  /*
   * This function is executed in the timer call back.
   * So all we do here is to put a command to DS and exit
   */
  cmd_ptr = ds_allocate_cmd_buf(DS_CMD_PAYLOAD_SIZE_ZERO);
  if (cmd_ptr == NULL)
  {
    ASSERT(0);
    return;
  }

  cmd_ptr->hdr.cmd_id = DS_CMD_PDN_POLLING_TMR_EXP;
  cmd_ptr->cmd_payload_ptr = callback_data ;

  ds_put_cmd(cmd_ptr);
} /* ds_pdn_mgr_polling_timer_cb */

/*===========================================================================
FUNCTION DS_PDN_MGR_APN_PARAM_CHG_TIMER_CB

DESCRIPTION
  This function is called when the APN_PARAM_CHGtimer expires. This call
  back function simply posts a command to DS. DS then processes the command
  and takes suitable action

PARAMETERS
  unsigned long - Call back data, points to the PDN context

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_pdn_mgr_apn_param_chg_timer_cb
(
  void* callback_data
)
{
  ds_command_type  *cmd_ptr   = NULL;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
  DS_MSG0(MSG_LEGACY_HIGH,"apn_param_chg_timer_cb");

  /*
   * This function is executed in the timer call back.
   * So all we do here is to put a command to DS and exit
   */
  cmd_ptr = ds_allocate_cmd_buf(DS_CMD_PAYLOAD_SIZE_ZERO);
  if (cmd_ptr == NULL)
  {
    ASSERT(0);
    return;
  }

  cmd_ptr->hdr.cmd_id = DS_CMD_PDN_APN_PARAM_CHG_TMR_EXP;
  cmd_ptr->cmd_payload_ptr = callback_data ;

  ds_put_cmd(cmd_ptr);
 
} /* ds_pdn_mgr_apn_param_chg_timer_cb */

/*===========================================================================
FUNCTION DS_PDN_MGR_ALLOCATE_NEW_INSTANCE

DESCRIPTION
  This function allocates instance type. 

PARAMETERS
  PDN ptr
  Instance type

DEPENDENCIES
  None.

RETURN VALUE  
  Pointer to newly allocated instance or NULL if allocation fails.

SIDE EFFECTS
  None.
===========================================================================*/
ds_pdn_mgr_inst_type_s* ds_pdn_mgr_allocate_new_instance
(
 ds_pdn_mgr_s               *pdn_mgr_p,
 ds_pdn_mgr_instance_type_e  inst_type
)
{
  ds_pdn_mgr_inst_type_s  *inst_p = NULL;
  ip6_sm_config_type      *ip6_config = NULL;
  ds_fwk_s_type           *fw_inst_p = NULL;

  /* Allocate FWK instance, as it can happen there is no client but call for
        that IP type is brought up. Applicable for atatch and additional pdn both. */     
  pdn_mgr_p->fw_index = 
    ds_fwk_alloc_attach_inst(pdn_mgr_p,
              ds_pdn_mgr_convert_pdn_inst_to_fw_ip_type(inst_type), 
                            pdn_mgr_p->is_default);  
  
  fw_inst_p = ds_fwk_get_inst_ptr_from_index(pdn_mgr_p->fw_index);
  if(!fw_inst_p)
  {
    DS_ERROR_FATAL("NULL FWK ptr !");
    return NULL;
  }

  DS_SYSTEM_HEAP_MEM_ALLOC(inst_p, sizeof(ds_pdn_mgr_inst_type_s),
  	                       ds_pdn_mgr_inst_type_s*);
  pdn_mgr_p->inst_ptr[inst_type] = inst_p;
   
  if(inst_p != NULL)
  {
    memset(inst_p, 0, sizeof(ds_pdn_mgr_inst_type_s));
    inst_p->type = inst_type;

    if(inst_type != DS_PDN_INSTANCE_NON_IP)
    {      
      DS_SYSTEM_HEAP_MEM_ALLOC(inst_p->flow, sizeof(ds_flow_list_s),
		 	                      ds_flow_list_s*);
      if(!inst_p->flow)
      {
        DS_ASSERT(0);
        return NULL;
      }
      
      inst_p->flow->default_flow_ptr = ds_flow_allocate_context(inst_p);
      list_init(&inst_p->flow->sec_flow_list);
    }
    
    /*-----------------------------------------------------------------
        Initialize the link list of SIP server IP addresses and domain names.
      -----------------------------------------------------------------*/
    if(inst_type == DS_PDN_INSTANCE_IPV4)
    {
     	memset (&fw_inst_p->v4_net_info.sip_serv_addr, 0, sizeof (list_type));
      list_init(&fw_inst_p->v4_net_info.sip_serv_addr);
    }
    else if(inst_type == DS_PDN_INSTANCE_IPV6)
    {
     	memset (&fw_inst_p->v6_net_info.sip_serv_addr, 0, sizeof (list_type));
      list_init(&fw_inst_p->v6_net_info.sip_serv_addr);

      // Allocate memory for default v6 address
      fw_inst_p->v6_net_info.ipv6_addrs[DS_DEFAULT_V6_INDEX] = 
  	          (ds_fwk_v6_addr_s_type*)ps_mem_get_buf(PS_MEM_IPV6_ADDR_TYPE);
	  
      if(fw_inst_p->v6_net_info.ipv6_addrs[DS_DEFAULT_V6_INDEX] == NULL)
      {
        ds_flow_delete(inst_p->flow->default_flow_ptr);
        DS_SYSTEM_HEAP_MEM_FREE(inst_p);
        pdn_mgr_p->inst_ptr[inst_type] = NULL;
        return NULL;
      }

      memset(fw_inst_p->v6_net_info.ipv6_addrs[DS_DEFAULT_V6_INDEX], 0, 
             sizeof(ds_fwk_v6_addr_s_type));  
      DS_SYSTEM_HEAP_MEM_ALLOC( inst_p->ds_pdn_v6_sm_cb,
	  	                        sizeof(ds_pdn_v6_sm_cb_type),
                                ds_pdn_v6_sm_cb_type*);
      if(inst_p->ds_pdn_v6_sm_cb == NULL)
      {
        ds_flow_delete(inst_p->flow->default_flow_ptr);
        DS_SYSTEM_HEAP_MEM_FREE(inst_p);
        pdn_mgr_p->inst_ptr[inst_type] = NULL;
        return NULL;
      }
  
      DS_SYSTEM_HEAP_MEM_ALLOC(inst_p->ip6_sm, sizeof(ip6_sm_type),
	  	                       ip6_sm_type*);
      if(inst_p->ip6_sm == NULL)
      {
        DS_SYSTEM_HEAP_MEM_FREE(inst_p->ds_pdn_v6_sm_cb);
        ds_flow_delete(inst_p->flow->default_flow_ptr);
        DS_SYSTEM_HEAP_MEM_FREE(inst_p);
        pdn_mgr_p->inst_ptr[inst_type] = NULL;
        return NULL;
      }
      
      memset(inst_p->ds_pdn_v6_sm_cb, 0 ,sizeof(ds_pdn_v6_sm_cb_type));
      memset(inst_p->ip6_sm, 0 ,sizeof(ip6_sm_type));
      
      inst_p->ip6_sm->this_sm_ptr = inst_p->ip6_sm;
      ip6_config = ds_pdn_nv_manager_get_ipv6_sm_config_info();

      ip6_sm_create(inst_p->ip6_sm, pdn_mgr_p, ip6_config);      

      ds_pdn_v6_sm_create(inst_p->ds_pdn_v6_sm_cb, pdn_mgr_p, inst_p->ip6_sm);         
    }
  }  
  
  return inst_p;
}

/*===========================================================================
FUNCTION DS_PDN_MGR_ALLOCATE_NEW_PDN

DESCRIPTION
  This function allocates unused pdn context. 

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE  
  Pointer to newly allocated pdn context or NULL if there is none left.

SIDE EFFECTS
  None.

===========================================================================*/
ds_pdn_mgr_s* ds_pdn_mgr_allocate_new_pdn_context
(
  void
)
{
  uint8          index;
  ds_pdn_mgr_s  *pdn_mgr_p = NULL;
  ds_local_err_val_e_type local_err = DS_LOCAL_ERR_VAL_NONE;
  uint32 pdn_index = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for(index = 0; index < DS_3GPP_MAX_PDN_CONTEXT; index++)
  {    
    if(!ds_pdn_mgr_validate_pdn_ptr(ds_pdn_manager[index]))
    {
      DS_SYSTEM_HEAP_MEM_ALLOC(ds_pdn_manager[index],sizeof(ds_pdn_mgr_s),
	  	                       ds_pdn_mgr_s*);
      pdn_mgr_p = ds_pdn_manager[index];
      if(NULL == pdn_mgr_p)
      {
        local_err = DS_LOCAL_ERR_VAL_0;
        break;
      }
      
      memset(pdn_mgr_p, 0, sizeof(ds_pdn_mgr_s));
      pdn_mgr_p->cookie = PDN_MGR_VALID_COOKIE;
      pdn_mgr_p->index = index;
      pdn_mgr_p->fw_index = PDN_MGR_INVALID_FW_INDEX;
    
      DS_SYSTEM_HEAP_MEM_ALLOC(pdn_mgr_p->pdp_profile, sizeof(ds_profile_type_s),
                               ds_profile_type_s*);
      if( NULL == pdn_mgr_p->pdp_profile )
      {
        local_err = DS_LOCAL_ERR_VAL_1;
        break;
      }
      memset(pdn_mgr_p->pdp_profile, 0, sizeof(ds_profile_type_s));

      /*------------------------------------------------------------------------
        Define the PDN inactivity timer. Specify the call back function and the
        call back data.
      ------------------------------------------------------------------------*/
      pdn_mgr_p->pdn_inactivity_timer =
           ps_timer_alloc(ds_pdn_mgr_inactivity_timer_cb,
                          (void*)(pdn_mgr_p));

      /*------------------------------------------------------------------------
        Define the polling timer. Specify the call back function and the
        call back data.
      ------------------------------------------------------------------------*/
       pdn_mgr_p->polling_timer =
            ps_timer_alloc(ds_pdn_mgr_polling_timer_cb,
                           (void*)(pdn_mgr_p));

      /* LTE specific parameters */
      if(ds_pdn_hdlr_get_current_network_mode() == SYS_SYS_MODE_LTE)
      {
        pdn_mgr_p->nw_mode = SYS_SYS_MODE_LTE;
        DS_SYSTEM_HEAP_MEM_ALLOC(pdn_mgr_p->lte_params,
			                     sizeof(ds_pdn_mgr_lte_pdn_params),
                                 ds_pdn_mgr_lte_pdn_params*);
        if(pdn_mgr_p->lte_params == NULL)
        {
          local_err = DS_LOCAL_ERR_VAL_2;
          break;
        }

        memset(pdn_mgr_p->lte_params, 0, sizeof(ds_pdn_mgr_lte_pdn_params));
        /*---------------------------------------------------------------------
              Define the apn timer. Specify the call back function and the
              call back data.
             ----------------------------------------------------------------------*/
        pdn_index = pdn_mgr_p->index;
        pdn_mgr_p->lte_params->apn_param_chg_timer =
                    ps_timer_alloc(ds_pdn_mgr_apn_param_chg_timer_cb,
                                   (void *)pdn_index);
      }
      /* GSM specific parameters */
      else if(ds_pdn_hdlr_get_current_network_mode() == SYS_SYS_MODE_GSM)
      {
        pdn_mgr_p->nw_mode = SYS_SYS_MODE_GSM;
        DS_SYSTEM_HEAP_MEM_ALLOC(pdn_mgr_p->gsm_params,
			                     sizeof(ds_pdn_mgr_gsm_pdn_params),
                                 ds_pdn_mgr_gsm_pdn_params*);
        if(pdn_mgr_p->gsm_params == NULL)
        {
          local_err = DS_LOCAL_ERR_VAL_3;
          break;
        }
        
        memset(pdn_mgr_p->gsm_params,0,sizeof(ds_pdn_mgr_gsm_pdn_params));
      }
      
      break;
    }
  } //end for loop over pdn_cntx_index

  /*--------------------------------------------------------------------------- 
   If all pdn contexts have been allocated, print out a msg saying its not the
   heap allocation that is failing
  ---------------------------------------------------------------------------*/
  if (index == DS_3GPP_MAX_PDN_CONTEXT)
  {
    DS_ERROR_LOG_0("MAX PDN contexts have been allocated");
    return NULL;
  }

  if(DS_LOCAL_ERR_VAL_NONE != local_err)
 	{
 	  DS_MSG1(MSG_LEGACY_ERROR, "Failed to allocate PDN. Error : %d", local_err);
 	  ds_pdn_mgr_free_pdn_context_dyn_mem(pdn_mgr_p);
 	  pdn_mgr_p = NULL;
  }
  
  return pdn_mgr_p;
}

/*===========================================================================
FUNCTION      DS_PDN_MGR_REJECT_NULL_APN

DESCRIPTION
  This function checks to see if null apn is supported
  Null apn is supported only if the PDN is attach PDN
 
PARAMETERS

DEPENDENCIES

RETURN VALUE
  True -  If the apn in the pdn context is NULL
              and the PDN is not attach PDN
  False - Otherwise
 
SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_pdn_mgr_reject_null_apn
(
  ds_pdn_mgr_s         *pdn_mgr_p,
  char                 *apn_name  
)
{
  boolean                 ret_val = FALSE;
  sys_sys_mode_e_type     network_mode = SYS_SYS_MODE_NONE;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
 /*------------------------------------------------------------------------- 
   Return False
   If the network mode is not LTE
   If attach SM is not Attached (This will take care of the attach PDN)
   If Rel 13 is supported
  -------------------------------------------------------------------------*/
  network_mode = ds_pdn_hdlr_get_current_network_mode();

  if ((network_mode != SYS_SYS_MODE_LTE) || 
      (ds_eps_pdn_attach_sm_get_state() != DS_EPS_ATTACHED) ||
       ds_pdn_nv_manager_is_rel_13_or_above_supported())
  {
    ret_val = FALSE;
  }
  /*------------------------------------------------------------------------ 
    If we have reached this stage, It is an additional PDN
    Check if the apn in the PDN context is NULL, If NULL apn return TRUE
   ------------------------------------------------------------------------*/
  else if (strlen(apn_name) == 0)
  {
    ret_val = TRUE;
  }

  DS_MSG1(MSG_LEGACY_HIGH, "ds_pdn_mgr_reject_null_apn returns %d", ret_val);
  return ret_val;
}

/*===========================================================================
FUNCTION      DS_PDN_MGR_GET_COMPANION_INST

DESCRIPTION
  Fetches the other instance from the current instance pointer.

PARAMETERS
  inst_p       - Pointer to the current instance.  

DEPENDENCIES
  Called in DS TASK context.

RETURN VALUE
  companion_inst_p - Pointer to the other instance.  

SIDE EFFECTS
  None.
===========================================================================*/
ds_pdn_mgr_inst_type_s* ds_pdn_mgr_get_companion_inst
(
  ds_pdn_mgr_s              *pdn_mgr_p,
  ds_pdn_mgr_instance_type_e inst_type
)
{
  if(inst_type == DS_PDN_INSTANCE_IPV4)
  {
    return pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV6];
  }
  else if(inst_type == DS_PDN_INSTANCE_IPV6)
  {
    return pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV4];
  }

  return NULL;
}

// undef if required later
#if 0
/*===========================================================================
FUNCTION DS_PDN_MGR_GET_V4_DNS_ADDR

DESCRIPTION
  This function returns the DNS addresses stored in the instance.

PARAMETERS
  inst_p: ptr to interface control block on which to operate on.
  pri_dns_ptr
  sec_dns_ptr

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_pdn_mgr_get_v4_dns_addr
(
  ds_pdn_mgr_inst_type_s      *inst_p,
  ps_ip_addr_type             *pri_dns_ptr,
  ps_ip_addr_type             *sec_dns_ptr
)
{
  pri_dns_ptr->addr.v4 = inst_p->v4.primary_dns.ps_s_addr;
  sec_dns_ptr->addr.v4  = inst_p->v4.secondary_dns.ps_s_addr;
  
  /*-------------------------------------------------------------------------
    Set appropriate ip address types.
  -------------------------------------------------------------------------*/
  if(pri_dns_ptr->addr.v4 == 0)
  {
    pri_dns_ptr->type = IP_ADDR_INVALID;
  }
  else
  {
    pri_dns_ptr->type = IPV4_ADDR;
  }
  
  if(sec_dns_ptr->addr.v4 == 0)
  {
    sec_dns_ptr->type = IP_ADDR_INVALID;
  }
  else
  {
    sec_dns_ptr->type = IPV4_ADDR;
  }

  return;
}/* ds_pdn_mgr_get_v4_dns_addr */

/*===========================================================================
FUNCTION DS_PDN_MGR_GET_V6_DNS_ADDR

DESCRIPTION
  This function returns the v6 DNS addresses stored in the instance.

PARAMETERS
  inst_p: ptr to interface control block on which to operate on.
  pri_dns_ptr
  sec_dns_ptr

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_pdn_mgr_get_v6_dns_addr
(
  ds_pdn_mgr_inst_type_s      *inst_p,
  ps_ip_addr_type             *pri_dns_ptr,
  ps_ip_addr_type             *sec_dns_ptr
)
{
  pri_dns_ptr->addr.v6.ps_s6_addr64[0] =
                        inst_p->v6.primary_dns.ps_s6_addr64[0];
  pri_dns_ptr->addr.v6.ps_s6_addr64[1] =
                        inst_p->v6.primary_dns.ps_s6_addr64[1];
  
  sec_dns_ptr->addr.v6.ps_s6_addr64[0] =
                      inst_p->v6.secondary_dns.ps_s6_addr64[0];
  sec_dns_ptr->addr.v6.ps_s6_addr64[1] =
                      inst_p->v6.secondary_dns.ps_s6_addr64[1];
  
  /*-------------------------------------------------------------------------
    Set appropriate ip address types.
  -------------------------------------------------------------------------*/
  if( (pri_dns_ptr->addr.v6.ps_s6_addr64[0] == 0) &&
      (pri_dns_ptr->addr.v6.ps_s6_addr64[1] == 0))
  {
    pri_dns_ptr->type = IP_ADDR_INVALID;
  }
  else
  {
    pri_dns_ptr->type = IPV6_ADDR;
  }
  
  if( (sec_dns_ptr->addr.v6.ps_s6_addr64[0] == 0) &&
      (sec_dns_ptr->addr.v6.ps_s6_addr64[1] == 0))
  {
    sec_dns_ptr->type  = IP_ADDR_INVALID;
  }
  else
  {
    sec_dns_ptr->type  = IPV6_ADDR;
  }

  return;
}/* ds_pdn_mgr_get_v4_dns_addr */
#endif

/*===========================================================================
FUNCTION DS_PDN_MGR_GET_SIP_SERV_ADDR

DESCRIPTION
  This function returns the list of sip server addresses stored in the instance.

PARAMETERS
  tinst_p: ptr to interface control block on which to operate on.
  sip_serv_addr_ptr_array: pointer to the SIP server addresses array.
  count : as input value it contains the count of the SIP server
          addresses asked for and as a return value it contains the
          actual number of addresses returned.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_pdn_mgr_get_sip_serv_addr
(
  ds_pdn_mgr_s                *pdn_mgr_p,
  ds_pdn_mgr_instance_type_e   inst_type,
  ps_ip_addr_type             *sip_serv_addr_ptr_array,
  uint8                       *count
)
{
  ds_fwk_sip_serv_addr_type   *curr_sip_serv_addr = NULL;
  uint32                       temp_count = 0;  
  ds_fwk_s_type               *fw_inst_p = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  if(!ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p))
  {
    return;
  }

  fw_inst_p = ds_fwk_get_inst_ptr_from_index(pdn_mgr_p->fw_index);
  if(!fw_inst_p)
  {
    DS_ERROR_FATAL("NULL FWK ptr !");
    return;
  }

  /*-------------------------------------------------------------------------
    Traverse the list and copy the SIP server addresses in the array.
  -------------------------------------------------------------------------*/
  if(inst_type == DS_PDN_INSTANCE_IPV4)
  {
    curr_sip_serv_addr = list_peek_front(
    	                        &fw_inst_p->v4_net_info.sip_serv_addr);
  }
  else if(inst_type == DS_PDN_INSTANCE_IPV6)
  {
    curr_sip_serv_addr = list_peek_front(
    	                        &fw_inst_p->v6_net_info.sip_serv_addr);
  }
  	
  while (temp_count < *count && curr_sip_serv_addr != NULL)
  {
    sip_serv_addr_ptr_array[temp_count].type =
      curr_sip_serv_addr->sip_addr.type;

    if (curr_sip_serv_addr->sip_addr.type == IPV4_ADDR)
    {
      sip_serv_addr_ptr_array[temp_count].addr.v4.ps_s_addr =
        curr_sip_serv_addr->sip_addr.addr.v4.ps_s_addr;
      curr_sip_serv_addr = 
               list_peek_next(&fw_inst_p->v4_net_info.sip_serv_addr,
                              &curr_sip_serv_addr->link);
    }
    else if (curr_sip_serv_addr->sip_addr.type == IPV6_ADDR)
    {
      sip_serv_addr_ptr_array[temp_count].addr.v6.ps_s6_addr64[0] =
        curr_sip_serv_addr->sip_addr.addr.v6.ps_s6_addr64[0];
      sip_serv_addr_ptr_array[temp_count].addr.v6.ps_s6_addr64[1] =
        curr_sip_serv_addr->sip_addr.addr.v6.ps_s6_addr64[1];
      curr_sip_serv_addr = 
               list_peek_next(&fw_inst_p->v6_net_info.sip_serv_addr,
                              &curr_sip_serv_addr->link);
    }
    else
    {
      DS_ERROR_LOG_0("Invalid address type !");
    }

    temp_count++;
  }

  *count = temp_count;
  return;
} /* ds_pdn_mgr_get_sip_serv_addr() */

/*===========================================================================
FUNCTION DS_PDN_MGR_FREE_SIP_ADDR_LIST

DESCRIPTION
  This function frees the SIP server address list.

PARAMETERS
  sip_addr_list : pointer to the link list containing the SIP addresses.
  is_destroy    : boolean to indicate if the list needs to be destroyed.

RETURN VALUE

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_pdn_mgr_free_sip_addr_list
(
  list_type  *sip_addr_list,
  boolean     is_destroy
)
{
  ds_fwk_sip_serv_addr_type   *curr_sip_serv_addr;
  ds_fwk_sip_serv_addr_type   *next_sip_serv_addr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
   Free the list.
  -------------------------------------------------------------------------*/
  curr_sip_serv_addr = list_peek_front(sip_addr_list);
  while (curr_sip_serv_addr != NULL)
  {
    next_sip_serv_addr = list_peek_next(sip_addr_list,
                                        &curr_sip_serv_addr->link);

    list_pop_item(sip_addr_list, &(curr_sip_serv_addr->link));    
    DS_SYSTEM_HEAP_MEM_FREE(curr_sip_serv_addr);
    
    curr_sip_serv_addr = next_sip_serv_addr;
  } /* while SIP servers in the list exist */

  if(is_destroy)
  {
    list_destroy(sip_addr_list);
  }

  return;
} /*ds_pdn_mgr_free_sip_addr_list*/

/*===========================================================================
FUNCTION      ds_pdn_mgr_cleanup_inst

DESCRIPTION
  Moves the current Iface and other iface to DOWN state. Cleans the APN table 
  entry. Also frees the PDN context.

PARAMETERS
  iface_p       - Pointer to the iface that is to be brought down.  
  down_reason   - Reason to cleanup 

DEPENDENCIES
  Called in DS TASK context.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_pdn_mgr_cleanup_inst
(
  ds_pdn_mgr_s               *pdn_mgr_p,
  ds_pdn_mgr_instance_type_e  inst_type
)
{
  ds_pdn_mgr_inst_type_s  *inst_p = NULL;
  ds_pdn_event_info_u_type event_info;
  ds_fwk_s_type           *fw_inst_p = NULL;
  
  DS_MSG2(MSG_LEGACY_HIGH, "Posting down on instance %d, PDN %d", 
          inst_type, pdn_mgr_p->index);
  
  inst_p = pdn_mgr_p->inst_ptr[inst_type];
  memset(&event_info, 0, sizeof(ds_pdn_event_info_u_type));

  if(inst_p)
  {        
    DS_ENTER_CRIT_SECTION(&global_ps_crit_section);
     
    fw_inst_p = ds_fwk_get_inst_ptr_from_index(pdn_mgr_p->fw_index);
    if(!fw_inst_p)
    {
      DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      DS_ERROR_FATAL("NULL FWK ptr !");
      return;
    }

    if(inst_type != DS_PDN_INSTANCE_NON_IP)
    {
      ds_flow_delete(inst_p->flow->default_flow_ptr);      
      list_destroy(&inst_p->flow->sec_flow_list);
      DS_SYSTEM_HEAP_MEM_FREE(inst_p->flow);
    }

    if(inst_type == DS_PDN_INSTANCE_IPV4)
    {
      ds_pdn_mgr_free_sip_addr_list(&fw_inst_p->v4_net_info.sip_serv_addr, TRUE);
    }
    else if(inst_type == DS_PDN_INSTANCE_IPV6)
    {
     	ds_pdn_mgr_free_sip_addr_list(&fw_inst_p->v6_net_info.sip_serv_addr, TRUE);

      ps_delete_all_v6_prefixes(fw_inst_p);
      // Free memory for default v6 address.
      PS_MEM_FREE(fw_inst_p->v6_net_info.ipv6_addrs[DS_DEFAULT_V6_INDEX]);
      fw_inst_p->v6_net_info.ipv6_addrs[DS_DEFAULT_V6_INDEX] = NULL;

      DS_SYSTEM_HEAP_MEM_FREE(inst_p->ds_pdn_v6_sm_cb);

      ip6_sm_destroy(inst_p->ip6_sm);
      DS_SYSTEM_HEAP_MEM_FREE(inst_p->ip6_sm);
    }
    
    DS_SYSTEM_HEAP_MEM_FREE(inst_p);
    pdn_mgr_p->inst_ptr[inst_type] = NULL;

    event_info.pdn_down_info.addr_family = 
                  ds_pdn_mgr_convert_pdn_inst_to_fw_ip_type(inst_type);
    event_info.pdn_down_info.netdown_reason = pdn_mgr_p->net_down_reason;

    // generate PDN down event for this instance to framework
    ds_fwk_notify_events( pdn_mgr_p->fw_index,
                       ds_pdn_mgr_convert_pdn_inst_to_fw_ip_type(inst_type),
                       PDN_DOWN_EV,
                       event_info);                                              
                       
    DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  }

  return;
}

/*===========================================================================
FUNCTION      ds_pdn_mgr_cleanup_pdn

DESCRIPTION
  Moves the current Iface and other iface to DOWN state. Cleans the APN table 
  entry. Also frees the PDN context.

PARAMETERS
  iface_p       - Pointer to the iface that is to be brought down.  
  down_reason   - Reason to cleanup 

DEPENDENCIES
  Called in DS TASK context.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_pdn_mgr_cleanup_pdn
(
  ds_pdn_mgr_s               *pdn_mgr_p,
  ds_pdn_mgr_instance_type_e  inst_type
)
{
  ds_pdn_mgr_inst_type_s  *inst_p = NULL;
  ds_pdn_mgr_inst_type_s  *comp_inst_p = NULL;
  uint8                    index = 0;
  int                      bearer_inst = -1;
  ds_bearer_mgr_s         *bearer_p = NULL;
  ds_3gpp_mo_exp_activity_status_ind_type   data_activity_start;
/*-------------------------------------------------------------------------*/

  DS_MSG2(MSG_LEGACY_HIGH, "Clean up PDN %d with down_reason %x", 
          pdn_mgr_p->index, pdn_mgr_p->net_down_reason);

  memset(&data_activity_start, 0, 
         sizeof(ds_3gpp_mo_exp_activity_status_ind_type));
  
  if(inst_type < DS_PDN_INSTANCE_MAX)
  {
    inst_p = pdn_mgr_p->inst_ptr[inst_type];  
    if(inst_p)
    {
      comp_inst_p = ds_pdn_mgr_get_companion_inst(pdn_mgr_p,inst_type);
      ds_pdn_mgr_cleanup_inst(pdn_mgr_p,inst_type);
    }
    
    /*----------------------------------------------------------------------- 
      If the companion iface is in use and the anchor iface is throttled
      then do not post an iface down indication on the companion iface
    -----------------------------------------------------------------------*/
    if( (comp_inst_p != NULL) && (comp_inst_p->in_use) )
    {
      DS_MSG2(MSG_LEGACY_HIGH,
      	       "PDN: %d not being torn down, companion_inst:%d in use",
              pdn_mgr_p->index, comp_inst_p->type);
      return;
    }
    
    if(comp_inst_p != NULL)
    {            
      ds_pdn_mgr_cleanup_inst(pdn_mgr_p,comp_inst_p->type);
    }
  }
  else if (inst_type == DS_PDN_INSTANCE_MAX)
  {
    ds_pdn_mgr_teardown_instances_for_pdn(pdn_mgr_p, DS_NET_DOWN_REASON_INTERNAL_ERROR);
  }

  /*------------------------------------------------------------------------
    Flush any Pending MO Exception Data
  ------------------------------------------------------------------------*/
  if ( pdn_mgr_p->lte_params != NULL &&
  	    pdn_mgr_p->lte_params->mo_exception_data == TRUE )
  {
    bearer_p = pdn_mgr_p->bearer_ptr_tbl[DS_3GPP_DEFAULT_BEARER_INDEX];
    if(ds_bearer_mgr_validate_bearer_ptr(bearer_p))
    {
      bearer_inst = bearer_p->index;
      ds_pdn_cp_data_flush_num_mo_ex_data_cnt(bearer_inst);
      if(!ds_pdn_mgr_is_any_other_mo_ex_pdn_active(pdn_mgr_p))
      {
        data_activity_start.act_status = FALSE;
        (void) ds_msgr_snd_msg_send_ext( 
             DS_3GPP_MO_EXP_ACTIVITY_STATUS_IND,
             MSGR_DS_3GPP, 
             (msgr_hdr_struct_type*)(&data_activity_start),
             sizeof(ds_3gpp_mo_exp_activity_status_ind_type));
      }
    }
  }

  /*------------------------------------------------------------------------
    Remove the entry from the APN table
  ------------------------------------------------------------------------*/
  ds_apn_table_remove_entry_in_apn_table(pdn_mgr_p);


  /* Find an entry in the PDN Limit Table.
     If an entry is found, validate if the entry can be cleared at this point.
     If yes, clear the entry */
  if((index = ds_3gpp_pdn_limit_tbl_find_entry
         ((byte*)pdn_mgr_p->pdp_profile->context.apn)) 
                       != DS_3GPP_PDN_LIMIT_TBL_INVALID_ENTRY)
  {
    ds_3gpp_pdn_limit_tbl_remove_entry(index);
  }

  // Deallocate framework instance if attach PDN is brought down
  if( pdn_mgr_p->is_default &&
      pdn_mgr_p->fw_index != DS_FWK_INST_INVALID )
  {
    ds_fwk_dealloc_attach_inst(pdn_mgr_p->fw_index);
  }
  
  ds_pdn_mgr_free_pdn_context(pdn_mgr_p);
  return;
}

/*===========================================================================
FUNCTION DS_PDN_MGR_GET_CURRENT_ACTIVE_CONTEXT

DESCRIPTION
  This function returns the number of active bearer contexts.

PARAMETERS
  None.
  
DEPENDENCIES
  None.

RETURN VALUE  
  Number of active bearer contexts.

SIDE EFFECTS
  None.
===========================================================================*/
uint8 ds_pdn_mgr_get_current_active_context()
{
  uint8                  loop_index=0;
  uint8                  ref_count=0;
  ds_bearer_mgr_s       *bearer_p = NULL;
  ds_bearer_mgr_state_e  state;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for(loop_index=0;loop_index<DS_3GPP_MAX_BEARER_CONTEXT;loop_index++)
  {
    bearer_p = ds_bearer_mgr_get_instance_ptr(loop_index);
    if(!(ds_bearer_mgr_validate_bearer_ptr(bearer_p)))
    {
      continue;
    }
    /*-------------------------------------------------------------------
     Check bearer context table to check Bearer's which are not in DOWN and
     not in GOING_DOWN and not in COMING_UP state, and increment count.
    -------------------------------------------------------------------*/
    state = bearer_p->state;
    if( state != DS_BEARER_MGR_STATE_DOWN && 
        state != DS_BEARER_MGR_STATE_GOING_DOWN &&
        state != DS_BEARER_MGR_STATE_COMING_UP )
    {
      ref_count++;
    }
  }
  
  return ref_count;
}/* ds_pdn_mgr_get_current_active_context */

/*===========================================================================
FUNCTION DS_PDN_MGR_GET_NUM_ACTIVE_AND_PENDING_CONTEXT

DESCRIPTION
  This function returns the number of active and pending bearer contexts.

PARAMETERS
  None.
  
DEPENDENCIES
  None.

RETURN VALUE  
  Number of active and pending bearer contexts.

SIDE EFFECTS
  None.
===========================================================================*/
uint8 ds_pdn_mgr_get_num_active_and_pending_context
(
  ds_pdn_mgr_s*   pdn_mgr_p
)
{
  uint8                  loop_index=0;
  uint8                  ref_count=0;
  ds_bearer_mgr_s       *bearer_p = NULL;
  ds_bearer_mgr_state_e  state;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(!ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p))
  {
    return ref_count;
  }
  
  for(loop_index=0;loop_index<DS_3GPP_MAX_BEARER_CONTEXT;loop_index++)
  {
    bearer_p = ds_bearer_mgr_get_instance_ptr(loop_index);
    if(!(ds_bearer_mgr_validate_bearer_ptr(bearer_p)))
    {
      continue;
    }

    /*-------------------------------------------------------------------
     Check bearer context table to check Bearer's which are not in DOWN and
     not in GOING_DOWN, and increment count.If bearer is in coming up state
     for any other PDN except for the passed one, increment the count as well. 
    -------------------------------------------------------------------*/
    state = bearer_p->state;
    if( state != DS_BEARER_MGR_STATE_DOWN && 
        state != DS_BEARER_MGR_STATE_GOING_DOWN )
    {
      if( state == DS_BEARER_MGR_STATE_COMING_UP )
      {
        if(pdn_mgr_p->bearer_ptr_tbl[DS_3GPP_DEFAULT_BEARER_INDEX] != bearer_p)
        {
          ref_count++;
        }
      }  
      else
      {
        ref_count++;
      }
    }
  }
  
  return ref_count;
}/* ds_pdn_mgr_get_num_active_and_pending_context */

/*===========================================================================
FUNCTION ds_pdn_mgr_process_pwroff_lpm_cb

DESCRIPTION
  This function is called to process PWROFF/LPM CB by CM

PARAMETERS
  void

DEPENDENCIES
  void

RETURN VALUE  
  void

SIDE EFFECTS

===========================================================================*/
void ds_pdn_mgr_process_pwroff_lpm_cb
(
  void
)
{
  uint8                   index = 0;
  ds_pdn_mgr_s           *pdn_mgr_p = NULL;
  ds_pdn_mgr_inst_type_s *inst_p = NULL;

  DS_MSG0(MSG_LEGACY_HIGH,"LPM/PWROFF processing !");

  lpm_in_progress = TRUE;
  
#ifdef FEATURE_NBIOT_NTN
  if(dssysmgr_get_nb1_ntn_rat_status() && ds_pdn_mgr_is_any_pdn_active())
  {
    ds_pdn_ntn_free_info();
    ds_pdn_psm_populate_info_from_bearers(TRUE);
    ds_pdn_write_ntn_info_to_buffer();
  }
  else
  {
    DS_MSG0(MSG_LEGACY_HIGH,"device is not in NTN or no active pdn");
  }
#endif /* FEATURE_NBIOT_NTN */

  for(index = 0; index<DS_3GPP_MAX_PDN_CONTEXT; index++)
  {
    pdn_mgr_p = ds_pdn_mgr_get_pdn_ptr_from_index(index);
    if( !ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p) )
    {
    	 continue;
    }

    pdn_mgr_p->net_down_reason = DS_NET_DOWN_REASON_INTERNAL_LPM_OR_PWR_DOWN;
    
    inst_p = pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV6];
    if( (inst_p != NULL) &&
    	   (inst_p->state == DS_INSTANCE_STATE_UP) &&
    	   (inst_p->ds_pdn_v6_sm_cb->state == DS_PDN_V6_WAITING_FOR_NET_UP_STATE ||
    	    inst_p->ds_pdn_v6_sm_cb->state == DS_PDN_V6_NET_UP_STATE )
    	 )
    {
      ds_pdn_v6_process_ip6_stop_ev(inst_p->ds_pdn_v6_sm_cb);
    }    
  }

  lpm_in_progress = FALSE;
//Store the throttling timer value during reboot
  ds_pdn_throt_write_throt_timer_to_efs();
  cm_data_packet_state_notify(0xFF);
}/* ds_pdn_mgr_process_pwroff_lpm_cb */

/*===========================================================================
FUNCTION ds_pdn_mgr_process_sub_not_avail_cb

DESCRIPTION
  This function is called to process sub not available CB by CM

PARAMETERS
  void

DEPENDENCIES
  void

RETURN VALUE  
  void

SIDE EFFECTS

===========================================================================*/
void ds_pdn_mgr_process_sub_not_avail_cb
(
  void
)
{
  uint8                   index = 0;
  ds_pdn_mgr_s           *pdn_mgr_p = NULL;
  ds_pdn_mgr_inst_type_s *inst_p = NULL;

  DS_MSG0(MSG_LEGACY_HIGH,"Sub not available processing !");

  sub_not_avail_in_progress = TRUE;

  for(index = 0; index<DS_3GPP_MAX_PDN_CONTEXT; index++)
  {
    pdn_mgr_p = ds_pdn_mgr_get_pdn_ptr_from_index(index);
    if( !ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p) )
    {
    	 continue;
    }

    pdn_mgr_p->net_down_reason = DS_NET_DOWN_REASON_INTERNAL_MMGSDI_CARD_EVT;
    
    inst_p = pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV6];
    if( (inst_p != NULL) &&
    	   (inst_p->state == DS_INSTANCE_STATE_UP) &&
    	   (inst_p->ds_pdn_v6_sm_cb->state == DS_PDN_V6_WAITING_FOR_NET_UP_STATE ||
    	    inst_p->ds_pdn_v6_sm_cb->state == DS_PDN_V6_NET_UP_STATE )
    	 )
    {
      ds_pdn_v6_process_ip6_stop_ev(inst_p->ds_pdn_v6_sm_cb);
    }    
  }

  sub_not_avail_in_progress = FALSE;  
  
  cm_data_packet_state_notify(SYS_MODEM_AS_ID_MASK_1);
}/* ds_pdn_mgr_process_pwroff_lpm_cb */

/*===========================================================================
FUNCTION ds_pdn_mgr_local_teardown

DESCRIPTION
  This function is used to teardown calls when LPM transition occurs
 
PARAMETERS
  void
 
DEPENDENCIES
  void

RETURN VALUE  
  None.

SIDE EFFECTS

===========================================================================*/
void ds_pdn_mgr_local_teardown
(
  void
)
{
  ds_pdn_mgr_s        *pdn_mgr_p = NULL;
  ds_bearer_mgr_s     *bearer_mgr_p= NULL;
  uint8                bearer_inst= 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  for( bearer_inst=0;\
       bearer_inst<DS_3GPP_MAX_BEARER_CONTEXT;\
       bearer_inst++ )
  {
    bearer_mgr_p = ds_bearer_mgr_get_instance_ptr(bearer_inst);
    if(!ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p))
    {
      continue;
    }

    pdn_mgr_p = bearer_mgr_p->pdn_mgr_ptr;

    if (bearer_mgr_p->state == DS_BEARER_MGR_STATE_GOING_DOWN)
    {
      DS_MSG1(MSG_LEGACY_HIGH,
      	       "Bearer %d in GOING_DOWN state during LPM, local_cleanup",
      	       bearer_inst);
      ds_bearer_mgr_local_cleanup(bearer_mgr_p);
      continue;
    }

    ds_bearer_mgr_tear_down_bearer(pdn_mgr_p, bearer_mgr_p);
  }

  return;
}

/*===========================================================================
FUNCTION ds_pdn_mgr_handle_lpm_mode_change

DESCRIPTION
  This function is called to clean up calls when UE goes to LPM mode

PARAMETERS
  void

DEPENDENCIES
  void

RETURN VALUE  
  void

SIDE EFFECTS

===========================================================================*/
void ds_pdn_mgr_handle_lpm_mode_change
(
  sys_oprt_mode_e_type oprt_mode
)
{
  ds_pdn_apm_ind_pay_load  apm_pay_load = {0};
  /*----------------------------------------------------------------------*/

  if(ds3g_mmgsdi_is_subs_ready() == FALSE)
  {
    DS_ERROR_LOG_0("Subs is not ready, ignore LPM !");
    return;
  }

  if(oprt_mode == SYS_OPRT_MODE_LPM || 
     oprt_mode == SYS_OPRT_MODE_OFFLINE)
  {
    DS_MSG0(MSG_LEGACY_HIGH, "LPM mode transition, tearing down pending calls");

    ds_pdn_mgr_local_teardown();

    ds_3gpp_srv_req_throt_go_null(TRUE);
#ifdef FEATURE_MODE_TLB
    ds_eps_tlb_free_info_hndl();
#endif /* FEATURE_MODE_TLB */

    if(ds_pdn_nv_manager_is_rel_11_or_above_supported())
    {
      DS_MSG0(MSG_LEGACY_HIGH, "Reset max active pdp's in LPM");
      ds_3gpp_pdn_reset_max_simul_active_pdp_context();
    }

    /*---------------------------------------------------------------------
      This takes care of clearing all the throttling structures for
      global and per_plmn_throttling
      Also network overridden throttling will be cleared
    -----------------------------------------------------------------------*/
      ds_3gpp_pdn_throt_clear_throttling_on_lpm();
      ds_3gpp_network_override_hdlr_reset(FALSE);

    /*---------------------------------------------------------------------- 
      Send out an indication to APM to reset ATTACH PDN MGR
    -----------------------------------------------------------------------*/
    ds_pdn_apm_ind(DS_PDN_APM_RESET_ATTACH_APN_IND, &apm_pay_load);

#ifdef FEATURE_DATA_RPM
    if (ds_3gpp_is_rpm_enabled())
    {
      ds_3gpp_rpm_handle_lpm_mode();
    }
#endif /* FEATURE_DATA_RPM */

     /*---------------------------------------------------------------------------------- 
      Advertize the blocked APN's to MPPM
     ----------------------------------------------------------------------------------*/
    ds_3gpp_pdn_throt_advertise_blocked_apns_current_plmn();
  }

  return;
} /* ds_pdn_mgr_handle_lpm_mode_change */

/*===========================================================================
FUNCTION      DS_PDN_MGR_CHECK_WILL_TEARDOWN_APN_PARMS_CHANGED_PDN

DESCRIPTION
  this function will be called when iface bringup validation. it will check
  whether a pdn with same profile number as the input pdn already exist in
  ds_pdn_context_tbl. if yes, check if its apn name different with input 
  pdn's apn name and it is going to teardown by apn parms change.

PARAMETERS
  ds_pdn_context_s    *pdn_cntx_p
  
DEPENDENCIES
  Called in DS TASK context.

RETURN VALUE
  TRUE:validation pass
  FALSE:validation false

SIDE EFFECT
  None.
===========================================================================*/
static boolean ds_pdn_mgr_check_will_teardown_apn_parms_changed_pdn
(
  ds_pdn_mgr_s    *pdn_mgr_p
)
{ 
  uint8					     pdn_index = 0;
  ds_pdn_mgr_s		*temp_pdn_mgr_p = NULL;
  char				     	*curr_apn = NULL;
  char					     *temp_apn = NULL;
  boolean        ret_val = FALSE;

  /*--------------------------------------------------------------------------
  below codes are added for this scenerio:
  1.Setup attach pdn1 using profile 1 (apn name apn.a)
  2.Change profile1's apn name to apn.b, it will start a 5s timer to tear down pdn1
  3.Another pdn2 apn.b brought up by client (which also use profile 1)
  4.Then 5s timer arrive, DS try to ask CM to tear down attach pdn1, but due to  pdn2 
    also use profile 1, so pdn2 also got tore down
  5.Then we received call_end for pdn1 from CM and remove old apn (apn.a) entry from 
    apn_table(this is ok), and add the new apn (apn.b) to apn_table, this step failed 
    as apn.b already exist in apn_table
  6.So later in dsPdnContext_bearerDownIndCb, we can not find the pdn1 in apn_table and crash.

  solution is: in step3, we don't bringup pdn2
  --------------------------------------------------------------------------*/
  for (pdn_index = 0;pdn_index < DS_3GPP_MAX_PDN_CONTEXT;pdn_index++)
  {
    temp_pdn_mgr_p = ds_pdn_manager[pdn_index];

    if( !ds_pdn_mgr_validate_pdn_ptr(temp_pdn_mgr_p) ||
    	   (pdn_index == temp_pdn_mgr_p->index) )
    {
      continue;
    }

    if(pdn_mgr_p->pdp_profile_num == temp_pdn_mgr_p->pdp_profile_num)
    {
      curr_apn = (char *)pdn_mgr_p->pdp_profile->context.apn;
      temp_apn = (char *)temp_pdn_mgr_p->pdp_profile->context.apn;
      if( (temp_pdn_mgr_p->lte_params != NULL) &&
      	   (temp_pdn_mgr_p->lte_params->apn_param_chg_teardown == TRUE) && 
   	      (strncasecmp((char*)curr_apn,(char*)temp_apn, DS_PROFILE_APN_STRING_LEN+1) != 0))
      {
        ret_val = TRUE;	 
      }
    }      
  }
  return ret_val;
} /* ds_pdn_cntxt_check_will_teardown_apn_parms_changed_pdn */

/*===========================================================================
FUNCTION      DS_PDN_MGR_STORE_BEARER_CONTEXT

DESCRIPTION
  This function is used to store the bearer context information in the PDN
  context.

PARAMETERS
    pdn_mgr_p     - This is the PDN context inside which we want to store
                            the bearer
    bearer_mgr_p - This is the pointer to the bearer context
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - on successfully storing the bearer info
  FALSE - otherwise

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_pdn_mgr_store_bearer_context
(
  ds_pdn_mgr_s    *pdn_mgr_p,
  ds_bearer_mgr_s *bearer_mgr_p
)
{
  uint8             bearer_index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for(bearer_index = 0; 
      bearer_index < DS_3GPP_MAX_BEARER_CONTEXT_PER_PDN;
      bearer_index++)
  {
    if(pdn_mgr_p->bearer_ptr_tbl[bearer_index] == NULL)
    {
      pdn_mgr_p->bearer_ptr_tbl[bearer_index] = bearer_mgr_p;
      bearer_mgr_p->pdn_mgr_ptr = pdn_mgr_p;
      pdn_mgr_p->num_active_bearer_context++;
      return TRUE;
    }
  }
      
  return FALSE;
}

/*===========================================================================
FUNCTION      DS_PDN_MGR_REMOVE_BEARER_CONTEXT

DESCRIPTION
  This function is used to remove the bearer context information in the PDN
  context.

PARAMETERS
    pdn_mgr_p     - This is the PDN context from which we want to remove
                            the bearer
    bearer_mgr_p - This is the pointer to the bearer context
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - on successfully removing the bearer info
  FALSE - otherwise

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_pdn_mgr_remove_bearer_context
(
  ds_pdn_mgr_s    *pdn_mgr_p,
  ds_bearer_mgr_s *bearer_mgr_p
)
{
  uint8             bearer_index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(!pdn_mgr_p || !bearer_mgr_p)
  {
    return FALSE;
  }
  
  for(bearer_index = 0; 
      bearer_index < DS_3GPP_MAX_BEARER_CONTEXT_PER_PDN;
      bearer_index++)
  {
    if( pdn_mgr_p->bearer_ptr_tbl[bearer_index] != NULL &&
    	   pdn_mgr_p->bearer_ptr_tbl[bearer_index] == bearer_mgr_p )
    {
      pdn_mgr_p->bearer_ptr_tbl[bearer_index] = NULL;
      bearer_mgr_p->pdn_mgr_ptr = NULL;
      return TRUE;
    }
  }
      
  return FALSE;
}

/*===========================================================================
FUNCTION DS_PDN_MGR_SET_THROT_DOWN_REASON

DESCRIPTION
  This function set the throttle down reason based on the IP Type of the
  instance being passed.
 
PARAMETERS 
  inst_p        - Pointer to the iface which is to be examined
  down_reason    - To be populated
    
DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void ds_pdn_mgr_set_throt_down_reason
(
  ds_pdn_mgr_instance_type_e   inst_type, 
  ds_net_down_reason_type     *down_reason_p
)
{                        
  switch(inst_type)                         
  {                                                                 
    case DS_PDN_INSTANCE_IPV4:                                    
      *down_reason_p = DS_NET_DOWN_REASON_ERR_PDN_IPV4_CALL_THROTTLED; 
      break;                                                         
    case DS_PDN_INSTANCE_IPV6:                                    
      *down_reason_p = DS_NET_DOWN_REASON_ERR_PDN_IPV6_CALL_THROTTLED; 
      break;                                                        
    default:                                                        
      *down_reason_p = DS_NET_DOWN_REASON_NOT_SPECIFIED;               
  }                                                                 
}

/*===========================================================================
FUNCTION      DS_PDN_MGR_VALIDATE_INSTANCE_UP_CMD

DESCRIPTION
  Validates state, family for this IP instance and the other instance(for Dual IP).   

PARAMETERS
  iface_p       - Pointer to the iface that is to be brought up.    
  info_p        - Pointer to call information 
  
DEPENDENCIES
  Called in DS TASK context.

RETURN VALUE
  pass/fail flag

SIDE EFFECT 
  None.
===========================================================================*/
boolean ds_pdn_mgr_validate_instance_up_cmd
(
  ds_pdn_mgr_s                    *pdn_mgr_p,
  ds_pdn_mgr_instance_type_e       inst_type,
  ds_pdn_network_params_info_type *info,
  boolean                          is_comp_bring_up
)
{
  ds_local_err_val_e_type       local_err = DS_LOCAL_ERR_VAL_NONE;
  sys_sys_mode_e_type           network_mode = pdn_mgr_p->nw_mode;  
  ds_pdn_mgr_inst_type_s       *inst = NULL;
  ds_bearer_mgr_s              *def_bearer_p = NULL;
  ds_apn_ip_support_type_e      ip_support;
  ds_pdn_cp_data_mo_ex_force_attach_result_type 
  	                             force_attach_result = FALSE;
  boolean                       call_orig_before_ps_attach = FALSE;
  sys_srv_status_e_type         curr_srv_status;
  cm_srv_domain_pref_e_type     curr_oprt_mode_srv_domain;
  sys_srv_domain_e_type         curr_regd_srv_domain;
  boolean                       bring_down_call = FALSE;
  char                          *null_resolved_apn = NULL;
  boolean                       result = FALSE;
  uint8                         handle = 0;
  ds_net_down_reason_type       down_reason;
  uint8                         max_active_pdp_context;
  ds_3gpp_mo_exp_activity_status_ind_type  data_activity_start;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  inst = pdn_mgr_p->inst_ptr[inst_type];
  
  // This is only for the single PDN MO Exception SNI from ACB/T3346 case
  if ((ds_pdn_cp_data_is_mo_exception_data_supported_on_pdn(pdn_mgr_p) 
                                                    == TRUE) &&
      (ds_pdn_cp_data_is_force_attach_in_progress() == TRUE))
  {
    if((pdn_mgr_p->pdp_profile->context.pdp_type == DS_PROFILE_PDP_IPV4V6) &&
       (is_comp_bring_up == TRUE))
    {
      /* Companion instance will resume later for MO Exception */
      /* Return FALSE as we dont want instance bring up to continue */
      return FALSE;
    }
    else if((is_comp_bring_up == FALSE) &&
            (ds_eps_pdn_attach_sm_get_state() == 
                                      DS_EPS_PROCESSING_GET_PDN_CONN_IND))
    {
      /* Return TRUE for main instance bring up as force attach is in progress and
         validation was done previously */
      return TRUE;
    }
  }

  if(pdn_mgr_p->is_restore_mode == TRUE)
  {
    return TRUE;
  }
  
  /*--------------------------------------------------------------------------- 
    Check if NULL APN is supported
   --------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_LTE
  if(!ds_pdn_nv_manager_get_null_apn_support_for_non_attach_pdn() &&
      ds_pdn_mgr_reject_null_apn(pdn_mgr_p, 
                                 pdn_mgr_p->pdp_profile->context.apn))
  { 
    local_err = DS_LOCAL_ERR_VAL_0;
    pdn_mgr_p->net_down_reason = DS_NET_DOWN_REASON_NULL_APN_DISALLOWED;
    goto func_exit;
  }
#endif  /* FEATURE_DATA_LTE */

  if(TRUE == ds_pdn_profile_validator_is_apn_forbidden(pdn_mgr_p))
  {
    local_err = DS_LOCAL_ERR_VAL_1;
    pdn_mgr_p->net_down_reason = DS_NET_DOWN_REASON_INVALID_APN_NAME;
    goto func_exit;
  }

  if(ds_pdn_roaming_get_status() == TRUE)
  {
    if(pdn_mgr_p->pdp_profile->roaming_disallowed)
    {
      local_err = DS_LOCAL_ERR_VAL_2;
      pdn_mgr_p->net_down_reason = DS_NET_DOWN_REASON_APN_DISALLOWED_ON_ROAMING;
      goto func_exit;
    }
  }

  if(ds_pdn_nv_manager_is_rel_11_or_above_supported())
  {
    /*-------------------------------------------------------------------------
       Check if maximum simultanoues pdp has reached before triggering 
       another call and bringing up iface 
    -------------------------------------------------------------------------*/
    max_active_pdp_context = ds_3gpp_pdn_get_max_simul_active_pdp_context
                                (ds_3gpp_pdn_throt_sm_sys_mode_to_rat_type
                                 (network_mode));

    if( ds_pdn_mgr_get_current_active_context() >= max_active_pdp_context)
    {
      local_err = DS_LOCAL_ERR_VAL_14;
      goto func_exit;
    }
    }

  /*--------------------------------------------------------------------- 
   Check if throttle type is T3396_ABSENT, IF the throttled username/Password
   is different from the originated call information, unthrottle the
   PDN throt sm
    ---------------------------------------------------------------------*/
  if(ds_pdn_nv_manager_get_global_throttling_nv())
  {
    DS_MSG0(MSG_LEGACY_HIGH,"Global throttling enabled, Perform T3396 validation check");
    ds_3gpp_pdn_cntxt_validate_auth_params_and_reset_t3396_throt(
                                  pdn_mgr_p->pdp_profile,info);
  }
  /*----------------------------------------------------------------------
    Perform throttling check.
      - only for the first iface out of a dual IP call (the iface which
        application requested, in which case default bearer will be null)
      - if apn is throttled, cleanup pdn and return failure
        if Release 11 is enabled then, check if any of the EFacc classes are
        enabled.
        Criteria: Even if APN is throttled and Efacc is set, We send PDP 
                  context request towards the network.
  ----------------------------------------------------------------------*/
     if( (ds_pdn_mgr_get_default_bearer(pdn_mgr_p) == NULL && 
          ds_3gpp_pdn_throt_is_apn_throttled(
                                    (byte*)pdn_mgr_p->pdp_profile->context.apn, 
                                             inst_type, pdn_mgr_p, TRUE))
       )
       {
         ds_pdn_mgr_set_throt_down_reason(inst_type, &down_reason);
         
#ifdef FEATURE_DATA_RPM
	  if (ds_3gpp_is_rpm_enabled() && is_comp_bring_up == FALSE)
   {
      /*----------------------------------------------------------------------------- 
        Ensuring that if CPDP counter is already incremented once as part of
        V4 iface validation, its not incremented again during V6 iface validation
        or vice versa.
      ------------------------------------------------------------------------------*/
     if (down_reason == DS_NET_DOWN_REASON_ERR_PDN_IPV4_CALL_THROTTLED ||
         down_reason == DS_NET_DOWN_REASON_ERR_PDN_IPV6_CALL_THROTTLED)
     {
       ds_3gpp_rpm_inc_cpdp((byte*)pdn_mgr_p->pdp_profile->context.apn);
     }
   }
#endif /* FEATURE_DATA_RPM */
    
    pdn_mgr_p->net_down_reason = down_reason;
    local_err = DS_LOCAL_ERR_VAL_15;
    goto func_exit;	
	}
	
  /*-------------------------------------------------------------------------
     Check if maximum simultanoues pdp has reached before triggering 
     another call and bringing up iface 
  -------------------------------------------------------------------------*/
  if (ds_pdn_cp_data_is_enabled())
  {
    if( ds_pdn_mgr_get_num_active_and_pending_context(pdn_mgr_p) >= 
    	                           DS_PDN_CP_DATA_MAX_BEARER_SUPPORTED)
    {      
      local_err = DS_LOCAL_ERR_VAL_4;
      pdn_mgr_p->net_down_reason = DS_NET_DOWN_REASON_MAX_NB_PDN_REACHED;
      goto func_exit;
    }
  }

  network_mode = ds_pdn_hdlr_get_current_network_mode();

  /*---------------------------------------------------------------------------
    Check if network has overridden the IP type.
    If a call orig IP type is different from the overridden ip type then
    bring down the call
  ---------------------------------------------------------------------------*/  
  if(ds_3gpp_call_hdlr_check_if_pdp_type_throttled(inst, 
                                       pdn_mgr_p, network_mode) == TRUE)
  {
    return FALSE;
  }
   
  if( (network_mode == SYS_SYS_MODE_GSM && 
  	    ((pdn_mgr_p->pdp_profile->apn_bearer & 
  	                           DS_PROFILE_APN_BEARER_FLAG_G) == 0)) ||
      (network_mode == SYS_SYS_MODE_LTE && 
  	    ((pdn_mgr_p->pdp_profile->apn_bearer & 
  	                           DS_PROFILE_APN_BEARER_FLAG_L) == 0))
    )
  {
    local_err = DS_LOCAL_ERR_VAL_5;
    pdn_mgr_p->net_down_reason = DS_NET_DOWN_REASON_PROFILE_BEARER_INCOMPATIBLE;
    goto func_exit;
  }

  if ( network_mode == SYS_SYS_MODE_GSM && 
       ds_pdn_mgr_is_wwan_tech_suspended())
  {
    local_err = DS_LOCAL_ERR_VAL_14;
    pdn_mgr_p->net_down_reason = DS_NET_DOWN_REASON_INTERNAL_ERROR;
    DS_MSG0(MSG_LEGACY_HIGH,"call req on GSM when tech is suspended");
    goto func_exit;
  }
  
  /*------------------------------------------------------------------------
    We skip PDN Connection Limit Handling if this is initial attach
    We will first check if there is entry corresponding to the APN in the
    PDN Limit Table. If there is one, we get an index to that entry.
    If not, we create a new entry in the PDN Limit Table with the given
    parameters.
    Next we check if iface bringup is allowed for the given index.
    If yes, we continue. Otherwise we set the throttle down reason and return
    failure to the application.
   ------------------------------------------------------------------------*/
  if (ds_3gpp_pdn_limit_is_enabled())
  {
    if(ds_eps_pdn_attach_sm_get_state() != DS_EPS_ATTACHED)
     {
      DS_MSG0(MSG_LEGACY_HIGH,
              "Attach is in progress, PDN Limit Handling skipped");
    }
    else
    {
      result = ds_3gpp_pdn_cntxt_validate_and_update_pdn_limit_entry(
                             pdn_mgr_p->pdp_profile_num , 
                             &handle);

      if((result == FALSE) ||
         ((handle != DS_3GPP_PDN_LIMIT_TBL_INVALID_ENTRY) && 
          (ds_3gpp_pdn_limit_call_bringup_is_allowed(handle) == FALSE))) 
      {
        ds_pdn_mgr_set_throt_down_reason(inst_type, 
                                         &pdn_mgr_p->net_down_reason);
        local_err = DS_LOCAL_ERR_VAL_6;
        goto func_exit;
      }
      }
    }

/*------------------------------------------------------------------------- 
  APN reject throttle SM check
  -------------------------------------------------------------------------*/
  if (ds_3gpp_dam_is_apn_reject_sm_throttled(pdn_mgr_p) == TRUE)
  {
    ds_pdn_mgr_set_throt_down_reason(inst_type, &down_reason);
    local_err = DS_LOCAL_ERR_VAL_16;
    goto func_exit;
  }

    call_orig_before_ps_attach = 
                 ds_pdn_nv_manager_get_call_orig_before_ps_attach();              
                
    if ( call_orig_before_ps_attach == FALSE &&
         ds_pdn_hdlr_get_current_network_mode() != SYS_SYS_MODE_LTE )
    {
      /*-------------------------------------------------------------------------
        Allow call origination when srv_status is FULL_SERVICE 
        in the following scnearios:
        1. UE oprt_mode CS+PS and regd_srv_domain CS_PS or PS_Only.
        2. UE oprt_mode PS_only and regd_srv_domain PS_Only.
        3. UE oprt_mode CS_Only.
        This is applicable only for G. For LTE, when attach procedure is
        initiated, the srv_status will be lim_srv.
      -------------------------------------------------------------------------*/
    curr_srv_status = dssysmgr_get_srv_status();
    curr_oprt_mode_srv_domain = dssysmgr_get_oprt_mode_srv_domain();
    curr_regd_srv_domain = dssysmgr_get_srv_domain();
  
    DS_MSG3(MSG_LEGACY_HIGH,
    	       "curr_srv_status: %d, curr_oprt_mode_srv_domain: %d, "
    	       "curr_regd_srv_domain: %d",
                         curr_srv_status, curr_oprt_mode_srv_domain, curr_regd_srv_domain);
    
    if (SYS_SRV_STATUS_SRV == curr_srv_status)
    {
      if (curr_oprt_mode_srv_domain == CM_SRV_DOMAIN_PREF_CS_PS)
      {
        if ((curr_regd_srv_domain != SYS_SRV_DOMAIN_CS_PS) &&
            (curr_regd_srv_domain != SYS_SRV_DOMAIN_PS_ONLY))
        {
          bring_down_call = TRUE; 
        }
      }
      else if (curr_oprt_mode_srv_domain == CM_SRV_DOMAIN_PREF_PS_ONLY)
      {
        if (curr_regd_srv_domain != SYS_SRV_DOMAIN_PS_ONLY)
        {
          bring_down_call = TRUE; 
        }
      }
    }
    else
    {
      bring_down_call = TRUE; 
    }
  
    if (TRUE == bring_down_call)
    {    
      ds_pdn_mgr_set_throt_down_reason(inst_type, &pdn_mgr_p->net_down_reason);        
      local_err = DS_LOCAL_ERR_VAL_5;
      goto func_exit;        
    }
  }

  /*-------------------------------------------------------------------------
    Allow instance to be brought up if the attach_initiated_flag is true
    or if attach is complete.
  -------------------------------------------------------------------------*/
  if( (network_mode == SYS_SYS_MODE_LTE) &&
      (ds_pdn_hdlr_get_attach_initiated_flag() == FALSE) && 
      (ds_pdn_hdlr_get_attach_complete_flag() == FALSE) )
  {
    /*--------------------------------------------------------------------------
      We have entered a case where the attach init flag is FALSE in LTE and we
      do not have full service. We could be in a case where we are processing
      the iface up command on the second of the two ifaces in a dual IP bearer
      PDN context

      In such a case, we have to see if the PDN type is V4V6 and if the PDN
      has been flagged for Iface bring up
     
      We could also have entered the case where we are triggering SNI for
      MO exception data while attach was previously barred and force attach
      needs to be triggered.  
    --------------------------------------------------------------------------*/
    if((pdn_mgr_p->pdp_profile->context.pdp_type == DS_PROFILE_PDP_IPV4V6) &&
       (pdn_mgr_p->allow_inst_bring_up == TRUE))
    {
      DS_MSG0(MSG_LEGACY_HIGH,
      	       "Bringing up the second of the 2 instances of PDN for attach");
    }
    else if ((TRUE == pdn_mgr_p->lte_params->mo_exception_data) &&
             (TRUE == pdn_mgr_p->lte_params->allow_force_attach))
    {
      DS_MSG0(MSG_LEGACY_HIGH,
      	       "Bringing up an instance for PDN context supporting "
              "MO exception while not attached - will trigger force attach");
    }
    else
    {
      def_bearer_p = pdn_mgr_p->bearer_ptr_tbl[DS_3GPP_DEFAULT_BEARER_INDEX];
      if (ds_bearer_mgr_validate_bearer_ptr(def_bearer_p))
      {
        DS_MSG0(MSG_LEGACY_HIGH,
        	       "OTA request sent for companion instance bringup,"
                " Not bringing down instance");
        return TRUE;
      }
      
      local_err = DS_LOCAL_ERR_VAL_6;
      pdn_mgr_p->net_down_reason = DS_NET_DOWN_REASON_NO_SRV;
      goto func_exit;
    }
  }

  if(ds_apn_table_get_ip_support_info_for_pdn(pdn_mgr_p, &ip_support) == FALSE)
  {
    local_err = DS_LOCAL_ERR_VAL_7;
    pdn_mgr_p->net_down_reason = DS_NET_DOWN_REASON_INTERNAL_ERROR;
    goto func_exit;
  }

  /*-------------------------------------------------------------------------
    We should ensure that we are allowing only the default iface to be 
    brought up when the attach_init_flag is true. 
  -------------------------------------------------------------------------*/
  if(ds_pdn_hdlr_get_attach_initiated_flag() == TRUE)
  {
    if( (pdn_mgr_p->is_default != TRUE) &&
        (pdn_mgr_p->lte_params->mo_exception_data == FALSE) )
    {
      local_err = DS_LOCAL_ERR_VAL_8;
      pdn_mgr_p->net_down_reason = DS_NET_DOWN_REASON_NO_SRV;
      goto func_exit;
    }

    if(pdn_mgr_p->pdp_profile->context.pdp_type == DS_PROFILE_PDP_IPV4V6)
    {
      /*----------------------------------------------------------------------
        Mark in the pdn context that we should allow the next iface bring up
        This is because we are gonna reset the is_attach_init_flag and there
        is no way we can figure out if we can allow the other iface in this 
        PDN context to be brought up

        This is applicable only to Dual IP bearer PDN contexts
      ----------------------------------------------------------------------*/
      pdn_mgr_p->allow_inst_bring_up = TRUE;
    }

    /*------------------------------------------------------------------------
      Reset the is_attach_initiated_flag
    ------------------------------------------------------------------------*/
    ds_pdn_hdlr_set_attach_initiated_flag(FALSE);
  }
  else
  {
    /*--------------------------------------------------------------------------
      In LTE mode, if UE is not yet attached, we do not allow any non-default 
      iface to come up
    --------------------------------------------------------------------------*/
    if ( (network_mode == SYS_SYS_MODE_LTE) &&
         (ds_eps_pdn_attach_sm_get_state() != DS_EPS_ATTACHED) &&
         (pdn_mgr_p->allow_inst_bring_up == FALSE) &&
         (pdn_mgr_p->lte_params->mo_exception_data == FALSE) )
    {
      local_err = DS_LOCAL_ERR_VAL_9;
      pdn_mgr_p->net_down_reason = DS_NET_DOWN_REASON_NO_SRV;                  
      goto func_exit;
    }
  }

  /*--------------------------------------------------------------------------
    If the APN disable flag is set in profile, we should not attempt to bring
    up the Iface.
    Post Iface down and return.
  --------------------------------------------------------------------------*/
  if(pdn_mgr_p->pdp_profile->apn_disable_flag == TRUE)
  {
    local_err = DS_LOCAL_ERR_VAL_10;
    pdn_mgr_p->net_down_reason = DS_NET_DOWN_REASON_INTERNAL_ERROR;
    goto func_exit;
  }

  if(ds_pdn_mgr_is_wwan_tech_suspended() == TRUE && 
     pdn_mgr_p->lte_params->mo_exception_data == FALSE)
  {
    local_err = DS_LOCAL_ERR_VAL_15;
    DS_MSG0(MSG_LEGACY_HIGH, "call bring up req on LTE when tech is suspended");
    pdn_mgr_p->net_down_reason = DS_NET_DOWN_REASON_INTERNAL_ERROR;                  
    goto func_exit;
  }	  
  /* MO Exception Handling */
  if ( pdn_mgr_p->lte_params != NULL &&
  	    pdn_mgr_p->lte_params->mo_exception_data == TRUE )
  {
    memset(&data_activity_start, 0, 
           sizeof(ds_3gpp_mo_exp_activity_status_ind_type));
      
    data_activity_start.act_status = TRUE;
    (void) ds_msgr_snd_msg_send_ext( 
               DS_3GPP_MO_EXP_ACTIVITY_STATUS_IND,
                MSGR_DS_3GPP, 
              (msgr_hdr_struct_type*)(&data_activity_start),
                sizeof(ds_3gpp_mo_exp_activity_status_ind_type));
#ifdef FEATURE_GPS_WWAN_PRIORITY_SEARCH
     if(dssysmgr_get_gps_search_status())
    {
      /* Abort high prioirt search for new mo_ex data call req */
        cm_ph_cmd_terminate_get_networks(NULL,NULL,CM_CLIENT_TYPE_DS);      
      }
#endif /* FEATURE_GPS_WWAN_PRIORITY_SEARCH */

    if (ds_eps_pdn_attach_sm_get_state() != DS_EPS_ATTACHED)
    {

      if(!ds_pdn_mgr_ue_is_in_thermal_level3_mode()
#ifdef FEATURE_GPS_WWAN_PRIORITY_SEARCH
          && !dssysmgr_get_gps_search_status()
#endif /* FEATURE_GPS_WWAN_PRIORITY_SEARCH */
        )
      {
        force_attach_result = ds_pdn_cp_data_trigger_mo_ex_force_attach(pdn_mgr_p);
        if (DS_PDN_CP_DATA_MO_EXCP_FORCE_ATTACH_ALLOWED == force_attach_result)
        {
        /* Save SNI inst_p and info_p for resuming later */
          ds_pdn_cp_data_save_force_attach_params(pdn_mgr_p,inst_type,info);
        }
        else if (DS_PDN_CP_DATA_MO_EXCP_FORCE_ATTACH_NOT_ALLOWED == force_attach_result)
        {        
          local_err = DS_LOCAL_ERR_VAL_11;
          pdn_mgr_p->net_down_reason = DS_NET_DOWN_REASON_NO_SRV;
          goto func_exit;
        }
      }
      else
      {
      /* Save SNI inst_p and info_p for resuming later */
        DS_MSG0(MSG_LEGACY_HIGH," saving force attach param");
        ds_pdn_cp_data_save_force_attach_params(pdn_mgr_p,inst_type,info);
      }
      // Return Failure so we will skip the rest of IFACE Up Cmd until attached
      return FALSE;
    } /* PDN is not attached */
    else
    {
      if(ds_pdn_mgr_ue_is_in_thermal_level3_mode()
#ifdef FEATURE_GPS_WWAN_PRIORITY_SEARCH
         ||dssysmgr_get_gps_search_status()
#endif /* FEATURE_GPS_WWAN_PRIORITY_SEARCH */
        )
      {
          /* Save SNI inst_p and info_p for resuming later */
        DS_MSG0(MSG_LEGACY_HIGH," saving force attach param");
        ds_pdn_cp_data_save_force_attach_params(pdn_mgr_p,inst_type,info);
        return FALSE;
      }
    }
  } /* PDN supports MO exception */
  /* End MO Exception Handling */

  /*--------------------------------------------------------------------------
    Devices that do not support IMS shall NOT make a PDN connection request 
    using the class 3 APN in the APN table when the class 3 APN in the APN table 
    is different from the APN chosen by the network for the attach
  --------------------------------------------------------------------------*/
  ds_pdn_mgr_get_resolvedapn_for_defaultpdn(&null_resolved_apn);
  if( ds_pdn_nv_manager_get_rel_10_throttling_per_plmn_from_nv() &&
      ds_pdn_call_hdlr_disallow_class3_pdn(pdn_mgr_p, null_resolved_apn))
  {
    local_err = DS_LOCAL_ERR_VAL_13;
    pdn_mgr_p->net_down_reason = DS_NET_DOWN_REASON_INVALID_CLASS3_APN;
    goto func_exit;
  }

  if(ds_pdn_mgr_check_will_teardown_apn_parms_changed_pdn(pdn_mgr_p))
  {  
    local_err = DS_LOCAL_ERR_VAL_12;
    pdn_mgr_p->net_down_reason = DS_NET_DOWN_REASON_APN_PRM_CHG;
    goto func_exit;
  }

func_exit:  
  if(local_err != DS_LOCAL_ERR_VAL_NONE)
  {    
    DS_MSG1(MSG_LEGACY_ERROR,
           "Instance bring up validation failed, error : %d.", local_err);

    DS_IOT_MSG1(MSG_LEGACY_ERROR,
           "Instance bring up validation failed, error : %d.", local_err);

    if( ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p))
  	{    
      ds_pdn_mgr_cleanup_pdn(pdn_mgr_p, inst_type);
    }

    return FALSE;
  }

  return TRUE;
} /* ds_pdn_cntxt_validate_iface_up_cmd */

/*===========================================================================
FUNCTION      DS_PDN_MGR_INSTANCE_UP_HDLR

DESCRIPTION
  Brings up the IP instance requested after allocating a PDN instance.

PARAMETERS
  PDN context
  Instance type
  Call Info
  is_comp_bring_up - Is this call for companion instance bring up.

DEPENDENCIES
  Called in DS TASK context.

RETURN VALUE
  True- If successfull instance bring up.
  False - Otherwise

SIDE EFFECTS
  If MT table has matching call info as current call,
  this will clear the table entry.
===========================================================================*/
boolean ds_pdn_mgr_instance_up_hdlr
(
  ds_pdn_mgr_s                    *pdn_mgr_p,
  ds_pdn_mgr_instance_type_e       inst_type,
  ds_pdn_network_params_info_type *info_ptr,
  boolean                          is_comp_bring_up
)
{
  ds_local_err_val_e_type       local_err = DS_LOCAL_ERR_VAL_NONE;
  sys_sys_mode_e_type           network_mode;  
  ds_pdn_mgr_inst_type_s       *inst = NULL;  
  ds_pdn_mgr_inst_type_s       *comp_inst = NULL;
  ds_bearer_mgr_s              *bearer_mgr_p = NULL;
  ds_bearer_flow_manager_s     *bflow_manager_p = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( !ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p) ||
  	   inst_type >= DS_PDN_INSTANCE_MAX )
  {
    local_err = DS_LOCAL_ERR_VAL_0;
    goto func_exit;
  }
  
  inst = pdn_mgr_p->inst_ptr[inst_type];
  if( (inst == NULL) ||
  	   (inst->state != DS_INSTANCE_STATE_DOWN) )
  {
    DS_MSG0(MSG_LEGACY_HIGH, " Instance is NULL/ not in down state");
	return TRUE;
  }
  
  comp_inst = ds_pdn_mgr_get_companion_inst(pdn_mgr_p, inst_type);
  
  if(ds_pdn_mgr_validate_instance_up_cmd(
                 pdn_mgr_p, inst_type, info_ptr, is_comp_bring_up) == FALSE)
  {
    local_err = DS_LOCAL_ERR_VAL_2;
    goto func_exit;
  }

  /* Move the instance state to coming up state as validation is done */
  inst->state = DS_INSTANCE_STATE_COMING_UP;

  /*------------------------------------------------------------------------- 
   Save info_ptr in pdn. It may be required later for redialing the call.
  -------------------------------------------------------------------------*/
  if(info_ptr != NULL)
  { 
    memscpy(&pdn_mgr_p->info ,sizeof(ds_pdn_network_params_info_type),
            info_ptr,sizeof(ds_pdn_network_params_info_type)); 
  }
  else
  {
    DS_MSG0(MSG_LEGACY_HIGH, "Info_ptr is Null.");
  }
  	
  network_mode = ds_pdn_hdlr_get_current_network_mode();

#ifdef FEATURE_DATA_CC_SP_THROTTLING
  if((ds_3gpp_throt_sm_is_cc_sp_throttling_enabled())&&
      (network_mode != SYS_SYS_MODE_LTE))
  {
     ds_3gpp_cc_sp_clr_timer();
  }
#endif /* FEATURE_DATA_CC_SP_THROTTLING */

  /*---------------------------------------------------------------------------
    Check to see if the default bearer is already available. It is possible 
    that we are acting on the Iface UP command for a second Iface attached to 
    the same PDN context. In that case, there is no need to allocate a bearer.
  ---------------------------------------------------------------------------*/
  if(pdn_mgr_p->bearer_ptr_tbl[DS_3GPP_DEFAULT_BEARER_INDEX] == NULL)
  {
    /*-----------------------------------------------------------------------
      Allocate a bearer context from the free pool of bearers. If no bearer 
      is available, we should fail the call.
    -----------------------------------------------------------------------*/
    bearer_mgr_p = ds_bearer_mgr_allocate_new_bearer();

    if(!ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p))
    {
      pdn_mgr_p->net_down_reason = DS_NET_DOWN_REASON_INSUFFICIENT_RESOURCES;
      ds_pdn_mgr_cleanup_pdn(pdn_mgr_p, inst_type);
      local_err = DS_LOCAL_ERR_VAL_3;
      goto func_exit;
    }

    if(ds_pdn_mgr_store_bearer_context(pdn_mgr_p,bearer_mgr_p) != TRUE)
    {
      /*---------------------------------------------------------------------
        We should not hit this case of running out of placeholders for bearer
        during iface bring up. If this were to happen, it is probably because
        the bearer pointers were not cleaned up from a prior use of the PDN
        context.
      ---------------------------------------------------------------------*/
      DS_MSG0(MSG_LEGACY_HIGH,"Could not store the allocated bearer");

      //Deallocate bearer
      ds_bearer_mgr_free_dyn_mem(bearer_mgr_p);
      pdn_mgr_p->net_down_reason = DS_NET_DOWN_REASON_INTERNAL_ERROR;
      ds_pdn_mgr_cleanup_pdn(pdn_mgr_p, inst_type);
      local_err = DS_LOCAL_ERR_VAL_4;
      goto func_exit;
    }
    
    // Set this bearer as default.
    bearer_mgr_p->is_default = TRUE;

    /*-----------------------------------------------------------------------
      Allocate a PDN flow manager and bearer flow manager
    -----------------------------------------------------------------------*/
    if(ds_pdn_flow_manager_allocate_mem(pdn_mgr_p) == NULL)
    {
      pdn_mgr_p->net_down_reason = DS_NET_DOWN_REASON_INTERNAL_ERROR;
      ds_pdn_mgr_cleanup_pdn(pdn_mgr_p, inst_type);      
      local_err = DS_LOCAL_ERR_VAL_5;
      goto func_exit;
    }

    bflow_manager_p = 
      ds_bearer_flow_manager_allocate_mem(bearer_mgr_p);

    if(NULL == bflow_manager_p)
    {
       pdn_mgr_p->net_down_reason = DS_NET_DOWN_REASON_INTERNAL_ERROR;
       ds_pdn_mgr_cleanup_pdn(pdn_mgr_p, inst_type);
       local_err = DS_LOCAL_ERR_VAL_6;
       goto func_exit;
    }
    /*-------------------------------------------------------------------------
      Add Bearer Flow manager to Flow manager.
    -------------------------------------------------------------------------*/
    if ( ds_pdn_flow_mgr_add_bearer_to_pdn_flow(pdn_mgr_p,bearer_mgr_p) 
    	                                                          == FALSE )
    {
      DS_ERROR_FATAL("Error in adding bearer flow manager to flow manager");
    } 
    
    DS_MSG0(MSG_LEGACY_HIGH, "Allocated Flow Manager and bearer Flow Manager");

  }
  else
  {
    bearer_mgr_p = pdn_mgr_p->bearer_ptr_tbl[DS_3GPP_DEFAULT_BEARER_INDEX];
  }

  ds_flow_mgr_configure_default_flow(pdn_mgr_p,
                                     bearer_mgr_p,
                                     inst);

  /*----------------------------------------------------------------------------
    Now check if there is an instance of the other IP type bound to the same PDN. 
    If there is one, call bring up on the instance if the state is not COMING_UP.
    Return is needed after companion instance bring up, as bearer will be
    brought up with the call for same function for companion instance.
  ----------------------------------------------------------------------------*/
  if(comp_inst != NULL)
  {
    if(is_comp_bring_up == FALSE &&
       comp_inst->state == DS_INSTANCE_STATE_DOWN)
    {
      DS_MSG0(MSG_LEGACY_HIGH, "Bringing up other instance");
      if(ds_pdn_mgr_instance_up_hdlr(
                           pdn_mgr_p, comp_inst->type, info_ptr, TRUE) == TRUE)
      {
        return TRUE;
      }
    }    
  }
  
  if(pdn_mgr_p->state == DS_PDN_MGR_STATE_DOWN)
  {
    pdn_mgr_p->state = DS_PDN_MGR_STATE_COMING_UP;
  }

  /*-------------------------------------------------------------------------
    Bring up the bearer. 
  -------------------------------------------------------------------------*/      
  if (FALSE == ds_bearer_mgr_bring_up_bearer(bearer_mgr_p))
  {
    pdn_mgr_p->net_down_reason = DS_NET_DOWN_REASON_INTERNAL_ERROR;
	local_err = DS_LOCAL_ERR_VAL_7;
	goto func_exit;
  }
  
  return TRUE;

func_exit:

	 if(local_err != DS_LOCAL_ERR_VAL_NONE)
	 {
	   DS_MSG1(MSG_LEGACY_ERROR,
	   	       "Instance bring up failed, error : %d.", local_err);
    
    if(comp_inst != NULL)
    {
      if(is_comp_bring_up == FALSE &&
         comp_inst->state == DS_INSTANCE_STATE_DOWN)
      {
        DS_MSG0(MSG_LEGACY_HIGH, "Bringing up other instance");
        if(ds_pdn_mgr_instance_up_hdlr(
                        pdn_mgr_p, comp_inst->type, info_ptr, TRUE) == TRUE)
        {
          return TRUE;
        }
      }    
    }    
  }

  return FALSE;
} /* ds_pdn_cntxt_ifaceUpCmdHdlr() */

/*===========================================================================
FUNCTION ds_pdn_mgr_update_pco

DESCRIPTION
  This function is used to update PCO. This inturn invokes
  ds_pdn_cntxt_decodeProtocolParams to perform the actual update.
  
PARAMETERS
    Bearer ptr
    PCO
    ePCO    
    
DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void ds_pdn_mgr_update_pco
(
  ds_bearer_mgr_s                  *bearer_mgr_p,
  protocol_cfg_options_T           *config_options_p,
  ext_protocol_cfg_options_T       *ext_protocol_conf_opt_p
)
{
  ds_pdn_mgr_s                   *pdn_mgr_p = NULL;
  uint32                          decode_mask = 0;
  ds_pdn_protocol_cfg_options_s   ds_proto_config_options = {0};
  ds_profile_pdp_type_e           pdn_pdp_type = DS_PROFILE_PDP_MAX;
  ds_pdn_mgr_inst_type_s         *v4_inst = NULL, *v6_inst = NULL;
  ds_pdn_event_info_u_type        event_info;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset(&event_info, 0, sizeof(ds_pdn_event_info_u_type));
  if(!ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p) ||
     !config_options_p ||
     !ext_protocol_conf_opt_p )
  {
    DS_ERROR_LOG_0("NULL input !");
    return;
  }
  
  DS_MSG1(MSG_LEGACY_HIGH,
          "ds_3gpp_pdn_cntx_update_pcofor bearer index %d",
          bearer_mgr_p->index);

  if((SYS_RAT_LTE_NB1_RADIO_ACCESS == ds_pdn_mgr_get_active_rat()
#ifdef FEATURE_NBIOT_NTN
     || SYS_RAT_LTE_NB1_NTN_RADIO_ACCESS == ds_pdn_mgr_get_active_rat()
#endif /* FEATURE_NBIOT_NTN */
     )&&
     TRUE == ext_protocol_conf_opt_p->valid)
  {
    ds_proto_config_options.valid = 
      ds_pdn_pco_epco_hdlr_convert_nas_epco_to_ds
       (ext_protocol_conf_opt_p,&ds_proto_config_options);
  }
  else
  {
    ds_proto_config_options.valid = 
      ds_pdn_pco_epco_hdlr_convert_nas_pco_to_ds
      (config_options_p,&ds_proto_config_options);
  }

  pdn_mgr_p = bearer_mgr_p->pdn_mgr_ptr;
  if(!ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p))
  {
    return;
  }

  if(ds_proto_config_options.valid)
  {
    ds_bearer_mgr_disable_tx_flow(bearer_mgr_p, DS_BEARER_MODIFICATION_IN_PROGRESS_MASK);

    decode_mask = ds_pdn_mgr_decode_proto_params(pdn_mgr_p,
                                                 &ds_proto_config_options);   
    pdn_pdp_type = ds_apn_table_get_pdn_pdp_type(pdn_mgr_p);
    
    if( pdn_pdp_type == DS_PROFILE_PDP_IPV4 ||
        pdn_pdp_type == DS_PROFILE_PDP_IPV4V6 )
    {
      v4_inst = pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV4];
      if(v4_inst &&
         decode_mask & DS_3GPP_PCSCF_V4_PKT_DECODED_MASK )
      {        
        event_info.extended_ip_config_change_mask = 
                            DS_EXT_IP_CFG_MASK_PCSCF_ADDR;
        ds_fwk_notify_events( pdn_mgr_p->fw_index,
                      ds_pdn_mgr_convert_pdn_inst_to_fw_ip_type(v4_inst->type),
                              PDN_EXTENDED_IP_CONFIG_EX_EV,
                              event_info);                                              
      }       
    }
    
    if( pdn_pdp_type == DS_PROFILE_PDP_IPV6 ||
        pdn_pdp_type == DS_PROFILE_PDP_IPV4V6 )
    {
      v6_inst = pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV6];
      if(v6_inst &&
         decode_mask & DS_3GPP_PCSCF_V6_PKT_DECODED_MASK )
      {        
        event_info.extended_ip_config_change_mask = 
                            DS_EXT_IP_CFG_MASK_PCSCF_ADDR;
        ds_fwk_notify_events( pdn_mgr_p->fw_index,
                      ds_pdn_mgr_convert_pdn_inst_to_fw_ip_type(v6_inst->type),
                              PDN_EXTENDED_IP_CONFIG_EX_EV,
                              event_info);                                              
      }
    }    

    ds_bearer_mgr_enable_tx_flow(bearer_mgr_p, DS_BEARER_MODIFICATION_IN_PROGRESS_MASK);
  }
} /*ds_pdn_mgr_update_pco() */

/*===========================================================================
FUNCTION DS_PDN_MGR_FILL_PCO_CONTAINER_INFO

DESCRIPTION
  This function fills the requested container info in PCO. 
  
PARAMETERS   
  protocol_cfg_options_T       *protocol_config_options_p
    
DEPENDENCIES 
  None.

RETURN VALUE  
  TRUE - If packet container appended succesfully
  FALSE- Error

SIDE EFFECTS 
  None.
===========================================================================*/
boolean ds_pdn_mgr_fill_pco_container_info
(
  ds_pdn_protocol_cfg_options_s  *protocol_config_options_p,
  uint16                           container_id,
  ds_pdn_protocol_id_s          **proto_info
)
{
  uint16                 proto_info_index;
  uint8                  max_num_proto_ids = 0;
  ds_pdn_protocol_id_s *temp_proto_info = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Get the protocol info index. This index is used to fill in PPP packets
    in the PDP context req. msg
  -------------------------------------------------------------------------*/
  proto_info_index =  protocol_config_options_p->num_protocol_ids;  

  /*-------------------------------------------------------------------------
    Check if we have room to store this protocol info pkt in the pdp 
    context request
  -------------------------------------------------------------------------*/
  max_num_proto_ids = ds_pdn_pco_epco_hdlr_get_max_number_of_protocol_ids();
  if(proto_info_index >= max_num_proto_ids)
  {
    DS_ERROR_LOG_1("No room to store requested container =d in PCO",
    	              container_id);
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Allocate the memory to store the new protocol ID info
  -------------------------------------------------------------------------*/
  DS_SYSTEM_HEAP_MEM_ALLOC(temp_proto_info, sizeof(ds_pdn_protocol_id_s),
                            ds_pdn_protocol_id_s*);
  *proto_info = temp_proto_info;
  
  if ( NULL == temp_proto_info )
  {
    return FALSE;
  }
  
  memset(temp_proto_info, 0, sizeof(ds_pdn_protocol_id_s));
  
  /*-------------------------------------------------------------------------
    Indicates if the option is valid or not.
  -------------------------------------------------------------------------*/  
  protocol_config_options_p->valid = TRUE;

  /*---------------------------------------------------------------------
   Store the MSB of the container id field
  ---------------------------------------------------------------------*/  
  temp_proto_info->protocol_id[0] = ((container_id & 0xff00) >> 8);

  /*---------------------------------------------------------------------
   Store the LSB of the container id field
  ---------------------------------------------------------------------*/  
  temp_proto_info->protocol_id[1] = (container_id & 0x00ff );

  temp_proto_info->protocol_len = 0;  
  protocol_config_options_p->protocol_info_p[proto_info_index] = 
    temp_proto_info;

  proto_info_index++;
  protocol_config_options_p->num_protocol_ids = proto_info_index;

  DS_MSG1(MSG_LEGACY_HIGH,
  	       "Included container %d in orig params", container_id);
  return TRUE;
}/* ds_pdn_mgr_fill_pco_container_info */

/*===========================================================================
FUNCTION DS_PDN_MGR_FILL_PAP_ORIG_PARMS

DESCRIPTION
  This function fills the PAP specific protocol information in the PDP context
  req. message

PARAMETERS   :
    protocol_cfg_options_T *protocol_config_options_p 
    *username            - Ptr to User id
    user_id_len          - Length of User id
    *password            - Ptr to password
    password_len         - Length of password

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - PAP packet generated succesfully and included in PDP ctx req msg
  FALSE- Error

SIDE EFFECTS
  Updates Orig_params contents :
  orig_params_ptr->config_options.num_protocol_ids will be updated
  to reflect the no. of PPP packets included by this function
===========================================================================*/
boolean ds_pdn_mgr_fill_pap_orig_parms
(
  ds_pdn_protocol_cfg_options_s  *protocol_config_options_p,
  const byte                   *username,
  byte                         user_id_len,
  const byte                   *password,
  byte                         password_len
)
{
  int                     pap_pkt_len=0;   /* Length of PAP packet               */
  ds_pdn_protocol_id_s  *temp_proto_info = NULL;
  byte                    protocol_contents[MAX_PROTOCOL_ID_CONTENT] = {0};
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Encode a PAP packet based on user specified username & password and
    include it in protocol options field
  -------------------------------------------------------------------------*/
  pap_pkt_len = ds_pdn_ppp_gen_pap_auth_req_pkt(
    protocol_contents,
    MAX_PROTOCOL_ID_CONTENT,
    0,
    username,
    user_id_len,
    password,
    password_len
   );

  if(pap_pkt_len < 0 )
  {
    return FALSE;
  }
#define PPP_PAP_PROTOCOL      0xc023  /* Password Authentication Protocol  */

  if( ds_pdn_mgr_fill_pco_container_info(protocol_config_options_p, 
  	                                      PPP_PAP_PROTOCOL,
  	                                      &temp_proto_info) == FALSE )
  {
    return FALSE;
  } 	                                      

  /* Copy the protocol contents and length generated by PAP */
  memscpy(temp_proto_info->protocol_contents, 
          sizeof(protocol_contents),
          protocol_contents,
          sizeof(protocol_contents));  
  temp_proto_info->protocol_len = pap_pkt_len;
  
  return TRUE;
} /* ds_pdn_mgr_fill_pap_orig_parms() */

/*===========================================================================
FUNCTION DS_PDN_MGR_FILL_CHAP_ORIG_PARMS

DESCRIPTION
  This function fills the CHAP specific protocol information in 
  the PDP context req. message. The chap challenge and response are passed in
    
PARAMETERS   :
    protocol_config_options_p  - Ptr to config options 
    chal_str_ptr      - pointer to challenge generated by the challenger
    chal_str_len      - length of challenge
    chal_name_ptr     - pointer to the name string of the challenger
    chal_name_len     - length of the challenger's name
    digest_str_ptr    - pointer to md5 digest (challenge response) string
    digest_str_len    - length of the digest
    resp_str_ptr      - pointer to responders name
    resp_str_len      - length og the responders name
    generate_flag    - Whether CHAP response needs to be generated. 
    
DEPENDENCIES
  None.
  
RETURN VALUE 
  TRUE - CHAP packet generated succesfully and included in PDP ctx req msg
  FALSE- Error
  
SIDE EFFECTS 
  None.  
===========================================================================*/
static boolean ds_pdn_mgr_fill_chap_orig_params
(
  ds_pdn_protocol_cfg_options_s  *protocol_config_options_p,
  const byte                      *chal_str_ptr,
  byte                             chal_str_len,
  const byte                      *chal_name_ptr,
  byte                             chal_name_len,
  const byte                      *digest_str_ptr,
  byte                             digest_str_len,
  const byte                      *resp_str_ptr,
  byte                             resp_str_len,
  byte                             auth_id,
  boolean                          generate_flag
)
{
  int     chap_chal_pkt_len,chap_res_pkt_len;
  ds_pdn_protocol_id_s  *temp_proto_info = NULL;
  byte                    protocol_contents[MAX_PROTOCOL_ID_CONTENT] = {0};
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /*-------------------------------------------------------------------------
    Construct the challenge packet. Pass the parameters received
    to  construct the CHAP challenge
  -------------------------------------------------------------------------*/
  chap_chal_pkt_len = ds_pdn_ppp_gen_chap_challenge_pkt
    (
    protocol_contents,
    MAX_PROTOCOL_ID_CONTENT,
    auth_id,
    chal_str_ptr,
    chal_str_len,
    chal_name_ptr,
    chal_name_len
    );

  /*-------------------------------------------------------------------------
    If an error occurs in encoding pkt, abort further processing and 
    return an error
  -------------------------------------------------------------------------*/
  if( chap_chal_pkt_len < 0 )
  {
    return FALSE;
  }

#define PPP_CHAP_PROTOCOL     0xc223  /* Challenge Handshake Auth Protocol */

  if( ds_pdn_mgr_fill_pco_container_info(protocol_config_options_p, 
  	                                      PPP_CHAP_PROTOCOL,
  	                                      &temp_proto_info) == FALSE )
  {
    return FALSE;
  } 	                                      

  /* Copy the protocol contents and length generated by PAP */
  memscpy(temp_proto_info->protocol_contents, 
          sizeof(protocol_contents),
          protocol_contents,
          sizeof(protocol_contents));  
  temp_proto_info->protocol_len   = (uint8)chap_chal_pkt_len;

  /*-------------------------------------------------------------------------
    Construct the response packet. 
  -------------------------------------------------------------------------*/
  memset(protocol_contents, 0, sizeof(protocol_contents));
  temp_proto_info = NULL;

  if(generate_flag)
  {
    chap_res_pkt_len = ds_pdn_ppp_gen_chap_response_pkt
      (
      protocol_contents,
      MAX_PROTOCOL_ID_CONTENT,
      auth_id,
      chal_str_ptr,
      chal_str_len,
      digest_str_ptr,
      digest_str_len,
      resp_str_ptr,
      resp_str_len
      );
  }
  else
  {
    chap_res_pkt_len = ds_pdn_ppp_fill_chap_response_pkt
      (
      protocol_contents,
      MAX_PROTOCOL_ID_CONTENT,
      auth_id,
      digest_str_ptr,
      digest_str_len,
      resp_str_ptr,
      resp_str_len
      );
  }
  
  /*-------------------------------------------------------------------------
    If an error occurs in encoding pkt, abort further processing.
    Adjust the protocol_info_index : remove the chap challenge
    packet which has been included.
  -------------------------------------------------------------------------*/
  if( chap_res_pkt_len <0 )
  {
    protocol_config_options_p->num_protocol_ids --; 
    
    /* Free the memory allocated for the previous protocol ID */
    temp_proto_info = 
      protocol_config_options_p->
        protocol_info_p[protocol_config_options_p->num_protocol_ids];
    DS_SYSTEM_HEAP_MEM_FREE(temp_proto_info);
    protocol_config_options_p->
        protocol_info_p[protocol_config_options_p->num_protocol_ids] = NULL;
    return FALSE;
  }

  if( ds_pdn_mgr_fill_pco_container_info(protocol_config_options_p, 
  	                                      PPP_CHAP_PROTOCOL,
  	                                      &temp_proto_info) == FALSE )
  {
    return FALSE;
  } 	                                      

  /* Copy the protocol contents and length generated by PAP */
  memscpy(temp_proto_info->protocol_contents, 
          sizeof(protocol_contents),
          protocol_contents,
          sizeof(protocol_contents));  
  temp_proto_info->protocol_len   = (uint8)chap_res_pkt_len;

  return TRUE;      
} /* ds_pdn_cntxt_fill_chap_orig_params */

/*===========================================================================
FUNCTION DS_PDN_MGR_GEN_AND_FILL_CHAP_ORIG_PARMS

DESCRIPTION
  This function generates and fills the CHAP specific protocol information in
  the PDP context req. message. The chap challenge and response are generated
  internally.

PARAMETERS   :
    protocol_cfg_options_T       *protocol_config_options_p
    *username            - Ptr to User id
    user_id_len          - Length of User id
    *password            - Ptr to password
    password_len         - Length of password

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - CHAP packet generated succesfully and included in PDP ctx req msg
  FALSE- Error

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_pdn_mgr_gen_and_fill_chap_orig_parms
(
  ds_pdn_protocol_cfg_options_s  *protocol_config_options_p,
  const byte                   *username,
  byte                         user_id_len,
  const byte                   *password,
  byte                        password_len
)
{
  qword    qw_time;
  uint32   dsrand,i;
  uint8    chap_challenge_buffer[CHAP_CHALLENGE_LEN] = {0};  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 	
  /*-------------------------------------------------------------------------
    Generate a random stream to be used as the challenge., Use the
    32 bit clock tick count repeatedly to form a challenge pattern
  -------------------------------------------------------------------------*/
  time_get_ms( qw_time);   
  dsrand = qw_time[0];
  
  for(i=0; i < CHAP_CHALLENGE_LEN - 3 ; i+=4 )
  {
    chap_challenge_buffer[i]   = (uint8)( dsrand        & 0xff);
    chap_challenge_buffer[i+1] = (uint8)( (dsrand >>8)  & 0xff); 
    chap_challenge_buffer[i+2] = (uint8)( (dsrand >>16) & 0xff);
    chap_challenge_buffer[i+3] = (uint8)( (dsrand >>16) & 0xff);
  } 

  return ds_pdn_mgr_fill_chap_orig_params 
                 (
                  protocol_config_options_p,
                  (byte*)&chap_challenge_buffer[0],
                  (byte)CHAP_CHALLENGE_LEN,
                  (byte *)username,
                  (byte)user_id_len,
                  (byte*)password,
                  (byte)password_len,
                  (byte *)username,
                  (byte)user_id_len,
                  0,
                  TRUE
                 );
} /* ds_pdn_mgr_gen_and_fill_chap_orig_parms() */

/*===========================================================================
FUNCTION DS_PDN_MGR_FILL_IPCP_ORIG_PARAMS

DESCRIPTION
  This function fills the IPCP (Primary and Secondary DNS only) specific 
  protocol information in the PDP context request message.
  
PARAMETERS 
    *orig_params_ptr     - Ptr to PDN origination parameters 
    req_spec_dns_flg     - IS dns address requested
    primary_dns          - primary dns address requested
    secondary_dns        - secondary dns address requested
    req_spec_nbns_flg    - IS nbns address requested
    primary_nbns         - primary WINS address
    secondary_nbns       - secondary WINS address
                               
DEPENDENCIES 
  None.

RETURN VALUE  
  TRUE - IPCP packet generated succesfully and included in PDP context request
         message
  FALSE- Error
  
SIDE EFFECTS 
  Updates Orig_params contents : 
     orig_params_ptr->protocol_config_options.num_protocol_ids will be updated
     to reflect the no. of PPP packets included by this function
     
===========================================================================*/
boolean ds_pdn_mgr_fill_ipcp_orig_params
(
  ds_pdn_protocol_cfg_options_s  *protocol_config_options_p,
  boolean                       req_spec_dns_flg,/* Should spec DNS add req*/
  uint32                        primary_dns,    /* primary DNS             */
  uint32                        secondary_dns,  /* secondary DNS           */
  net_cfg_params_mask_e_type    valid_fields    /* IPCP fileds to request */
)
{
  int    ipcp_pkt_len=0;                          /* Length of IPCP packet */
  uint32 p_dns_req,s_dns_req;
  ds_pdn_protocol_id_s  *temp_proto_info = NULL;
  byte                    protocol_contents[MAX_PROTOCOL_ID_CONTENT] = {0};
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  /*-------------------------------------------------------------------------
    Should we ask for the DNS addr specified or the values 0 implying that
    net should assin us one. If laptop specified a specific DNS addr in
    ipcp, we should include it, but for socks call, we always get it from
    the network first
  -------------------------------------------------------------------------*/
  if ( ! req_spec_dns_flg )
  {
    p_dns_req = s_dns_req = 0x0;
  }
  else
  {
    p_dns_req = primary_dns;
    s_dns_req = secondary_dns;
  }

  /*-------------------------------------------------------------------------
    Encode a IPCP packet with Primary and Secondary DNS and NBNS request 
    options and include it in protocol options field.
  -------------------------------------------------------------------------*/
  ipcp_pkt_len= ds_pdn_gen_ipcp_req_pkt(
    protocol_contents,
    MAX_PROTOCOL_ID_CONTENT,
    0,
    p_dns_req,
    s_dns_req,
    valid_fields
  );
  
  if(ipcp_pkt_len < 0 )
  {
    return FALSE;
  }

#define PPP_IPCP_PROTOCOL     0x8021  /* Internet Protocol Control Protocol*/

  if( ds_pdn_mgr_fill_pco_container_info(protocol_config_options_p, 
  	                                      PPP_IPCP_PROTOCOL,
  	                                      &temp_proto_info) == FALSE )
  {
    DS_ERROR_LOG_0("Error generating IPCP packet");
    return FALSE;
  } 	                                      

  /* Copy the protocol contents and length generated by PAP */
  memscpy(temp_proto_info->protocol_contents, 
          sizeof(protocol_contents),
          protocol_contents,
          sizeof(protocol_contents));

  /*-------------------------------------------------------------------------
    Store the length of the protocol identifier contents
  -------------------------------------------------------------------------*/  
  temp_proto_info->protocol_len = (uint8)ipcp_pkt_len;  
  return TRUE;
} /* ds_pdn_mgr_fill_ipcp_orig_params() */

/*===========================================================================
FUNCTION DS_PDN_MGR_FILL_IPV4_DNS_CONTAINER

DESCRIPTION
  This function appends DNS server Ipv4 address request container to the 
  PCO buffer. 
  
PARAMETERS   
  protocol_cfg_options_T       *protocol_config_options_p
    
DEPENDENCIES 
  None.

RETURN VALUE  
  TRUE - IPV4 DNS packet container appended succesfully
  FALSE- Error

SIDE EFFECTS 
  None.
===========================================================================*/
boolean ds_pdn_mgr_fill_ipv4_dns_container
(
  ds_pdn_protocol_cfg_options_s  *protocol_config_options_p
)
{
  ds_pdn_protocol_id_s  *temp_proto_info = NULL;
  /*--------------------------------------------------------*/

  return ds_pdn_mgr_fill_pco_container_info(protocol_config_options_p, 
    	                                       IPV4_DNS_CONTAIN_ID,
  	                                         &temp_proto_info);
}

/*===========================================================================
FUNCTION DS_PDN_MGR_FILL_IPV6_DNS_PARAMS

DESCRIPTION
  This function fills the IPV6 (DNS only) specific protocol information 
  in the PDP context request message.
  
PARAMETERS 
    protocol_cfg_options_T       *protocol_config_options_p
                               
DEPENDENCIES 
  None.

RETURN VALUE  
  TRUE - IPV6 DNS packet generated succesfully and included in PDP context 
  request message
  FALSE- Error
  
SIDE EFFECTS 
  Updates Orig_params contents : 
     orig_params_ptr->protocol_config_options.num_protocol_ids will be updated
     to reflect the no. of protocol config options included by this function
     
===========================================================================*/
boolean ds_pdn_mgr_fill_ipv6_dns_params
(
  ds_pdn_protocol_cfg_options_s  *protocol_config_options_p
)
{
  ds_pdn_protocol_id_s  *temp_proto_info = NULL;
/*--------------------------------------------------------*/

  return ds_pdn_mgr_fill_pco_container_info(protocol_config_options_p, 
     	                                      IPV6_DNS_CONTAIN_ID,
  	                                         &temp_proto_info);
} /* ds_pdn_mgr_fill_ipv6_dns_params() */

/*===========================================================================
FUNCTION DS_PDN_MGR_FILL_AUTH_PARAMS_FROM_INFO

DESCRIPTION
  This function fills the authentication information obtained from the info_ptr
  in the instance up cmd hdlr.
  
PARAMETERS 
  *protocol_config_options_p    - config option that has to be filled
  *profile_p,                   - PDP profile
  *info_p                   - info pointer
                               
DEPENDENCIES 
  None.

RETURN VALUE  
  TRUE - Auth info successfully included in PDP context request
         message
  FALSE- Auth info not included in PDP context request.
  
SIDE EFFECTS 
  None     
===========================================================================*/
boolean ds_pdn_mgr_fill_auth_params_from_info
(
  ds_pdn_protocol_cfg_options_s        *protocol_config_options_p,
  ds_profile_type_s                     *profile_p, 
  const ds_pdn_network_params_info_type *ppp_info_p
)
{
  boolean                   result = FALSE;
  ds_profile_auth_enum_type auth_type_to_use;


  ds_pdn_auth_mgr_get_auth_type
    (profile_p->context.apn,
    (ds_profile_auth_enum_type)ppp_info_p->auth_info.auth_type,
    &auth_type_to_use);

  if(auth_type_to_use == DS_PROFILE_AUTH_CHAP_OR_PAP) 
  {
    if(DS_PDN_AUTH_CHAP == ds_pdn_nv_manager_get_config_pdn_level_auth())
    {
      auth_type_to_use = DS_PROFILE_AUTH_CHAP;
    }
    else
    {
      auth_type_to_use = DS_PROFILE_AUTH_PAP;
    }
  }
  
  DS_MSG1(MSG_LEGACY_HIGH, "Auth specfied: %d", auth_type_to_use);

  switch (auth_type_to_use) 
  {
    case DS_PROFILE_AUTH_NONE:
      /* Returning failure ensures that auth info is not sent out for eps */
      return FALSE;
      

    case DS_PROFILE_AUTH_PAP:
      result = ds_pdn_mgr_fill_pap_orig_parms 
                (
                 protocol_config_options_p,
                 (byte*)&ppp_info_p->auth_info.params.user_id_info[0],
                 (byte)ppp_info_p->auth_info.params.user_id_len,
                 (byte*)&ppp_info_p->auth_info.params.passwd_info[0],
                 (byte)ppp_info_p->auth_info.params.passwd_len
                );

      if(!result )
      {
        return FALSE;
      }
      else
      { 
        /*-------------------------------------------------------------------
          If the authentication type, username and password are used from
          the ppp info, then store these parameters in the pdp profile
          params.
        -------------------------------------------------------------------*/
        (void)strlcpy((char*)profile_p->auth.username,
              (char*)ppp_info_p->auth_info.params.user_id_info,
                sizeof(profile_p->auth.username));

        (void)strlcpy((char*)profile_p->auth.password,
                (char*)ppp_info_p->auth_info.params.passwd_info,
                sizeof(profile_p->auth.password));
      }
      break;

    case DS_PROFILE_AUTH_CHAP:
      if ((ppp_info_p->valid_fields & NET_CFG_PRM_CHAP_CHAL_INFO_MASK)!= 0)
      /*lint +e655 */
      {     
          result = ds_pdn_mgr_fill_chap_orig_params 
                 (
                  protocol_config_options_p,
                  (byte*)&ppp_info_p->auth_info.params.challenge_info[0],
                  (byte)ppp_info_p->auth_info.params.challenge_len,
                  (byte*)&ppp_info_p->auth_info.params.challenge_name[0],
                  (byte)ppp_info_p->auth_info.params.challenge_name_len,
                  (byte*)&ppp_info_p->auth_info.params.passwd_info[0],
                  (byte)ppp_info_p->auth_info.params.passwd_len,
                  (byte*)&ppp_info_p->auth_info.params.user_id_info[0],
                  (byte)ppp_info_p->auth_info.params.user_id_len,
                  (byte)ppp_info_p->auth_info.auth_id,
                  FALSE
                 );
      }
      else
      {
        result = ds_pdn_mgr_gen_and_fill_chap_orig_parms 
                   (
                    protocol_config_options_p,
                    (byte*)&ppp_info_p->auth_info.params.user_id_info[0],
                    (byte)ppp_info_p->auth_info.params.user_id_len,
                    (byte*)&ppp_info_p->auth_info.params.passwd_info[0],
                    (byte)ppp_info_p->auth_info.params.passwd_len
                   );
      }

      if(! result )
      {
        return FALSE;
      }
      else
      {
        /*-------------------------------------------------------------------
         If the authentication type, username and password are used from
         the ppp info, then store these parameters in the pdp profile
         params.
        -------------------------------------------------------------------*/
        (void)strlcpy((char*)profile_p->auth.username,
                (char*)ppp_info_p->auth_info.params.user_id_info,
                sizeof(profile_p->auth.username));

        (void)strlcpy((char*)profile_p->auth.password,
                (char*)ppp_info_p->auth_info.params.passwd_info,
                sizeof(profile_p->auth.password));
      }
      break;

    default:
      return FALSE;
  }
  
  return TRUE;
}/* ds_pdn_mgr_fill_auth_params_from_info */

/*===========================================================================
FUNCTION DS_PDN_MGR_FILL_AUTH_PARAMS_FROM_PROFILE

DESCRIPTION
  This function fills the authentication information obtained from profile
  in the PDP context request message.
  
PARAMETERS 
  *protocol_config_options_p    - Config option that has to be filled
  *profile_p,                   - PDP profile
   subs_id                      - Subscription id
 
DEPENDENCIES 
  None.

RETURN VALUE  
  TRUE - Auth info successfully included in PDP context request
         message
  FALSE- Error
  
SIDE EFFECTS 
    
===========================================================================*/
boolean ds_pdn_mgr_fill_auth_params_from_profile
(
  ds_pdn_protocol_cfg_options_s   *protocol_config_options_p,
  const ds_profile_type_s          *profile_p
)
{
  boolean result;
  ds_profile_auth_enum_type auth_type_to_use;

  ds_pdn_auth_mgr_get_auth_type(profile_p->context.apn,
                                 profile_p->auth.auth_type,
                                 &auth_type_to_use);
  
  if(auth_type_to_use == DS_PROFILE_AUTH_CHAP_OR_PAP) 
  {
    if(DS_PDN_AUTH_CHAP == ds_pdn_nv_manager_get_config_pdn_level_auth())
    {
      auth_type_to_use = DS_PROFILE_AUTH_CHAP;
    }
    else
    {
      auth_type_to_use = DS_PROFILE_AUTH_PAP;
    }
  }

  DS_MSG1(MSG_LEGACY_HIGH, "Auth specfied: %d", auth_type_to_use);

  switch (auth_type_to_use) 
  {
    case DS_PROFILE_AUTH_NONE:
      return FALSE;

    case DS_PROFILE_AUTH_PAP:
      result = ds_pdn_mgr_fill_pap_orig_parms
                (
                  protocol_config_options_p,
                  (byte*) profile_p->auth.username,
                  (byte)strlen((char*)profile_p->auth.username),
                  (byte*) profile_p->auth.password,
                  (byte)strlen((char*)profile_p->auth.password)
                );

      if(! result )
      {
        return FALSE;
      }
      break;

    case DS_PROFILE_AUTH_CHAP:
      result = ds_pdn_mgr_gen_and_fill_chap_orig_parms
                 (
                  protocol_config_options_p,
                  (byte*) profile_p->auth.username,
                  (byte)strlen((char*)profile_p->auth.username),
                  (byte*) profile_p->auth.password,
                  (byte)strlen((char*)profile_p->auth.password)
                 );

      if(! result )
      {
        return FALSE;
      }
      break;

    default:
      return FALSE;
  }/* switch */
  
  return TRUE;
}

/*===========================================================================
FUNCTION DS_PDN_MGR_FILL_MCC_MNC_TO_PCO

DESCRIPTION
  This function fills the mcc/mnc to PCO
    
PARAMETERS   :
    ds_pdn_protocol_id_s  - Ptr to config options 
    MCC
    MNC
    mnc_includes_pcs_digit
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  TRUE - Successfully filled the operator info
  FALSE- Error
  
SIDE EFFECTS 
  None.
===========================================================================*/
static boolean ds_pdn_mgr_fill_mcc_mnc_to_pco
(
  ds_pdn_protocol_id_s           *proto_info,
  uint16                           mcc,
  uint16                           mnc_digits,
  boolean                          mnc_includes_pcs_digit
)
{
  uint8 mcc_digit_1,mcc_digit_2,mcc_digit_3;
  uint8 mnc_digit_1,mnc_digit_2,mnc_digit_3;
  uint8 temp ;
  nv_ehrpd_imsi_type   imsi_data;
 /*------------------------------------------------------------------------*/

  temp = 0;
  mcc_digit_1 = mcc_digit_2 = mcc_digit_3 = 0;
  mnc_digit_1 = mnc_digit_2 = mnc_digit_3 = 0;
 
  /* If mcc and mnc are both zero; then we need to get the mcc,mnc value from the sim card else
     use the values from the profile.*/
  if (mcc==0 && mnc_digits==0 )
  {
    memset(&imsi_data, 0, sizeof(nv_ehrpd_imsi_type));
 
    if (0)//FALSE == ds3g_mmgsdi_get_imsi_data(&imsi_data))    // write APi for this
    {
      DS_ERROR_LOG_0("Failed to read IMSI data from SIM");
      return FALSE;
    }
 
    if (imsi_data.num_of_mnc_digits == 2)
    {
      imsi_data.ehrpd_mnc[2] = 0x0F;
    }
    
    mcc_digit_1= imsi_data.ehrpd_mcc[0];
    mcc_digit_2= imsi_data.ehrpd_mcc[1];
    mcc_digit_3= imsi_data.ehrpd_mcc[2];
 
    mnc_digit_1= imsi_data.ehrpd_mnc[0];
    mnc_digit_2= imsi_data.ehrpd_mnc[1];
    mnc_digit_3= imsi_data.ehrpd_mnc[2];
  }
  else
  {
    /* The mcc is supposed to be a 3 digit number is thus divided by 100 to get the mcc_digit_1.
       Similarly  we  get mcc_digit_2 & mcc_digit_3 by appropriately removing the hundred's place and ten's place
       respectively */

    mcc_digit_1 = mcc /100;
    temp= mcc -(100*mcc_digit_1);
    mcc_digit_2=  temp/10;
    mcc_digit_3= mcc - (100*mcc_digit_1 + 10*mcc_digit_2);

   /* The mnc is supposed to be a 3 or 2 digit number is thus divided by 100 to get the mcc_digit_1.
   Similarly  we  get mnc_digit_2 & mnc_digit_3 by appropriately removing the hundred's place and ten's place
   respectively */

    mnc_digit_1 = mnc_digits /100;
    temp= mnc_digits -(100*mnc_digit_1);
    mnc_digit_2=  temp/10;
    mnc_digit_3= mnc_digits - (100*mnc_digit_1 + 10*mnc_digit_2);


    /*If the mnc_digit_1=0 amd 
      mnc_includes_pcs_digit is FALSE  Example: 34 will be considered as 
     mnc_includes_pcs_digit is  TRUE  Example: 34 will be considered as  034 */
    if (mnc_digit_1==0 && !(mnc_includes_pcs_digit))
    {
      mnc_digit_1=mnc_digit_2;
      mnc_digit_2=mnc_digit_3;
      mnc_digit_3=0x0F; 
    }

  }

   /*---------------------------------------------------------------------- 
       3 Octets: How we encode mcc/mnc as part of Pco contents:"
        OCTET1:-   mcc_digit_2  mcc_digit_1
        OCTET2:-   mnc_digit_3  mcc_digit_3
        OCTET3:-   mnc_digit_2  mnc_digit_1
   -----------------------------------------------------------------------*/

  proto_info->protocol_contents[0] = mcc_digit_1 | (mcc_digit_2 << 4);
  proto_info->protocol_contents[1] = mcc_digit_3 | (mnc_digit_3 << 4);
  proto_info->protocol_contents[2]= mnc_digit_1 | (mnc_digit_2 << 4);
  proto_info->protocol_len = MCC_MNC_LENGTH;

  return TRUE;
}

/*===========================================================================
FUNCTION DS_PDN_MGR_FILL_OPER_PCO_INFO_FROM_PROFILE

DESCRIPTION
  This function fills the operator specific protocol information in 
  the PDP context req. message. 
    
PARAMETERS   :
    protocol_config_options_p  - Ptr to config options 
    profile_p - Prt to the profile info    
DEPENDENCIES
  None.
  
RETURN VALUE 
  TRUE - Successfully filled the operator info
  FALSE- Error
  
SIDE EFFECTS 
  
===========================================================================*/
boolean ds_pdn_mgr_fill_oper_pco_info_from_profile
(
  ds_pdn_protocol_cfg_options_s  *protocol_config_options_p,
  ds_profile_type_s               *profile_p
)
{

  uint16  mcc = 0,mnc_digits = 0;
  ds_pdn_protocol_id_s  *temp_proto_info = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ((profile_p == NULL) || 
      (protocol_config_options_p == NULL))
  {
    return FALSE;
  }

  if (profile_p->operator_reserved_pco < DS_OPERATOR_RESERVED_PCO_ID_RANGE_BEGIN) 
  {
    return FALSE; 
  }

  if( ds_pdn_mgr_fill_pco_container_info(protocol_config_options_p, 
  	                                      profile_p->operator_reserved_pco,
  	                                      &temp_proto_info) == FALSE )
  {
    return FALSE;
  } 	                                      

  DS_MSG3(MSG_LEGACY_HIGH,
  	       "Fill OPER PCO params Mcc: %d Mnc: %d Operator Reserved Pco %d ",
  	       profile_p->mcc, profile_p->mnc.mnc_digits,
  	       profile_p->operator_reserved_pco);

  mcc        = profile_p->mcc;
  mnc_digits = profile_p->mnc.mnc_digits;

  ds_pdn_mgr_fill_mcc_mnc_to_pco(temp_proto_info,mcc,mnc_digits,
                                 profile_p->mnc.mnc_includes_pcs_digit);
  return TRUE;
}

#ifdef FEATURE_DATA_IMS
/*===========================================================================
FUNCTION DS_PDN_MGR_GEN_PCSCF_ADDRESS_REQUEST_PACKET

DESCRIPTION
  This function generates a PCSCF request packet to request the 
  pcscf address.

PARAMETERS   
  orig_params_ptr  - Pointer to the buffer to store the pcscf request packet
    
DEPENDENCIES 
  None.

RETURN VALUE  
  DS3G_SUCCESS - packet generated succesfully
  DS3G_FAILURE - pkt not generated
  
SIDE EFFECTS 
  None.
===========================================================================*/
boolean ds_pdn_mgr_gen_pcscf_address_request_packet
(
  ds_pdn_protocol_cfg_options_s  *protocol_config_options_p,
  const ds_profile_type_s         *profile_params /* profile parameters */
)
{
  boolean                 request_pcscf_flag = FALSE;
  uint8                   rel_version = 0;
  ds_pdn_apm_req_pay_load req_payload = {0};
  ds_pdn_apm_ext_error_code   dsd_result = DS_PDN_APM_UNSEPCIFIED;
  boolean                 sip_v4_pcscf_addr_req = FALSE;
  boolean                 sip_v6_pcscf_addr_req = FALSE;
  ds_pdn_protocol_id_s  *temp_proto_info = NULL;
  int8                    profile_id = INVALID_PROFILE_NUMBER;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  //Get the attach APN first.
  dsd_result = ds_pdn_apm_req(DS_PDN_APM_ATTCH_REQ, &req_payload);

  if (dsd_result == DS_PDN_APM_SUCCESS)
  {
    profile_id = ds_pdn_mgr_get_profile_id(profile_params->context);	
    if ( (INVALID_PROFILE_NUMBER != profile_id) &&
    	    (req_payload.profile_id == profile_id) )
    {
      //The profile that we are querying about is the current attach APN.
      req_payload.rule_type = DS_PDN_APM_ATTACH_RULE;
    }
  }
  else 
  {
    //The profile that we are querying on is not the current attach APN.
    //So its an on demand PDN.
    req_payload.rule_type = DS_PDN_APM_ON_DEMAND_RULE;
  }
  
  /* Get the PCSCF rules from APM */  
  req_payload.profile_id = profile_id;
  (void)ds_pdn_apm_req( DS_PDN_APM_MANDATORY_PCSCF_REQ , 
                        &req_payload );
  DS_MSG1(MSG_LEGACY_HIGH, "APM response to MANDATORY_PCSCF_REQ is %d",
          req_payload.mandatory_pcscf); 

  request_pcscf_flag = profile_params->request_pcscf_address_flag;

  /*-------------------------------------------------------------------------
    Check if we have to fill this protocol info pkt in the pdp 
    context request based on NV item 3gpp_rel_support,
    if mode is not LTE. For LTE always fill this container.
  -------------------------------------------------------------------------*/
  if ( ds_pdn_hdlr_get_current_network_mode() != SYS_SYS_MODE_LTE )
  {
    rel_version = ds_pdn_nv_manager_get_3gpp_rel_version();
    if ( rel_version < (uint8)DS_3GPP_REL8 )
    {
      /* No need to generate pcscf address request; return SUCCESS from here */
      DS_MSG0(MSG_LEGACY_HIGH, "PCSCF addr container not filled"); 
      return TRUE;
    }
  }

  /* If V6 is mandatory in the APM rules or the profile is configured to
     request it; then include the request packet.*/
  if ( ipv6_mandatory_and_ipv4_donotcare == req_payload.mandatory_pcscf ||
       ipv4_mandatory_and_ipv6_mandatory == req_payload.mandatory_pcscf ||
       ipv4_mandatory_or_ipv6_mandatory == req_payload.mandatory_pcscf ||
       ( ( TRUE == request_pcscf_flag ) &&
         ( DS_PROFILE_PDP_IPV6 == profile_params->context.pdp_type ||
           DS_PROFILE_PDP_IPV4V6 == profile_params->context.pdp_type ) ) )
  {
    if( ds_pdn_mgr_fill_pco_container_info(protocol_config_options_p, 
                                           SIP_PCSCF_V6_ADDRESS_PROTOCOL,
                                           &temp_proto_info) == FALSE )
    {
      return FALSE;
    }                                        

    sip_v6_pcscf_addr_req = TRUE;
  }

  /* If V4 is mandatory in the APM rules or the profile is configured to
     request it; then include the request packet.*/
  if ( ipv4_mandatory_and_ipv6_donotcare == req_payload.mandatory_pcscf ||
       ipv4_mandatory_and_ipv6_mandatory == req_payload.mandatory_pcscf ||
       ipv4_mandatory_or_ipv6_mandatory == req_payload.mandatory_pcscf ||
       ( ( TRUE == request_pcscf_flag ) &&
         ( DS_PROFILE_PDP_IPV4 == profile_params->context.pdp_type ||
           DS_PROFILE_PDP_IPV4V6 == profile_params->context.pdp_type ) ) )  
  {
    temp_proto_info = NULL;
    if( ds_pdn_mgr_fill_pco_container_info(protocol_config_options_p, 
                                           SIP_PCSCF_V4_ADDRESS_PROTOCOL,
                                           &temp_proto_info) == FALSE )
    {
      return FALSE;
    }  
    
    sip_v4_pcscf_addr_req = TRUE;
  }

  /*-------------------------------------------------------------------------- 
    Request PCSCF RESELECTION if
    REL 12 is enabled, profile param is set and only if either
    sip_v4_pcscf_addr or sip_v6_pcscf_addr is requested
  --------------------------------------------------------------------------*/
  if ((ds_pdn_nv_manager_is_rel_12_or_above_supported()) && 
      (sip_v4_pcscf_addr_req || sip_v6_pcscf_addr_req))
  {
    temp_proto_info = NULL;
    if( ds_pdn_mgr_fill_pco_container_info(protocol_config_options_p, 
                                           SIP_PCSCF_RESELECTION_SUPPORT_PROTOCOL,
                                           &temp_proto_info) == FALSE )
    {
      return FALSE;
    }  
  }

  return TRUE;
} /* ds_pdn_cntxt_gen_pcscf_address_request_packet */


/*===========================================================================
FUNCTION DS_PDN_MGR_GEN_IMCN_SUBSYSTEM_REQUEST_PACKET

DESCRIPTION
  This function generates a IMCN request packet to request the 
  signaling only context.

PARAMETERS   
  orig_params_ptr  - Pointer to the buffer to store the IMCN request packet
    
DEPENDENCIES 
  None.

RETURN VALUE  
  TRUE  - packet generated succesfully
  FALSE - pkt not generated
  
SIDE EFFECTS 
  None.
===========================================================================*/
boolean ds_pdn_mgr_gen_imcn_subsystem_request_packet
(
  ds_pdn_protocol_cfg_options_s  *protocol_config_options_p 
)
{
  ds_pdn_protocol_id_s  *temp_proto_info = NULL;

  if( ds_pdn_mgr_fill_pco_container_info(protocol_config_options_p, 
  	                                      IM_CN_SUBSYSTEM_PROTOCOL,
  	                                      &temp_proto_info) == FALSE )
  {
    return FALSE;
  } 	                                      

  return TRUE;
} /* ds_pdn_mgr_gen_imcn_subsystem_request_packet */
#endif // FEATURE_DATA_IMS

/*===========================================================================
FUNCTION DS_PDN_MGR_FILL_ADDRESS_ALLOC_PARAM

DESCRIPTION
  This function fills the IPV4 address allocation container in PCO
  
PARAMETERS 
    protocol_cfg_options_T    *protocol_config_options_p 
    *profile_p           - Ptr to ds profile
                               
DEPENDENCIES 
  None.

RETURN VALUE  
  TRUE - Successfully filled IPv4 Address allocation in PCO
  FALSE- Error
  
SIDE EFFECTS 
  None.     
===========================================================================*/
void ds_pdn_mgr_fill_address_alloc_param
(
  ds_pdn_protocol_cfg_options_s  *protocol_config_options_p,
  ds_profile_type_s               *profile_p
)
{
  ds_pdn_protocol_id_s  *temp_proto_info = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_pdn_mgr_fill_pco_container_info(protocol_config_options_p, 
  	                                  IP_ADDR_ALLOC_NAS_CONTAINER_ID,
  	                                  &temp_proto_info);  
  return;
} /* ds_pdn_mgr_fill_address_alloc_param() */

/*===========================================================================
FUNCTION DS_PDN_MGR_FILL_BCM_REQUEST

DESCRIPTION
  This function fills the BCM container in PCO
  
PARAMETERS 
    protocol_cfg_options_T    *protocol_config_options_p 
                               
DEPENDENCIES 
  None.

RETURN VALUE  
  none
  
SIDE EFFECTS 
  None     
===========================================================================*/
void ds_pdn_mgr_fill_bcm_indication
(
  ds_pdn_protocol_cfg_options_s  *protocol_config_options_p
)
{
  ds_pdn_protocol_id_s  *temp_proto_info = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Fill BCM parameter for rel version higher than rel6 and if umts_nw_init_qos 
    NV is enabeled
  --------------------------------------------------------------------------*/
  if( (ds_pdn_nv_manager_get_umts_nw_init_qos_support_nv() != DS_PDN_NV_ENABLED)|| 
      (ds_pdn_nv_manager_get_3gpp_rel_version() <= (uint8)DS_3GPP_REL6))
  {
    return;
  }

  ds_pdn_mgr_fill_pco_container_info(protocol_config_options_p, 
  	                                  BEARER_CONTROL_MODE_CONTAINER_ID,
  	                                  &temp_proto_info);    
  return;
} /* ds_pdn_mgr_fill_bcm_indication() */

/*===========================================================================
FUNCTION DS_PDN_MGR_FILL_IPV4V6_LINK_MTU_CONTAINER

DESCRIPTION
  This function fills the IPV4V6 mtu PCO container
  
PARAMETERS 
    protocol_cfg_options_T    *protocol_config_options_p 
                               
DEPENDENCIES 
  None.

RETURN VALUE  
  None.
  
SIDE EFFECTS 
  None.    
===========================================================================*/
void ds_pdn_mgr_fill_ipv4v6_link_mtu_container
(
  ds_pdn_protocol_cfg_options_s  *protocol_config_options_p
)
{
  
  uint8                   rel_version;
  sys_sys_mode_e_type     mode = SYS_SYS_MODE_NONE;
  ds_pdn_protocol_id_s  *temp_proto_info = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Check if we have to fill this protocol info pkt in the pdp 
    context request based on NV item 3gpp_rel_version for non-LTE and 
    LTE_REL version (lte_3gpp_release_ver) for LTE sys mode.
  -------------------------------------------------------------------------*/
  rel_version = ds_pdn_nv_manager_get_3gpp_rel_version();
  mode = ds_pdn_hdlr_get_current_network_mode();

  if (( ( mode != SYS_SYS_MODE_LTE )&&( rel_version < (uint8)DS_3GPP_REL10 ))
#ifdef FEATURE_DATA_LTE
        || (( mode == SYS_SYS_MODE_LTE )&& 
           (ds_pdn_nv_manager_get_nv_lte_rel_version() < (uint8)LTE_3GPP_REL10))||
           (TRUE == ds_pdn_nv_manager_get_disable_mtu_request())
#endif /* FEATURE_DATA_LTE */
    )
  {
    return;
  }

  ds_pdn_mgr_fill_pco_container_info(protocol_config_options_p, 
  	                                  IPV4_LINK_MTU_CONTAINER_ID,
  	                                  &temp_proto_info);    
  return;
}/* ds_pdn_mgr_fill_ipv4v6_link_mtu_container */

/*===========================================================================
FUNCTION DS_PDN_MGR_FILL_IPV4V6_LOCAL_ADDRESS_SUPPORT_CONTAINER

DESCRIPTION
  This function fills the IPV4V6 mtu PCO container
  
PARAMETERS 
    protocol_cfg_options_T    *protocol_config_options_p 
                               
DEPENDENCIES 
  None.

RETURN VALUE  
  None.
  
SIDE EFFECTS 
  None.    
===========================================================================*/
void ds_pdn_mgr_fill_ipv4v6_local_address_support_container
(
  ds_pdn_protocol_cfg_options_s  *protocol_config_options_p
)
{
  ds_pdn_protocol_id_s  *temp_proto_info = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  

  ds_pdn_mgr_fill_pco_container_info(protocol_config_options_p, 
  	                                  LOCAL_ADDRESS_SUPPORT_IN_TFT_CONTAINER_ID,
  	                                  &temp_proto_info);    
  return;
}/* ds_pdn_mgr_fill_ipv4v6_local_address_support_container */

/*===========================================================================
FUNCTION ds_pdn_mgr_store_encoded_apn

DESCRIPTION
  This function encodes the apn name contained in apn_src
  and stores it in apn_dest.


PARAMETERS
  apn_dest:   Destination to store the encoded apn.
  apn_src:    Uncoded APN name.

DEPENDENCIES
  None.

RETURN VALUE
  Length of the encoded APN

SIDE EFFECTS
  None.
===========================================================================*/
uint8 ds_pdn_mgr_store_encoded_apn
(
  byte *apn_dest,
  byte *apn_src
)
{
  uint8 apn_encoded_length,label_length;
  byte  *label_ptr,*label_len_ptr,*pdp_addr_str_ptr;
  int   pdp_addr_str_len,apn_len_parsed;

  /*-------------------------------------------------------------------------
    Get the length of the apn string.
  -------------------------------------------------------------------------*/
  pdp_addr_str_len = strlen((char *)apn_src);

  /*-------------------------------------------------------------------------
    If the apn is a null apn, set the first byte of the destination apn to 0
  -------------------------------------------------------------------------*/
  if(pdp_addr_str_len == 0)
  {
    apn_dest[0] = 0;
    return 1;
  }

  if(( pdp_addr_str_len + 1 ) > MAX_APN_ADR_LEN  )
  {
    return 1;
  }

  /*-------------------------------------------------------------------------
    We have a valid APN.
    Check if we have room to encode the APN first. Since APN is specified as
    label1.label2.label3 (e.g www.ya-hoo.com ), and encoded as
    <len1><label1><len2><label2><len3L<label3> (3www6ya-hoo3com)
    the total length required to encode will be one more than the
    original length of the APN string
  -------------------------------------------------------------------------*/
#define LABEL_DELIMITOR 0x2e

  /*-------------------------------------------------------------------------
    Encode APN as per encoding rules defined in TS23.003 section 9.1
    The APN consists of one or more labels. Each label is coded as one octet
    length field followed by that number of octets coded as 8 bit ASCII chars

    label_len_ptr is used to point to the byte where the length of the
    current label must be stored. It is initialized to the start of the apn
    storage area.It is set to be one byte before the label starts.
    label_ptr is used to encode each label. It is initialized to the start of
    the apn storage area + 1.
    apn_encoded_length is used to track the total number of bytes encoded
    It initilized to '1' to account for the label length of the first label
  -------------------------------------------------------------------------*/
  label_len_ptr = &apn_dest[0];
  label_ptr = &apn_dest[1];
  label_length  = 0;
  apn_encoded_length  = 1;

  /*-------------------------------------------------------------------------
    Get the total length of the APN string and initiliaze the bytes parsed to
    to zero.
    pdp_addr_str_ptr tracks the current byte which is being parsed in the
    APN. Initilize it to the start of the APN string
  -------------------------------------------------------------------------*/
  apn_len_parsed = 0;
  pdp_addr_str_ptr = (byte*)&apn_src[0];

  /*-------------------------------------------------------------------------
    Now do the encoding while parsing the APN string. If we detect an error,
    we abort encoding and return as though NO apn is included.
  -------------------------------------------------------------------------*/
  for(apn_len_parsed = 0;
      apn_len_parsed < pdp_addr_str_len;
      apn_len_parsed++,pdp_addr_str_ptr++)
  {
    if( *pdp_addr_str_ptr == LABEL_DELIMITOR )
    {
      /*---------------------------------------------------------------------
        At the end of the current label. Encode the label length and reset
        the fields for the next label
      ---------------------------------------------------------------------*/
      DS_MSG1(MSG_LEGACY_HIGH, "Encoding APN: label len:%d",label_length);
      *label_len_ptr = label_length;
      /*---------------------------------------------------------------------
        The length of the next label starts at the end of the current label.
        and label_ptr starts at one byte off the label_len_ptr
        Increment apn_encoded_length by 1 to account for the storage reqd for
        the legnth of the next label
      ---------------------------------------------------------------------*/
      label_len_ptr = label_ptr;
      label_ptr++;
      apn_encoded_length++;
      label_length  = 0;
    }
    else
    {
      /*---------------------------------------------------------------------
        Encode the character and keep track of the length encoded so far
        increment the current label's ptr
        increment the length encoded so far
        increment the total space used in encoding all labels till now
      ---------------------------------------------------------------------*/
      *label_ptr = *pdp_addr_str_ptr ;
      label_ptr++;
      label_length++;
      apn_encoded_length++;
    }
  }
      
  if(apn_encoded_length > 1 )
  {
    /*-----------------------------------------------------------------------
      Tie of the length of the last label encoded if we had any
    -----------------------------------------------------------------------*/
    *label_len_ptr = label_length;
    DS_MSG1(MSG_LEGACY_HIGH, "Encoding APN: label len:%d",label_length);
  }
  
  return apn_encoded_length;
} /* ds_pdn_mgr_store_encoded_apn */

/*===========================================================================
FUNCTION ds_pdn_mgr_tear_down_instance

DESCRIPTION
  This function tears down the instance.  

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS  
===========================================================================*/
void ds_pdn_mgr_tear_down_instance
( 
  ds_pdn_mgr_s                 *pdn_mgr_p,
  ds_pdn_mgr_instance_type_e    inst_type,
  ds_net_down_reason_type       down_reason
)
{   
  ds_bearer_mgr_s          *bearer_mgr_p = NULL;
  ds_pdn_mgr_inst_type_s   *inst_p = NULL;
  ds_bearer_flow_manager_s *bflow_p = NULL;
  ds_bearer_flow_manager_list_type *bflow_list_p = NULL;
/*-------------------------------------------------------------------------*/

  inst_p = pdn_mgr_p->inst_ptr[inst_type];

  /* Get the default bearer */
  bearer_mgr_p = pdn_mgr_p->bearer_ptr_tbl[DS_3GPP_DEFAULT_BEARER_INDEX];

  /*---------------------------------------------------------------------------------------
  If there are any pending flow bound to the default bearer and TFT's are installed on 
  the default bearer we clear all the flow if instance Down is triggered
  -----------------------------------------------------------------------------------------*/
  bflow_p = ds_bearer_flow_manager_find_entry(bearer_mgr_p);
  if(NULL != bflow_p)
  {   
    bflow_list_p = list_peek_front(&(bflow_p->flow_context));
    while(bflow_list_p && bflow_list_p->flow_context_p)
    { 
      ds_bearer_flow_manager_list_type *next_flow_cntxt = 
      	                                list_peek_next(&(bflow_p->flow_context),
                                                      &(bflow_list_p->bflow_link));
      if(bflow_list_p->flow_context_p->inst_ptr == inst_p)
      {
        list_pop_item(&(bflow_p->flow_context), 
                      &(bflow_list_p->bflow_link));        
        DS_MSG3(MSG_LEGACY_HIGH, "Flow %x removed from bearer %d bearer flow %d", 
                bflow_list_p->flow_context_p, bearer_mgr_p->index, bflow_p);

        ds_flow_release_flow_context(bflow_list_p->flow_context_p);
        DS_SYSTEM_HEAP_MEM_FREE(bflow_list_p);
      }
      
      bflow_list_p = next_flow_cntxt;
    }
  }

  /*-------------------------------------------------------------------------
    Unbind the default flow
  -------------------------------------------------------------------------*/
  if(inst_p != NULL &&
     inst_p->flow != NULL)
  {
    ds_flow_go_null_ind(inst_p->flow->default_flow_ptr);
  }
 
  if(DS_PDN_INSTANCE_IPV4 == inst_type)
  {
    DS_MSG3(MSG_LEGACY_HIGH,
            "MH posting iface down ind on PDN %d, "
            "instance %d, reason: %x",
            pdn_mgr_p->index, inst_type, down_reason);
    ds_pdn_mgr_cleanup_inst(pdn_mgr_p, inst_type);
  }
  else if(DS_PDN_INSTANCE_IPV6 == inst_type)
  {
    if(NULL != inst_p)
    {
      /*--------------------------------------------------------------------    
        Send a Link down event to DS PDN V6 SM. Link Down event transitions 
        SM to closed state. In closed state SM sends instance down indication.
      --------------------------------------------------------------------*/    
      ds_pdn_v6_process_link_down_ev(inst_p->ds_pdn_v6_sm_cb,
                                     down_reason);
    }
  }
  else if(DS_PDN_INSTANCE_NON_IP == inst_type)
  {
    if( NULL != inst_p )
    {
      DS_MSG3(MSG_LEGACY_HIGH,
              "MH posting iface down ind on PDN %d, "
              "instance %d, reason: 0x%x",
              pdn_mgr_p->index, inst_type, down_reason);
      ds_pdn_mgr_cleanup_inst(pdn_mgr_p, inst_type);
    }
  }
  else
  {
    DS_ERROR_LOG_0("Iface has invalid IP type");
  }
  
  return;
}

/*===========================================================================
FUNCTION ds_pdn_mgr_bearer_down_ind

DESCRIPTION
  This function handles bearer teardown from PDN perspective.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS  
===========================================================================*/
void ds_pdn_mgr_bearer_down_ind
( 
  ds_pdn_mgr_s                  *pdn_mgr_p,
  ds_bearer_mgr_s               *bearer_mgr_p,
  ds_net_down_reason_type        net_down_reason
)
{   
  ds_pdn_mgr_inst_type_s   *v4_inst_p = NULL ;
  ds_pdn_mgr_inst_type_s   *v6_inst_p = NULL ;
  ds_pdn_mgr_inst_type_s   *non_ip_inst_p = NULL ;
  ds_profile_pdp_type_e     pdn_pdp_type;
  ds_pdn_network_params_info_type *info_p = NULL;
  ds_profile_auth_enum_type requested_auth_type;
  uint8                     pdn_index = 0;
  /*-------------------------------------------------------------------------*/
 
  if( !ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p) ||
      !ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p) )
  {
    return;
  }

  pdn_index = pdn_mgr_p->index;
  DS_MSG3(MSG_LEGACY_HIGH, 
          "ds_pdn_mgr_bearer_down_ind, PDN %d Bearer %d in state %d",
          pdn_index, bearer_mgr_p->index, bearer_mgr_p->state); 

  switch(bearer_mgr_p->state)
  {
    case DS_BEARER_MGR_STATE_PEND_REDIAL:
      info_p = &(pdn_mgr_p->info);
      if(bearer_mgr_p->is_default) 
      {
        if( (info_p->valid_fields & NET_CFG_PRM_AUTH_PREF_MASK)||
            (info_p->valid_fields & NET_CFG_PRM_AUTH_USERNAME_MASK)||
            (info_p->valid_fields & NET_CFG_PRM_AUTH_PASSWORD_MASK) )
        {
          requested_auth_type = (ds_profile_auth_enum_type)
        	                                  info_p->auth_info.auth_type;
        }
        else
        {
          requested_auth_type = pdn_mgr_p->pdp_profile->auth.auth_type;
        }    
        DS_MSG1(MSG_LEGACY_HIGH, "Read auth_type %d from profile/info", 
        	       requested_auth_type);
      
        /* Indicate to the auth manager that the bearer activation failed*/
        ds_pdn_auth_mgr_call_failure
          (pdn_mgr_p->pdp_profile->context.apn, requested_auth_type, 
           net_down_reason, bearer_mgr_p->call_was_connected);
      }
      
     /*Move the PDN state to PENDING REDIAL only if it was in COMING UP state*/     
      if(pdn_mgr_p->state == DS_PDN_MGR_STATE_COMING_UP)
      {
        pdn_mgr_p->state = DS_PDN_MGR_STATE_PEND_REDIAL;
      }
  	
      ds_pdn_mgr_setup_redial(pdn_mgr_p, bearer_mgr_p, net_down_reason);
      return;
         
    case DS_BEARER_MGR_STATE_DOWN:  
      /*--------------------------------------------------------------------------- 
        Get the Iface. This can be obtained by looking at the PDP type in the
        profile if the call was not connected.
        If the call had been connected, then the APN table is the place to get this
        info from because there is a possibility that the n/w did not give what was
        asked. The APN table is updated to reflect this info correctly only if the
        call was connected
      ---------------------------------------------------------------------------*/
      if(bearer_mgr_p->call_was_connected == TRUE)
      {
        pdn_pdp_type = ds_apn_table_get_pdn_pdp_type(pdn_mgr_p);
      }
      else
      {
        pdn_pdp_type = pdn_mgr_p->pdp_profile->context.pdp_type;
      }
    
      DS_MSG1(MSG_LEGACY_HIGH, "pdn_pdp_type is %d", pdn_pdp_type);
  
      if(pdn_pdp_type == DS_PROFILE_PDP_NON_IP)
      {
        non_ip_inst_p = pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_NON_IP];
      }
      else
      {
        v4_inst_p = pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV4];        
        v6_inst_p = pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV6];
      }       
     
      switch (pdn_mgr_p->net_down_reason)
      {
        case DS_NET_DOWN_REASON_DATA_SETTINGS_DISABLED:
        case DS_NET_DOWN_REASON_DATA_ROAMING_SETTINGS_DISABLED:
        case DS_NET_DOWN_REASON_APN_DISALLOWED_ON_ROAMING:
        case DS_NET_DOWN_REASON_INTERNAL_APN_DISABLED:
        {
          //ds_3gpp_pdn_cntxt_detach_and_block_plmn_if_attach_profile_unavailable(subs_id);
          break;
        }
     
        default:
          break;
      }
    
      /*----------------------------------------------------------------------------
        Decrement the number of active bearer contexts if the number of bearer cntx
        is greater than 0
      ----------------------------------------------------------------------------*/
      DS_ASSERT(pdn_mgr_p->num_active_bearer_context > 0);
      pdn_mgr_p->num_active_bearer_context--;
      
      DS_MSG1(MSG_LEGACY_HIGH, "Number of active bearer_cntx %d",
              pdn_mgr_p->num_active_bearer_context);
  
      ds_pdn_apn_rate_ctrl_process_call_end(bearer_mgr_p->apn_index);
      bearer_mgr_p->apn_index = -1;
  
      if(pdn_mgr_p->num_active_bearer_context == 0)
      {
        if(pdn_mgr_p->nw_mode == SYS_SYS_MODE_LTE)
        {
          ds_pdn_cp_data_process_call_end(bearer_mgr_p);
        }
    
        pdn_mgr_p->ue_initiated_tear_down = FALSE;        
        pdn_mgr_p->net_down_reason = net_down_reason; 
        pdn_mgr_p->state = DS_PDN_MGR_STATE_GOING_DOWN;
  
        if (v4_inst_p != NULL)
        {
          ds_pdn_mgr_tear_down_instance(pdn_mgr_p, v4_inst_p->type,
                                        net_down_reason);
        }
  
        if (v6_inst_p != NULL)
        {
          ds_pdn_mgr_tear_down_instance(pdn_mgr_p, v6_inst_p->type,
                                        net_down_reason);
        }
    
        if (non_ip_inst_p != NULL)
        {
          ds_pdn_mgr_tear_down_instance(pdn_mgr_p, non_ip_inst_p->type,
                                        net_down_reason);
        }
    
        /*----------------------------------------------------------------------
         Validate PDN to avoid multiple clean up done as part of instance tear down.
         NOTE : We need to reinitialise local value of PDN ptr, as the PDN can be freed in
         above functions but the local value will still be intact, which may lead to crash
         later when we try to dereference the valid cookie parameter in PDN validation function.
        -----------------------------------------------------------------------*/    
        pdn_mgr_p = ds_pdn_mgr_get_pdn_ptr_from_index(pdn_index);
        if(ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p))    
        {
        /*--------------------------------------------------------------------------
          Release Bearer Flow Manager associated with the bearer.
        --------------------------------------------------------------------------*/
          ds_pdn_flow_manager_remove_bearer_from_pdn_flow(
                                             pdn_mgr_p, bearer_mgr_p);        
          ds_bearer_flow_manager_release_flow(bearer_mgr_p);
  
        /*-----------------------------------------------------------------------
          Clear the PDN context timer. This needs to be done only if the timer was 
          started.
        -----------------------------------------------------------------------*/
          if(pdn_mgr_p->pdp_profile->inactivity_timer_val > 0)
          {    
            ps_timer_cancel(pdn_mgr_p->pdn_inactivity_timer);
            ps_timer_cancel(pdn_mgr_p->polling_timer);
          } /* pdn_inactivity_timer > 0 */

          if(pdn_mgr_p->lte_params)
          {
            ps_timer_cancel(pdn_mgr_p->lte_params->apn_param_chg_timer);
          }
        
          ds_pdn_mgr_cleanup_pdn(pdn_mgr_p, DS_PDN_INSTANCE_MAX); 
     
          ds_eps_bearer_mgr_deactivate_last_bearer_if_needed();
        }
      }
      else
      {                
        /*--------------------------------------------------------------------------
          Release Bearer Flow Manager associated with the bearer.
        --------------------------------------------------------------------------*/
        ds_pdn_flow_manager_remove_bearer_from_pdn_flow(
                                           pdn_mgr_p, bearer_mgr_p);        
        ds_bearer_flow_manager_release_flow(bearer_mgr_p); 
        ds_pdn_mgr_remove_bearer_context(pdn_mgr_p,bearer_mgr_p);
        ds_bearer_mgr_free_dyn_mem(bearer_mgr_p);
      }          
      break;
    
    default:
      DS_MSG1(MSG_LEGACY_ERROR, "Unhandled bearer state %d",
              bearer_mgr_p->state);
      break;
  }
  
  return;
}/* ds_pdn_mgr_bearer_down_ind */

/*===========================================================================
FUNCTION DS_PDN_MGR_RESEND_PDN_CONN_REQ

DESCRIPTION
  This function is used to resend the PDN connectivity request. This function
  is applicable only to the LTE mode of operation.

PARAMETERS
  pdn_mgr_p - Pointer to the PDN context on which the request is to be sent

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_pdn_mgr_resend_pdn_conn_req
(
  ds_pdn_mgr_s*              pdn_mgr_p,
  ds_pdn_mgr_instance_type_e inst_type
)
{
  ds_pdn_mgr_inst_type_s   *inst_p;
  ds_bearer_mgr_s          *bearer_mgr_p;
  ds_bearer_flow_manager_s *bflow_manager_p= NULL;
  ds_pdn_flow_manager_s    *flow_manager_p = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(pdn_mgr_p->state != DS_PDN_MGR_STATE_COMING_UP)
  {
    pdn_mgr_p->state = DS_PDN_MGR_STATE_COMING_UP;
  }

  inst_p = pdn_mgr_p->inst_ptr[inst_type];
  	
  bearer_mgr_p = pdn_mgr_p->bearer_ptr_tbl[DS_3GPP_DEFAULT_BEARER_INDEX];
  /*------------------------------------------------------------------------
     If the bearer is already allocated, we only need to bring up the bearer
  ------------------------------------------------------------------------*/
  if(!ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p))
  {
    /*-----------------------------------------------------------------------
      Allocate a bearer context from the free pool of bearers. If no bearer 
      is available, we should fail the call. 
      -----------------------------------------------------------------------*/
    bearer_mgr_p = ds_bearer_mgr_allocate_new_bearer();
    if(!ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p))
    {
      pdn_mgr_p->net_down_reason = DS_NET_DOWN_REASON_INSUFFICIENT_RESOURCES;
      ds_pdn_mgr_cleanup_pdn(pdn_mgr_p, inst_type);
      return;
    }
  
    if(ds_pdn_mgr_store_bearer_context(pdn_mgr_p, bearer_mgr_p) != TRUE)
    {
      pdn_mgr_p->net_down_reason = DS_NET_DOWN_REASON_INSUFFICIENT_RESOURCES;
      ds_pdn_mgr_cleanup_pdn(pdn_mgr_p, inst_type);
      return;
    } 
            
    bearer_mgr_p->is_default = TRUE;
  
    flow_manager_p = ds_pdn_flow_mgr_get_entry(pdn_mgr_p->index);
    if (flow_manager_p == NULL)
    {
      if(ds_pdn_flow_manager_allocate_mem(pdn_mgr_p) == NULL)
      {
        DS_ERROR_LOG_0("Unable to allocate Flow Manager");
        ds_pdn_mgr_cleanup_pdn(pdn_mgr_p, inst_type);
        return;
      }
    }

    bflow_manager_p = ds_bearer_flow_manager_allocate_mem(bearer_mgr_p);
    if(NULL == bflow_manager_p)
    {
      DS_ERROR_LOG_0("Unable to allocate Bearer flow Manager");
      ds_pdn_mgr_cleanup_pdn(pdn_mgr_p, inst_type);
      return;
    }
    /*-------------------------------------------------------------------------
      Add Bearer Flow manager to Flow manager.
    -------------------------------------------------------------------------*/
    if (ds_pdn_flow_mgr_add_bearer_to_pdn_flow(pdn_mgr_p,bearer_mgr_p) 
    	                                                          == FALSE )
    {
      DS_ERROR_LOG_0("Error in adding bearer flow manager to flow manager");
    } 

    ds_flow_mgr_configure_default_flow(pdn_mgr_p,
                                       bearer_mgr_p,
                                       inst_p);    
  }
  else
  {
    DS_MSG0(MSG_LEGACY_HIGH, "ds_pdn_cntxt_resend_pdn_conn_req: "
            "Bearer already allocated. Invoking bearer bringup");
  }

  (void)ds_bearer_mgr_bring_up_bearer(bearer_mgr_p);         
  return;
} /* ds_pdn_mgr_resend_pdn_conn_req */

/*===========================================================================
FUNCTION DS_PDN_MGR_SETUP_PDN_WITH_DS_INSTANCE

DESCRIPTION
  This function associates a valid DS instance to a newly allocated pdn context
  Updated the pdp type in the profile and the APN table as required.
 
PARAMETERS 
  new_pdn_cnt_p    - Pointer to the newly allocated PDN context
  old_pdn_cntx_p    - Pointer to the old PDN context
  inst_p                 - Pointer to a previously valid DS instance that is being
                              associated with the newly allocated PDN

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_pdn_mgr_setup_pdn_with_ds_instance
(
  ds_pdn_mgr_s           *new_pdn_cntx_p, 
  ds_pdn_mgr_s           *old_pdn_cntx_p, 
  ds_pdn_mgr_inst_type_s *inst_p
)
{
  ds_apn_ip_support_type_e ip_support_to_request = DS_IP_TYPE_INVALID;
  ds_apn_ip_support_type_e ip_support_granted = DS_IP_TYPE_INVALID;

  memscpy(new_pdn_cntx_p->pdp_profile,
          sizeof(ds_profile_type_s),
          old_pdn_cntx_p->pdp_profile,
          sizeof(ds_profile_type_s));

  if(inst_p->type == DS_PDN_INSTANCE_IPV4)
  {
     new_pdn_cntx_p->pdp_profile->context.pdp_type = DS_PROFILE_PDP_IPV4;
     new_pdn_cntx_p->inst_ptr[DS_PDN_INSTANCE_IPV4] = inst_p;     
     ip_support_to_request = DS_IPV4_SINGLE_BEARER;
     ip_support_granted = DS_IPV6_SINGLE_BEARER;
  }
  else if(inst_p->type == DS_PDN_INSTANCE_IPV6)
  {
    new_pdn_cntx_p->pdp_profile->context.pdp_type = DS_PROFILE_PDP_IPV6;
    new_pdn_cntx_p->inst_ptr[DS_PDN_INSTANCE_IPV6] = inst_p;    
    ip_support_to_request = DS_IPV6_SINGLE_BEARER;
    ip_support_granted = DS_IPV4_SINGLE_BEARER;

    inst_p->ip6_sm->pdn_ptr = new_pdn_cntx_p;
    inst_p->ds_pdn_v6_sm_cb->pdn_mgr_p = new_pdn_cntx_p;
  }

  /*----------------------------------------------------------------------
  Unbind the old bearer from the flow of this instance
  ----------------------------------------------------------------------*/
  ds_flow_go_null_ind(inst_p->flow->default_flow_ptr);
   
  ds_bearer_flow_manager_remove_flow_context_from_flow_manager(
             old_pdn_cntx_p->bearer_ptr_tbl[DS_3GPP_DEFAULT_BEARER_INDEX],
             inst_p->flow->default_flow_ptr);
   
  /*-----------------------------------------------------------------------
   Update the new PDN context with old PDNs Profile Number
  -------------------------------------------------------------------------*/
  new_pdn_cntx_p->pdp_profile_num = old_pdn_cntx_p->pdp_profile_num;
  new_pdn_cntx_p->apn_type = old_pdn_cntx_p->apn_type;

 /*----------------------------------------------------------------------
  Update the APN table with the new PDN context pointer and the IP type
  being requested for this connection.
  This has to be done in two steps. First we have to change the IP support
  in APN table to reflect the fact that we are going to establish two
  Single bearer PDN connections.
  Then update the APN table with the PDN context pointer. 
  ----------------------------------------------------------------------*/
  (void)ds_apn_table_update_apn_table(new_pdn_cntx_p->pdp_profile->context.apn,
                                      ip_support_granted, old_pdn_cntx_p);
  
  (void)ds_apn_table_add_pdn(new_pdn_cntx_p->pdp_profile->context.apn,
                             ip_support_to_request, new_pdn_cntx_p);

  
  /*------------------------------------------------------------------------- 
   Save info_ptr in pdn_context. It may be required later for redialing the
   call.
  -------------------------------------------------------------------------*/
  memscpy(&(new_pdn_cntx_p->info),
	         sizeof(ds_pdn_network_params_info_type),
          &(old_pdn_cntx_p->info),
          sizeof(ds_pdn_network_params_info_type));

  /* Copy the FWK index as well */
  new_pdn_cntx_p->fw_index = old_pdn_cntx_p->fw_index;

  ds_fwk_alloc_attach_inst(new_pdn_cntx_p, 
                       ds_pdn_mgr_convert_pdn_inst_to_fw_ip_type(inst_p->type),
                           FALSE);
  return;
}  /* ds_3gpp_pdn_cntxt_setup_pdn_with_ds_iface */


/*===========================================================================
FUNCTION DS_PDN_MGR_DISSOCIATE_INSTANCE

DESCRIPTION
  This function dissociates the PDN context from the instance that was not
  granted in Activate Bearer Indication

PARAMETERS   
  pdn_mgr_p             - Pointer to the PDN context
  ip_support_granted - The IP support that was returned by the Network
    
DEPENDENCIES
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
void ds_pdn_mgr_dissociate_instance
(
  ds_pdn_mgr_s             *pdn_mgr_p, 
  ds_apn_ip_support_type_e  ip_support_granted
)
{
  ds_pdn_mgr_inst_type_s    *inst_p = NULL;
  ds_pdn_mgr_s              *new_pdn_mgr_p = NULL;
  ds_net_down_reason_type    down_reason = DS_NET_DOWN_REASON_NOT_SPECIFIED;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( ip_support_granted == DS_IPV4V6 ||
      !ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p) ) 
  {
    return;
  }

  DS_MSG1(MSG_LEGACY_HIGH,
  	       "Entered dissociate instance. IP support granted was: %d ",
          ip_support_granted);

  /*--------------------------------------------------------------------------
    Get the DS Iface to dissociate based on the IP support granted.
    Example, if V4 was granted, the V6 DS Iface has to be dissociated.
  --------------------------------------------------------------------------*/
  switch(ip_support_granted)
  {
    case DS_IPV4_ONLY:
      down_reason = DS_NET_DOWN_REASON_IP_V4_ONLY_ALLOWED;
      inst_p = pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV6];
      break;

    case DS_IPV4_SINGLE_BEARER:
      down_reason = DS_NET_DOWN_REASON_SINGLE_ADDR_BEARER_ONLY;
      inst_p = pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV6];
      break;

    case DS_IPV6_ONLY: 
      down_reason = DS_NET_DOWN_REASON_IP_V6_ONLY_ALLOWED;
      inst_p = pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV4];
      break;

    case DS_IPV6_SINGLE_BEARER:
      down_reason = DS_NET_DOWN_REASON_SINGLE_ADDR_BEARER_ONLY;
      inst_p = pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV4];
      break;
       
    default:
      DS_ASSERT(0);
      break;
  }

  if(inst_p == NULL)
  {
    DS_MSG0(MSG_LEGACY_ERROR, "inst_p is NULL");
    return;
  }

  /*--------------------------------------------------------------------------
    Tear down the instance for which IP support was not returned
    This is to be done only when the instance is out of use or if the IP support
    returned is IPV4_ONLY or IPV6_ONLY.
    --------------------------------------------------------------------------*/
  if( inst_p->in_use == FALSE ||
      ip_support_granted == DS_IPV4_ONLY || 
      ip_support_granted == DS_IPV6_ONLY )
  {
    ds_pdn_mgr_tear_down_instance(pdn_mgr_p, inst_p->type, down_reason);
    return;
  } /* Iface to be dissociated is not in use */

  /*--------------------------------------------------------------------------
    If the instance is IN USE, then we have to resend PDN connection request.
    This however will not hold good if the instance is default. Also we should 
    not resend the PDN connection request if the IP support that was returned
    is DS_IPV4_ONLY or DS_IPV6_ONLY.

    The steps are as follows:
      Allocate a new PDN context
      Cache the Profile info into new PDN context. Ensure that we set the PDP
      type to the one we want to get. Never set it as IPV4V6
      Do the binding between the PDN context and DS instance
      Then enqueue DS3GPP_RESEND_PDN_CONN_REQ_CMD which will allocate bearer 
      and send out a new request
  --------------------------------------------------------------------------*/
  new_pdn_mgr_p = ds_pdn_mgr_allocate_new_pdn_context();
  if(ds_pdn_mgr_validate_pdn_ptr(new_pdn_mgr_p) == FALSE)
  {
    ds_pdn_mgr_tear_down_instance(pdn_mgr_p, inst_p->type, 
    	                             DS_NET_DOWN_REASON_INSUFFICIENT_RESOURCES);
  }
  else
  {
    /* Unbind the instance from old PDN */
    pdn_mgr_p->inst_ptr[inst_p->type] = NULL;
    	
    ds_pdn_mgr_setup_pdn_with_ds_instance(new_pdn_mgr_p,
    	                                     pdn_mgr_p,
    	                                     inst_p);  
    ds_pdn_mgr_resend_pdn_conn_req(new_pdn_mgr_p, inst_p->type);
  }

  return;
} /* ds_pdn_mgr_dissociate_instance */      

/*===========================================================================
FUNCTION ds_pdn_mgr_conv_data_comp_from_pdp_t_to_sys_t

DESCRIPTION
  This function converts Data Compression parameter from PDP Profile's 
  native type to sys_pdp_data_comp_e_type.

PARAMETERS
    ds_umts__pdp_data_comp_e_type - PDP Profile Header Compression type

DEPENDENCIES
  None.

RETURN VALUE
  sys_pdp_data_comp_e_type - Header Compression type.

SIDE EFFECTS
  None.
===========================================================================*/
sys_pdp_data_comp_e_type ds_pdn_mgr_conv_data_comp_from_pdp_t_to_sys_t
(
  const ds_profile_data_comp_e_type *pdp_d_comp
)
{
  sys_pdp_data_comp_e_type sys_d_comp;

  switch (*pdp_d_comp) {
  case DS_PROFILE_DATA_COMP_OFF:
      sys_d_comp = SYS_PDP_DATA_COMP_OFF;
      break;
  case DS_PROFILE_DATA_COMP_ON:
      sys_d_comp = SYS_PDP_DATA_COMP_ON;
      break;
  case DS_PROFILE_DATA_COMP_V42_BIS:
      sys_d_comp = SYS_PDP_DATA_COMP_V42_BIS;
      break;
  case DS_PROFILE_DATA_COMP_V44:
      sys_d_comp = SYS_PDP_DATA_COMP_V44;
      break;
  default:
      DS_ASSERT(0);
      sys_d_comp = SYS_PDP_DATA_COMP_MAX;
  }

  return sys_d_comp;
}

/*===========================================================================
FUNCTION ds_pdn_mgr_conv_header_comp_from_pdp_t_to_sys_t

DESCRIPTION
  This function converts Header Compression parameter from PDP Profile's 
  native type to sys_pdp_header_comp_e_type.

PARAMETERS
    ds_umts_pdp_header_comp_e_type - PDP Profile Header Compression type

DEPENDENCIES
  None.

RETURN VALUE
  sys_pdp_header_comp_e_type - Header Compression type.

SIDE EFFECTS
  None.

===========================================================================*/
sys_pdp_header_comp_e_type ds_pdn_mgr_conv_header_comp_from_pdp_t_to_sys_t
(
  const ds_profile_header_comp_e_type *pdp_h_comp
)
{
  sys_pdp_header_comp_e_type sys_h_comp;

  switch (*pdp_h_comp) {
  case DS_PROFILE_HEADER_COMP_OFF:
      sys_h_comp = SYS_PDP_HEADER_COMP_OFF;
      break;
  case DS_PROFILE_HEADER_COMP_ON:
      sys_h_comp = SYS_PDP_HEADER_COMP_ON;
      break;
  case DS_PROFILE_HEADER_COMP_RFC1144:
      sys_h_comp = SYS_PDP_HEADER_COMP_RFC1144;
      break;
  case DS_PROFILE_HEADER_COMP_RFC2507:
      sys_h_comp = SYS_PDP_HEADER_COMP_RFC2507;
      break;
  case DS_PROFILE_HEADER_COMP_RFC3095:
      sys_h_comp = SYS_PDP_HEADER_COMP_RFC3095;
      break;
  default:
      DS_ASSERT(0);
      sys_h_comp = SYS_PDP_HEADER_COMP_MAX;
  }

  return sys_h_comp;
}/* ds_pdn_mgr_conv_header_comp_from_pdp_t_to_sys_t */

/*===========================================================================
FUNCTION ds_pdn_mgr_set_mtu

DESCRIPTION
  This function sets MTU for the provided instance.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_pdn_mgr_set_mtu
( 
  ds_pdn_mgr_s               *pdn_mgr_p,
  ds_pdn_mgr_instance_type_e  inst_type,
  uint16                       mtu
)
{
  ds_fwk_s_type           *fw_inst_p = NULL;
  uint16                    old_mtu = 0;
  uint16                    new_mtu = 0;
  ds_pdn_event_info_u_type  event_info;  
  
  fw_inst_p = ds_fwk_get_inst_ptr_from_index(pdn_mgr_p->fw_index);
  if(!fw_inst_p)
  {
    DS_ERROR_FATAL("NULL FWK ptr !");
    return;
  }
  memset(&event_info, 0, sizeof(ds_pdn_event_info_u_type));
  switch(inst_type)
  {
    case DS_PDN_INSTANCE_IPV4:
       old_mtu = fw_inst_p->v4_net_info.mtu;
       new_mtu = fw_inst_p->v4_net_info.mtu = 
    	 	                   MIN( MAX(mtu, MIN_V4_MTU), MAX_SYSTEM_MTU );
      break;

    case DS_PDN_INSTANCE_IPV6:
       old_mtu = fw_inst_p->v6_net_info.mtu;
       
       DS_MSG2(MSG_LEGACY_HIGH, "Passed MTU %d , IP6 default MTU %d", 
               mtu, ps_ip_addr_get_ip6_default_mtu());

       new_mtu = fw_inst_p->v6_net_info.mtu = 
                 MIN( MAX(mtu, MIN_V6_MTU), ps_ip_addr_get_ip6_default_mtu() );
      break;

    case DS_PDN_INSTANCE_NON_IP:
       old_mtu = fw_inst_p->non_ip_net_info.mtu;
       new_mtu = fw_inst_p->non_ip_net_info.mtu = 
                                   MIN( MAX(mtu, MIN_NON_IP_MTU), MAX_SYSTEM_MTU );
      break;

    default:
    	 break;
  }

  DS_MSG2(MSG_LEGACY_HIGH, "MTU %d set for instance type %d", 
           new_mtu, inst_type);
   if(new_mtu != old_mtu)
   {
     event_info.extended_ip_config_change_mask = DS_EXT_IP_CFG_MASK_MTU;  
     ds_fwk_notify_events(fw_inst_p->ds_fwk_index,
                          ds_pdn_mgr_convert_pdn_inst_to_fw_ip_type(inst_type),
                          PDN_EXTENDED_IP_CONFIG_EX_EV,
                          event_info);
   }
   
  return;
}/* ds_pdn_mgr_set_mtu */

/*===========================================================================
FUNCTION ds_pdn_mgr_set_msisdn_info

DESCRIPTION
  This function sets MSISDN INFO for the provided instance.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_pdn_mgr_set_msisdn_info
( 
  ds_pdn_mgr_s               *pdn_mgr_p,
  ds_sys_msisdn_info_type    *msisdn_info
)
{
  ds_fwk_s_type                   *fw_inst_p = NULL;
  ds_pdn_event_info_u_type         event_info;  
  uint8                            num_ip_type = 0;
  ds_sys_event_msisdn_info_type    msisdn_event_info;
  char                            *resolved_apn = NULL; 
  
  fw_inst_p = ds_fwk_get_inst_ptr_from_index(pdn_mgr_p->fw_index);
  if(!fw_inst_p)
  {
    DS_ERROR_FATAL("NULL FWK ptr !");
    return;
  }

  (void)ds_apn_table_get_resolved_apn_name(pdn_mgr_p, &resolved_apn);

  
  memset(&event_info, 0, sizeof(ds_pdn_event_info_u_type));
  memset(&msisdn_event_info, 0, sizeof(ds_sys_event_msisdn_info_type));
  memset(&(fw_inst_p->msisdn_info), 0, sizeof(ds_sys_msisdn_info_type));
  
  
  if (memcmp(&(fw_inst_p->msisdn_info),
           msisdn_info,
           sizeof(ds_sys_msisdn_info_type)) != 0)
  {

    /*Copy the MSISDN info into event info before generating the event*/

    memscpy(&(fw_inst_p->msisdn_info),
            sizeof(ds_sys_msisdn_info_type),
            msisdn_info,
            sizeof(ds_sys_msisdn_info_type));
	
    memscpy(&(msisdn_event_info.msisdn_info),
            sizeof(ds_sys_msisdn_info_type),
            msisdn_info,
            sizeof(ds_sys_msisdn_info_type));

    strlcpy(msisdn_event_info.apn_name,
              pdn_mgr_p->pdp_profile->context.apn,
              DS_SYS_MAX_APN_LEN);
    strlcpy(msisdn_event_info.resolved_apn_name,
              resolved_apn,
              DS_SYS_MAX_APN_LEN);

    for(num_ip_type = DS_PDN_INSTANCE_IPV4;num_ip_type < DS_PDN_INSTANCE_MAX; num_ip_type++)
    {
      event_info.extended_ip_config_change_mask = DS_EXT_IP_CFG_MASK_MSISDN_PCO;
      ds_fwk_notify_events(fw_inst_p->ds_fwk_index,
                           ds_pdn_mgr_convert_pdn_inst_to_fw_ip_type(num_ip_type),
                           PDN_EXTENDED_IP_CONFIG_EX_EV,
                           event_info);
    }
    ds_sys_event_ind(
                     DS_SYS_TECH_3GPP,
                     DS_SYS_EVENT_APN_MSISDN_INFO_CHANGE_EV,
                     (void *)&msisdn_event_info);

  }
   
  return;
}/* ds_pdn_mgr_set_msisdn_info*/

/*===========================================================================
FUNCTION DS_PDN_MGR_UPDATE_V4_PARAMS 
 
DESCRIPTION
  This function updates IPV4 param in DS instance structure with Primary DNS,
  secondary DNS and Gateway Address

PARAMETERS 
  instance ptr
  decode_ps_param : IPCP option parameters
 
DEPENDENCIES 
  None. 
  
RETURN VALUE 
  None.
 
SIDE EFFECTS 
  None 
===========================================================================*/
void ds_pdn_mgr_update_v4_params
(
  ds_fwk_v4_net_info_type   *v4_info,
  ds_ipcp4_options_s         decode_param_v4
)
{
  /*-----------------------------------------------------------------------
    Set the DNS, NBNS, GW address for the iface.
  -----------------------------------------------------------------------*/
  v4_info->primary_dns.ps_s_addr =
          ps_htonl(decode_param_v4.primary_dns);
  v4_info->secondary_dns.ps_s_addr =
          ps_htonl(decode_param_v4.secondary_dns);
  v4_info->gateway.ps_s_addr =
          ps_htonl(decode_param_v4.gateway_addr);

  DS_MSG3(MSG_LEGACY_HIGH, 
  	       "Setting up Type IP V4 PS Iface: "
  	       "Gateway address for type IP call :%lx, "
  	       "PDNS address for type IP call :%lx, "
  	       "SDNS address for type IP call :%lx",
          (unsigned long)decode_param_v4.gateway_addr,  \
          (unsigned long)decode_param_v4.primary_dns,   \
          (unsigned long)decode_param_v4.secondary_dns);
  return;
}

/*===========================================================================
FUNCTION DS_PDN_MGR_UPDATE_V6_PARAMS 
 
DESCRIPTION
  This function updates IPV6 param in DS instance structure with
  Primary DNS, secondary DNS and Gateway Address

PARAMETERS 
  instance_ptr
  decode_ps_param : IPV6CP option parameters
 
DEPENDENCIES 
  None. 
  
RETURN VALUE 
  None.
 
SIDE EFFECTS 
  None 
===========================================================================*/
void ds_pdn_mgr_update_v6_params
(
  ds_fwk_v6_net_info_type   *v6_info,
  ds_ipv6cp_options_s        decode_param_v6
)
{
  /*-------------------------------------------------------------------------
    Set the DNS, NBNS, GW address for the instance.
  -------------------------------------------------------------------------*/
  v6_info->primary_dns = decode_param_v6.primary_dns;
  v6_info->secondary_dns = decode_param_v6.secondary_dns;

  /*lint -e572*/ 
  DS_MSG4(MSG_LEGACY_HIGH,
          "PDNS address for type IPV6 call :%lx%lx, "
          "SDNS address for type IPV6 call :%lx%lx",
          ps_ntohll(decode_param_v6.primary_dns.ps_s6_addr64[0]),
          ps_ntohll(decode_param_v6.primary_dns.ps_s6_addr64[1]),
          ps_ntohll(decode_param_v6.secondary_dns.ps_s6_addr64[0]),
          ps_ntohll(decode_param_v6.secondary_dns.ps_s6_addr64[1]));

  return;
}

/*===========================================================================
FUNCTION      ds_pdn_mgr_decode_ipv4v6_link_mtu

DESCRIPTION
  This function decodes the ip link mtu size from the PCO.

PARAMETERS
  protocol_pkt_ptr   -  pointer to the PCO contents field
  protocol_pkt_len   -  container length
  pdn_context_p      -  Pointer to current PDN context
  decode_mask        -  Mask pointer containing status of decode operation

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None.
===========================================================================*/
static void ds_pdn_mgr_decode_ipv4v6_link_mtu
(
  const byte           *protocol_pkt_ptr, /* Pointer to the buffer forMTU info */
  byte                  protocol_pkt_len,  /* Length of the MTU container */
  ds_pdn_mgr_s         *pdn_mgr_p,
  uint32               *decode_mask_ptr
)
{
  uint16                  mtu_len = 0;
  uint16                  rel_version;
  ds_pdn_mgr_inst_type_s *v4_inst = NULL;
  ds_pdn_mgr_inst_type_s *v6_inst = NULL;

  /*------------------------------------------------------------------------*/
  v4_inst = pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV4];
  v6_inst = pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV6];
  
  if ((NULL == v4_inst && NULL == v6_inst) ||
      (NULL == decode_mask_ptr))
  {
    return;
  }

  /*-------------------------------------------------------------------------
    If IPV4 Link MTU container length is not 2, then the link MTU content 
    should be ignored. Ref: TS24.008 sec 10.5.6.3
  --------------------------------------------------------------------------*/
  if (protocol_pkt_len != IPV4_LINK_MTU_CONTAINER_LEN)
  {
    DS_ERROR_LOG_1("IPV4V6 Link MTU container length is: =d. Ignoring MTU", 
                   protocol_pkt_len);
    return;
  }
  
  /*-------------------------------------------------------------------------
    Check if we have to decode this protocol info based on NV item
    3gpp_rel_version for non-LTE and LTE_REL version (lte_3gpp_release_ver)
    for LTE sys mode.
  -------------------------------------------------------------------------*/
  rel_version = ds_pdn_nv_manager_get_3gpp_rel_version();

  if (( ( ds_pdn_hdlr_get_current_network_mode() != SYS_SYS_MODE_LTE ) &&
        ( rel_version < (uint8)DS_3GPP_REL10 ) )
#ifdef FEATURE_DATA_LTE
        || (( ds_pdn_hdlr_get_current_network_mode() == SYS_SYS_MODE_LTE )&&
            (ds_pdn_nv_manager_get_nv_lte_rel_version() < LTE_3GPP_REL10 ))
#endif /* FEATURE_DATA_LTE */
    )
  {
    return;
  }

  /*---------------------------------------------------------------------------
    The container identifier contents field contains the binary coded 
    representation of the IPv4 link MTU size in octets. Bit 8 of the first 
    octet of the container identifier contents field contains the most 
    significant bit and bit 1 of the second octet of the container identifier 
    contents field contains the least significant bit. 
    Ref: TS24.008 sec 10.5.6.3
  ----------------------------------------------------------------------------*/
  mtu_len = (uint16)(protocol_pkt_ptr[0] << 8) | (uint16)(protocol_pkt_ptr[1]);

  if(MAX_SYSTEM_MTU < mtu_len)
  {
    *decode_mask_ptr |= DS_3GPP_LINK_MTU_INVALID;
  }
  else
  {
    if(v4_inst != NULL && 
       (pdn_mgr_p->pdp_profile->context.pdp_type == DS_PROFILE_PDP_IPV4 ||
        pdn_mgr_p->pdp_profile->context.pdp_type == DS_PROFILE_PDP_IPV4V6))
    {
      ds_pdn_mgr_set_mtu(pdn_mgr_p, v4_inst->type, mtu_len);
    }
  
    if(v6_inst != NULL && 
       (pdn_mgr_p->pdp_profile->context.pdp_type == DS_PROFILE_PDP_IPV6 ||
        pdn_mgr_p->pdp_profile->context.pdp_type == DS_PROFILE_PDP_IPV4V6))
    {
      /*---------------------------------------------------------------------------
        As per IPv6 RFC, the minimum supported MTU is 1280. PS_IFACE_SET_MTU 
        macro checks the MTU size given as input against default IPv6 MTU 1280 
        and ensures that IPv6 MTU being set is equal to or greater than 1280.
      ----------------------------------------------------------------------------*/
      ds_pdn_mgr_set_mtu(pdn_mgr_p, v6_inst->type, mtu_len);
    }
    
    *decode_mask_ptr |= DS_3GPP_LINK_MTU_DECODED_MASK;
  }
}/* ds_pdn_mgr_decode_ipv4v6_link_mtu */


/*===========================================================================
FUNCTION	  ds_pdn_mgr_decode_msisdn

DESCRIPTION
  This function decodes the msisdn from the PCO.

PARAMETERS
  protocol_pkt_ptr   -  pointer to the PCO contents field
  protocol_pkt_len   -  container length
  pdn_mgr_p 	 -  Pointer to current PDN context
  decode_mask   	 -  Mask pointer containing status of decode operation

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None.
===========================================================================*/
void ds_pdn_mgr_decode_msisdn
(
  const byte		   *protocol_pkt_ptr, /* Pointer to the buffer forMTU info */
  byte  				protocol_pkt_len,  /* Length of the MTU container */
  ds_pdn_mgr_s         *pdn_mgr_p,
  uint32			   *decode_mask_ptr
)
{
   ds_sys_msisdn_info_type         msisdn_info;
  /*------------------------------------------------------------------------*/

  
  if (NULL == decode_mask_ptr)
  {
    return;
  }

  if(protocol_pkt_ptr == NULL)
  {
	return;
  }

  if(protocol_pkt_len <=0)
  {
    DS_ERROR_LOG_1( "Invalid decoding MSISDN Length =d",
					  protocol_pkt_len);
    return;
  }

  msisdn_info.msisdn_length = protocol_pkt_len;
   
  memscpy(msisdn_info.msisdn,
		  sizeof(msisdn_info.msisdn),
		  protocol_pkt_ptr,
		  protocol_pkt_len);

  ds_pdn_mgr_set_msisdn_info(pdn_mgr_p,&msisdn_info);

  
  DS_MSG1(MSG_LEGACY_HIGH,"Setting Msisdn info %d",
					msisdn_info.msisdn[0]);

  *decode_mask_ptr |= DS_3GPP_MSISDN_INFO_DECODE_MASK;


  return;
}

/*===========================================================================
FUNCTION	  ds_pdn_mgr_get_msisdn_info

DESCRIPTION
  This function fetches the msisdn info from the fwk instance.

PARAMETERS
  arg_val_ptr[in] -  Pointer to operation specific argument
  ps_errno [out]    -    Specifies type of error in case of error.
                             DS_EFAULT if parameter is invalid
                             DS_EINVAL if operation is invalid
                             DS_NOMEMORY if memory allocation fails


DEPENDENCIES  None.

RETURN VALUE  TRUE  if fetch successful.
              FALSE if something goes wrong.


SIDE EFFECTS  None.
===========================================================================*/
boolean ds_pdn_mgr_get_msisdn_info
(
  void            *arg_val_ptr,
  int16           *ps_errno
)
{
  ds_fwk_s_type                   *fw_inst_p = NULL;
  ds_pdn_mgr_s                    *pdn_mgr_p = NULL;
  ds_sys_ioctl_msisdn_info_type   *msisdn_info = NULL;
/*-------------------------------------------------------------------------*/
   
   if ((arg_val_ptr == NULL) || (ps_errno == NULL))
   {
     if(ps_errno != NULL)
     {
       *ps_errno = DS_EFAULT;
     }
    
     DS_ERROR_LOG_0("Null Pointer passed");
     return FALSE;
   }
 
   msisdn_info = (ds_sys_ioctl_msisdn_info_type*)arg_val_ptr;
 
   if((ds_apn_table_get_v4_pdn(msisdn_info->apn_name,&pdn_mgr_p) == TRUE) ||
       (ds_apn_table_get_v6_pdn(msisdn_info->apn_name,&pdn_mgr_p) == TRUE))
   {
     if(!ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p))
     {
       *ps_errno = DS_EINVAL;
        return FALSE;
     }

     fw_inst_p = ds_fwk_get_inst_ptr_from_index(pdn_mgr_p->fw_index);
     if(!fw_inst_p)
     {
       DS_ERROR_FATAL("NULL FWK ptr !");
       *ps_errno = DS_EINVAL;
        return FALSE;
     }     

     memscpy(&(msisdn_info->msisdn_info),
             sizeof(ds_sys_msisdn_info_type),
             &(fw_inst_p->msisdn_info),
             sizeof(ds_sys_msisdn_info_type));

     return TRUE;
   }

   else
   {
     return FALSE;
   }

}



/*===========================================================================
FUNCTION ds_pdn_mgr_decode_proto_params

DESCRIPTION
  This function decodes the protocol parameters received for a PDP call.

PARAMETERS

DEPENDENCIES
  None.

RETURN VALUE
  Integer bitmask representing the decoded packets 

SIDE EFFECTS
  None.
===========================================================================*/
uint32 ds_pdn_mgr_decode_proto_params
(
  ds_pdn_mgr_s                     *pdn_mgr_p,
  ds_pdn_protocol_cfg_options_s   *protocol_config_options_p
)
{
  word                      num_pids    = 0;   /* Number of Protocol IDs */
  word                      protocol_id = 0;   /* Protocol ID */
  byte                      index       = 0;   /* Protocol options index */
  int                       num_dns_ipv6 = 0;
  uint32                    num_dns_ipv4 = 0;
  ds_pdn_mgr_inst_type_s   *v4_inst = NULL;     /* v4 Interface ptr for the call */ 
  ds_pdn_mgr_inst_type_s   *v6_inst = NULL;     /* v6 Interface ptr for the call */ 
  ds_pdn_mgr_inst_type_s   *non_ip_inst = NULL; /* non-ip Interface ptr for the call */
#ifdef FEATURE_DATA_IMS
  uint32                    num_v4_pcscf_addresses = 0; /* number of v4 pcscf address*/
  uint32                    num_v6_pcscf_addresses = 0; /* number of v6 pcscf address*/
  ps_ip_addr_type           pcscf_v4_addr_array[MAX_PCSCF_ADDRESSES] = {{0}};
  ps_ip_addr_type           pcscf_v6_addr_array[MAX_PCSCF_ADDRESSES] = {{0}};
                                        /* Array of pcscf address pointers */
#endif
  uint32                    decode_mask = 0;
  boolean                   profile_pco_id_configured = FALSE;
                                    /* Bit vector representing decoded packet types */
  ds_ipcp_options_s         decode_ipcp_param = {{0}};
  ds_fwk_sip_serv_addr_type *sip_addr = NULL;                           
  uint16                    loop =0;  
  ds_fwk_s_type            *fw_inst_p = NULL;  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  DS_MSG0(MSG_LEGACY_HIGH, "Decode protocol param");

  v4_inst = pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV4];
  v6_inst = pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV6];
  non_ip_inst = pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_NON_IP];  
  
  fw_inst_p = ds_fwk_get_inst_ptr_from_index(pdn_mgr_p->fw_index);
  if(!fw_inst_p)
  {
    DS_ERROR_FATAL("NULL FWK ptr !");
    return FALSE;
  }

  if (protocol_config_options_p->valid == FALSE)
  {
    DS_ERROR_LOG_0("decode params : Valid = FALSE");
    return decode_mask;
  }

  /*-------------------------------------------------------------------------
    Get the number of protocol IDs received. Max is NUM_PROTOCOL_ID ( for PCO)
    MAX_NUM_EPCO_PROTOCOL_ID (for EPCO ).
  -------------------------------------------------------------------------*/
  num_pids = protocol_config_options_p->num_protocol_ids;

  while (num_pids > 0)
  {
    /*----------------------------------------------------------------------
      Get the Protocol ID.
    ----------------------------------------------------------------------*/
    protocol_id = protocol_config_options_p->
              protocol_info_p[index]->protocol_id[0] << 8;
    protocol_id |= protocol_config_options_p->
              protocol_info_p[index]->protocol_id[1];

    DS_MSG2(MSG_LEGACY_HIGH, "For PDN index %d, decode Protocol ID %d",
            pdn_mgr_p->index, protocol_id);

    switch (protocol_id)
    {
      case PPP_IPCP_PROTOCOL:
        if (v4_inst)
        {
          /*-----------------------------------------------------------------
                IPCP Protocol (0x8021). Decode the packet and store the options
                in the call specific ipcp_options structure.          
                 -----------------------------------------------------------------*/
          if 
          (
            ds_pdn_decode_ipcp_pkt(
               &protocol_config_options_p->
               protocol_info_p[index]->protocol_contents[0],
               protocol_config_options_p->
               protocol_info_p[index]->protocol_len,
               &(decode_ipcp_param.ipcp4_options)
               )!= TRUE
          )
          {
            decode_mask |= DS_3GPP_PCO_DECODE_FAILED_MASK; 
            return decode_mask;
          }
          ds_pdn_mgr_update_v4_params(&fw_inst_p->v4_net_info,
          	                           decode_ipcp_param.ipcp4_options);
          decode_mask |= DS_3GPP_IPCP_PKT_DECODED_MASK; 
        }
        break;
        
     case IPV4_DNS_CONTAIN_ID:
       if (v4_inst)
       {
          /*------------------------------------------------------------------
            IPV4 DNS (0x000D). Decode the packet and store the options
            in the call specific ipv4 options          
          ------------------------------------------------------------------*/
          ds_pdn_decode_ipv4_pkt(
              &protocol_config_options_p->
              protocol_info_p[index]->protocol_contents[0],
              protocol_config_options_p->
              protocol_info_p[index]->protocol_len,
             &(decode_ipcp_param.ipcp4_options),
              &num_dns_ipv4
              );

         ds_pdn_mgr_update_v4_params(&fw_inst_p->v4_net_info,
         	                           decode_ipcp_param.ipcp4_options);
         decode_mask |= DS_3GPP_IPV4_PKT_DECODED_MASK;
       }
       break;
       
     case IPV6_DNS_CONTAIN_ID:
       if (v6_inst)
       {
        /*------------------------------------------------------------------
          IPV6 DNS (0x0003). Decode the packet and store the options
          in the call specific ipv6 options          
        ------------------------------------------------------------------*/
        ds_pdn_decode_ipv6_pkt(
            &protocol_config_options_p->
            protocol_info_p[index]->protocol_contents[0],
            protocol_config_options_p->
            protocol_info_p[index]->protocol_len,
            &(decode_ipcp_param.ipv6cp_options),
            &num_dns_ipv6
            );

        ds_pdn_mgr_update_v6_params(&fw_inst_p->v6_net_info,
        	                           decode_ipcp_param.ipv6cp_options);
        decode_mask |= DS_3GPP_IPV6_PKT_DECODED_MASK;
       }
       else
       {
         DS_ERROR_LOG_0("Ignoring IPV6 DNS pkt for V4 only call ");
       }
       break;

#ifdef FEATURE_DATA_IMS
     case SIP_PCSCF_V6_ADDRESS_PROTOCOL:
        /*-------------------------------------------------------------------
           Decode v6 pcscf address packet (0x0001), and store address to v4 
           array or v6 array respectively since NW may return both v4 and v6 
           pcscf address in the dual IP bearer case
        -------------------------------------------------------------------*/
        if(ds_pdn_decode_v6_pcscf_pkt(
             &protocol_config_options_p->
             protocol_info_p[index]->protocol_contents[0],
             protocol_config_options_p->
             protocol_info_p[index]->protocol_len,
             pcscf_v4_addr_array,
             &num_v4_pcscf_addresses,
             pcscf_v6_addr_array,
             &num_v6_pcscf_addresses
             ) == TRUE)
        {
          decode_mask |= DS_3GPP_PCSCF_V6_PKT_DECODED_MASK;
        }
        break;
        
      case SIP_PCSCF_V4_ADDRESS_PROTOCOL:
        /*-------------------------------------------------------------------
           Decode v4 pcscf address packet (0x000C), and store address to v4 
           array 
        -------------------------------------------------------------------*/
        if(ds_pdn_decode_v4_pcscf_pkt(
             &protocol_config_options_p->
             protocol_info_p[index]->protocol_contents[0],
             protocol_config_options_p->
             protocol_info_p[index]->protocol_len,
             pcscf_v4_addr_array,
             &num_v4_pcscf_addresses) == TRUE)
        {
          decode_mask |= DS_3GPP_PCSCF_V4_PKT_DECODED_MASK;
        }
        break;
#endif // FEATURE_DATA_IMS

      case BEARER_CONTROL_MODE_CONTAINER_ID:
        if(ds_pdn_flow_mgr_decode_bcm(
             &protocol_config_options_p->
             protocol_info_p[index]->protocol_contents[0],
             protocol_config_options_p->
             protocol_info_p[index]->protocol_len,
             pdn_mgr_p->index) == TRUE)
        {
          decode_mask |= DS_3GPP_BEARER_CONTROL_MODE_DECODED_MASK;
        }
        break;

      case IPV4_LINK_MTU_CONTAINER_ID:
        ds_pdn_mgr_decode_ipv4v6_link_mtu(
             &protocol_config_options_p->
             protocol_info_p[index]->protocol_contents[0],
             protocol_config_options_p->
             protocol_info_p[index]->protocol_len,
             pdn_mgr_p, &decode_mask);
        break;

      case NON_IP_LINK_MTU_CONTAINER_ID:
        ds_pdn_call_hdlr_decode_non_ip_link_mtu(
             &protocol_config_options_p->
             protocol_info_p[index]->protocol_contents[0],
             protocol_config_options_p->
             protocol_info_p[index]->protocol_len,
             pdn_mgr_p,&decode_mask);
        break;

      case APN_RATE_CONTROL_CONTAINER_ID:
        ds_pdn_apn_rate_ctrl_decode_pco_info(
             &protocol_config_options_p->
             protocol_info_p[index]->protocol_contents[0],
             protocol_config_options_p->
             protocol_info_p[index]->protocol_len,
             pdn_mgr_p, 
             &decode_mask);
        break;
			 
      case ADDITIONAL_APN_RATE_CONTROL_CONTAINER_ID:
        ds_pdn_additional_apn_rate_ctrl_decode_pco_info(
             &protocol_config_options_p->
             protocol_info_p[index]->protocol_contents[0],
             protocol_config_options_p->
             protocol_info_p[index]->protocol_len,
             pdn_mgr_p, 
             &decode_mask);
        break;

      case MSIDSN_CONTAINER_ID:
        ds_pdn_mgr_decode_msisdn(
                  &protocol_config_options_p->
                    protocol_info_p[index]->protocol_contents[0],
                  protocol_config_options_p->
                    protocol_info_p[index]->protocol_len,
                  pdn_mgr_p, 
                  &decode_mask);
      break;

      default:
        if (DS_OPERATOR_RESERVED_PCO_ID_RANGE_BEGIN <= protocol_id) 
        {
          if (pdn_mgr_p->pdp_profile->operator_reserved_pco == protocol_id)
          {
            DS_MSG0(MSG_LEGACY_HIGH, "PCO ID is configured in the profile");
            profile_pco_id_configured = TRUE;
          }

          if (ds_pdn_decode_operator_reserved_pco_pkt(
              &protocol_config_options_p->
              protocol_info_p[index]->protocol_contents[0],
              protocol_config_options_p->
              protocol_info_p[index]->protocol_len, 
              pdn_mgr_p,
              (uint16)protocol_id,
              profile_pco_id_configured) == TRUE) 
          {
            decode_mask |= DS_3GPP_OPER_DECODED_MASK;
          }
        }
        break;        
    }
    index++;
    num_pids--;
  }

#ifdef FEATURE_DATA_IMS
  /*-------------------------------------------------------------------------
    If one or more pcscf addresses were returned by network, store them in
    the ds instance structure.
  -------------------------------------------------------------------------*/
  if(v4_inst)
  {    
    DS_MSG1(MSG_LEGACY_HIGH, "recd %d v4_pcscf address", 
    	       num_v4_pcscf_addresses);
        
    if(num_v4_pcscf_addresses > 0)
    { 
      /*-----------------------------------------------------------------------
         ds3gpp_decode_v6_pcscf_pkt() decodes both v4 and v6 pcscf addresses.
         We need to update decode_mask with DS_3GPP_PCSCF_V4_PKT_DECODED here
         since it may have been missed before
      -----------------------------------------------------------------------*/
      decode_mask |= DS_3GPP_PCSCF_V4_PKT_DECODED_MASK;      

      ds_pdn_mgr_free_sip_addr_list(&fw_inst_p->v4_net_info.sip_serv_addr, 
                                    FALSE);

      for(loop=0; loop<num_v4_pcscf_addresses; loop++)
      {
         DS_SYSTEM_HEAP_MEM_ALLOC(sip_addr, sizeof(ds_fwk_sip_serv_addr_type),
                                  ds_fwk_sip_serv_addr_type*);
         DS_ASSERT(sip_addr!=NULL);
         memset(sip_addr, 0, sizeof(ds_fwk_sip_serv_addr_type));

         memscpy(&sip_addr->sip_addr,
                 sizeof(ps_ip_addr_type),
                 &pcscf_v4_addr_array[loop],
                 sizeof(ps_ip_addr_type));         
         list_push_back(&fw_inst_p->v4_net_info.sip_serv_addr, &sip_addr->link);
      }      
    }         
  }

  if(v6_inst)
  {    
    DS_MSG1(MSG_LEGACY_HIGH, "recd %d v6_pcscf address", 
    	       num_v6_pcscf_addresses);

			 if(num_v6_pcscf_addresses > 0)
			 {
      ds_pdn_mgr_free_sip_addr_list(&fw_inst_p->v6_net_info.sip_serv_addr, 
                                    FALSE);
       
			   for(loop=0; loop<num_v6_pcscf_addresses; loop++)
      {
         DS_SYSTEM_HEAP_MEM_ALLOC(sip_addr, sizeof(ds_fwk_sip_serv_addr_type),
         	                      ds_fwk_sip_serv_addr_type*);
         DS_ASSERT(sip_addr!=NULL);
         memset(sip_addr, 0, sizeof(ds_fwk_sip_serv_addr_type));

         memscpy(&sip_addr->sip_addr,
                 sizeof(ps_ip_addr_type),
                 &pcscf_v6_addr_array[loop],
                 sizeof(ps_ip_addr_type));
         list_push_back(&fw_inst_p->v6_net_info.sip_serv_addr, &sip_addr->link);
      }
			 }			       
		}
#endif

  /*-----------------------------------------------------------------------
    Free the memory for the protocol config options which was allocated 
    during parsing of PCO or EPCO 
  -------------------------------------------------------------------------*/
  for (loop = 0; loop < protocol_config_options_p->num_protocol_ids; loop++)
  {
    DS_SYSTEM_HEAP_MEM_FREE(protocol_config_options_p->protocol_info_p[loop]);
    protocol_config_options_p->protocol_info_p[loop] = NULL;
  }
  
  return decode_mask;
} /* ds_pdn_mgr_decode_proto_params() */

/*===========================================================================
FUNCTION ds_pdn_mgr_setup_pdp_ip_proto_stack

DESCRIPTION
  This function configures IP/nonIP instances with NAS provided call info.  

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE  
  Returns success or failure depending validity of configuration/mode_info.

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_pdn_mgr_setup_pdp_proto_stack
( 
  ds_pdn_mgr_s                   *pdn_mgr_p,
  ds_profile_addr_type           *ds_pdn_addr,  
  ds_pdn_protocol_cfg_options_s *protocol_config_options_p
)
{    
  ds_pdn_mgr_inst_type_s      *v4_inst = NULL;
  ds_pdn_mgr_inst_type_s      *v6_inst = NULL;
  ds_pdn_mgr_inst_type_s      *non_ip_inst = NULL;
  ds_profile_pdp_type_e        pdn_pdp_type;  
  uint32                       decode_mask = 0;  /* Bit vector representing PCO decode info*/ 
  uint16                       mtu_size;  
  ds_pdn_apm_ind_pay_load      ind_payload = {0};  
  ds_fwk_s_type               *fw_inst_p = NULL;  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_MSG0(MSG_LEGACY_HIGH, "Setting up PDP protocol stack");

  mtu_size = ds_pdn_nv_manager_get_mtu_from_nv();
  
  /* Get the DS instance based on the PDP type */
  pdn_pdp_type = ds_apn_table_get_pdn_pdp_type(pdn_mgr_p);
  
  if( pdn_pdp_type == DS_PROFILE_PDP_IPV4 ||
  	   pdn_pdp_type == DS_PROFILE_PDP_IPV4V6 )
  {
    v4_inst = pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV4];
  }

  if( pdn_pdp_type == DS_PROFILE_PDP_IPV6 ||
  	   pdn_pdp_type == DS_PROFILE_PDP_IPV4V6 )
  {
    v6_inst = pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV6];
  }

  if( pdn_pdp_type == DS_PROFILE_PDP_NON_IP )
  {
    non_ip_inst = pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_NON_IP];
  }

  fw_inst_p = ds_fwk_get_inst_ptr_from_index(pdn_mgr_p->fw_index);
  if(!fw_inst_p)
  {
    DS_ERROR_FATAL("NULL FWK ptr !");
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Get any other protocol specific options that are received and store it
    in the call specific structure.
    Note: If the network had assigned the DNS, we would have decoded it and
    overwritten the ones selected by US during call origination.    
  -------------------------------------------------------------------------*/
  if (pdn_mgr_p->is_restore_mode) 
  {
      ds_pdn_psm_set_pco_parameters(pdn_mgr_p);
     /*set the mtu mask, so that default mtu is not set */  
     decode_mask = DS_3GPP_LINK_MTU_DECODED_MASK;
  }
  else
  {
    decode_mask = ds_pdn_mgr_decode_proto_params(
       pdn_mgr_p,
       protocol_config_options_p
    );
  }

  if( decode_mask & DS_3GPP_PCO_DECODE_FAILED_MASK ||
      decode_mask & DS_3GPP_LINK_MTU_INVALID )
  {
    return FALSE;
  }
  
  if(v4_inst != NULL)
  {
    fw_inst_p->v4_net_info.ipv4_addr = ds_pdn_addr->pdp_addr.pdp_addr_ipv4;    	
    DS_MSG5(MSG_LEGACY_HIGH, 
    	       "Setting IP Address to %d.%d.%d.%d for PDN %d ",
            (unsigned char)(fw_inst_p->v4_net_info.ipv4_addr),
            (unsigned char)(fw_inst_p->v4_net_info.ipv4_addr >> 8),
            (unsigned char)(fw_inst_p->v4_net_info.ipv4_addr >> 16),
            (unsigned char)(fw_inst_p->v4_net_info.ipv4_addr >> 24),
            pdn_mgr_p->index);    

    if ( pdn_mgr_p->is_default )
    {
      ds_pdn_apm_set_ipv4_addr(&ind_payload, TRUE);
      ds_pdn_apm_set_profile_id(&ind_payload, pdn_mgr_p->pdp_profile_num);
      (void)ds_pdn_apm_ind(DS_PDN_APM_IP_ADDRESS_IND, &ind_payload);
    }
    
    if (!(decode_mask & DS_3GPP_LINK_MTU_DECODED_MASK))
    {
      ds_pdn_mgr_set_mtu(pdn_mgr_p, v4_inst->type, mtu_size);
    }    
  }

  if(v6_inst != NULL)
  {
    fw_inst_p->v6_net_info.ipv6_addrs[0]->iid = 
    	                         ds_pdn_addr->pdp_addr.pdp_addr_ipv6.in6_u.\
    	                                 u6_addr64[DS_3GPP_IPV6_IID_OFFSET];
    if (!(decode_mask & DS_3GPP_LINK_MTU_DECODED_MASK))
    {
      ds_pdn_mgr_set_mtu(pdn_mgr_p, v6_inst->type, mtu_size);
    }  
  }

  if(NULL != non_ip_inst)
  {
    if (!(decode_mask & DS_3GPP_LINK_MTU_DECODED_MASK))
    {
      ds_pdn_mgr_set_mtu(pdn_mgr_p, non_ip_inst->type, mtu_size);
    }
  }
  
  return TRUE;
} /* ds_pdn_mgr_setup_pdp_proto_stack */

ip_addr_enum_type ds_pdn_mgr_convert_pdn_inst_to_fw_ip_type
(
  ds_pdn_mgr_instance_type_e  pdn_inst_type
)
{
  switch(pdn_inst_type)
  {
    case DS_PDN_INSTANCE_IPV4:
    	 return IPV4_ADDR; 

  	 case DS_PDN_INSTANCE_IPV6:
    	 return IPV6_ADDR; 

  	 case DS_PDN_INSTANCE_NON_IP:
    	 return NON_IP_ADDR; 

    default:
    	 break;
  }

  return IP_ADDR_INVALID;
}
  
ds_pdn_mgr_instance_type_e ds_pdn_mgr_convert_fw_ip_type_to_pdn_inst
(
  ip_addr_enum_type ip_type  
)
{
  switch(ip_type)
  {  
    case IPV4_ADDR:
      return DS_PDN_INSTANCE_IPV4; 
    
    case IPV6_ADDR:
      return DS_PDN_INSTANCE_IPV6; 

    case NON_IP_ADDR:
      return DS_PDN_INSTANCE_NON_IP; 
    
    default:
      break;
  }

  return DS_PDN_INSTANCE_MAX;
}

/*===========================================================================
FUNCTION ds_pdn_mgr_instance_up_ind

DESCRIPTION
  This function posts instance up indications.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS  
  None.
===========================================================================*/
void ds_pdn_mgr_instance_up_ind
(
  ds_pdn_mgr_s            *pdn_mgr_p,
  ds_pdn_mgr_inst_type_s  *inst_p
)
{   
  ds_pdn_apm_ind_pay_load  ind_payload = {0};
  ds_pdn_event_info_u_type event_info;

  DS_MSG2(MSG_LEGACY_HIGH, "Instance up indication for type %d, PDN index %d",
  	       inst_p->type, pdn_mgr_p->index);

  memset(&event_info, 0, sizeof(ds_pdn_event_info_u_type));
  switch(inst_p->type)
  {
    case DS_PDN_INSTANCE_IPV4:      
      inst_p->state = DS_INSTANCE_STATE_UP;
      ds_flow_context_activate_ind(inst_p->flow->default_flow_ptr);

      //Inform APM that the V4 address has been configured.
      if (pdn_mgr_p->is_default)
      {                
        ds_pdn_apm_set_ipv4_addr(&ind_payload, TRUE);     
        ds_pdn_apm_set_profile_id(&ind_payload, pdn_mgr_p->pdp_profile_num);
        (void)ds_pdn_apm_ind(DS_PDN_APM_IP_ADDRESS_IND, &ind_payload);
      }
      break;

    case DS_PDN_INSTANCE_IPV6:
    	 inst_p->state = DS_INSTANCE_STATE_UP;

       //Check for cached v4 down indication if any and post CLAT SM creation
       if(ds_pdnmgr_is_prof_clat_enabled(pdn_mgr_p->pdp_profile_num) &&
          pdn_mgr_p->cache_v4_inst_down == TRUE)
       {
         ds_pdn_post_clat_sm_creation(pdn_mgr_p);
       }
      break;

    case DS_PDN_INSTANCE_NON_IP:      
      inst_p->state = DS_INSTANCE_STATE_UP;
      /*ds_flow_context_ds_flow_activate_ind(inst_p->flow.default_flow_ptr,
                                           DS_EIC_NOT_SPECIFIED);*/
      break;

    default:
      break;
  }

  // invoke PDN event to FW
  ds_fwk_notify_events( pdn_mgr_p->fw_index,
                     ds_pdn_mgr_convert_pdn_inst_to_fw_ip_type(inst_p->type),
                     PDN_UP_EV,
                     event_info);                                              
  
  return;
}

/*===========================================================================
FUNCTION ds_pdn_mgr_validate_pcscf_address

DESCRIPTION
  This function validates if the network returned PCSCF address is consistent
  with the rules specified in the APM. 
  If the rules defined in APM are not met by the instances in this PDN context;
  PDN connection is torn down.

PARAMETERS 
  pdn_mgr_p

DEPENDENCIES
  None

RETURN VALUE
  boolean - TRUE if PCSCF addreess is valid according to the rules
                 FALSE if  PCSCF adderess is not valid according to the rules
   
SIDE EFFECTS
  None
===========================================================================*/
boolean ds_pdn_mgr_validate_pcscf_address
(
  ds_pdn_mgr_s          *pdn_mgr_p,
  boolean               *v4_or_v6_pcscf_is_present
)
{
  ds_pdn_mgr_inst_type_s        *inst_p = NULL;
  ds_pdn_apm_req_pay_load        req_payload;  
  boolean                        v4_pcscf_is_present = FALSE;
  boolean                        v6_pcscf_is_present = FALSE;  
  boolean                        pcscf_addr_is_valid = TRUE;  
  uint8                          i = 0;
#ifdef FEATURE_DATA_LTE
  boolean                       throttling_is_allowed = FALSE;
                                 /* throttling_is_allowed flag ensures that we 
                                    don't increment throttling counter for LTE
                                    Attach Failure scenarios if this is a VZW 
                                    configuration. */
#endif /*FEATURE_DATA_LTE*/
  ds_pdn_apm_ind_pay_load       ind_payload = {0};
  boolean                       any_other_pdn_up = FALSE;
  ds_3gpp_throt_rat_e_type      rat_type = DS_3GPP_THROT_RAT_GLOBAL; 
                                /* RAT on which throttling needs to be
                                   performed */
  ds_3gpp_throttle_sm_cb_data_type      cb_data;
  ds_3gpp_pdn_throt_ident_info_type     throt_ident_info;
  ds_net_down_reason_type ds_down_reason = 
                                    DS_NET_DOWN_REASON_INVALID_PCSCF_ADDR;
  ds_command_type  *cmd_ptr   = NULL;
  /*---------------------------------------------------------------------------*/
  memset(&req_payload, 0, sizeof(ds_pdn_apm_req_pay_load));
  ds_3gpp_throt_fill_cb_data (&cb_data, NULL, NULL, (void*)pdn_mgr_p);

  if (ds_pdn_nv_manager_get_enable_pdn_throt_per_rat())
    {
      rat_type = ds_3gpp_pdn_throt_sm_sys_mode_to_rat_type
                   (pdn_mgr_p->nw_mode);
  }

  ds_3gpp_pdn_throt_fill_ident_info_ex
  (
     &throt_ident_info, 
     (byte*)pdn_mgr_p->pdp_profile->context.apn,
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


  /* We allow throttling in the following cases:
     1. Given PDN does not have is_default flag set.
     2. Given PDN has is_default flag set, but there is 
        another PDN which is already in UP State 
     3. VZW Configuration is not enabled
  */

  any_other_pdn_up = ds_pdn_mgr_is_any_other_pdn_up(pdn_mgr_p);

#ifdef FEATURE_DATA_LTE
  if (pdn_mgr_p->is_default == FALSE || 
      ds_pdn_nv_manager_get_regular_pdn_throttling_nv())
  {
    throttling_is_allowed = TRUE;
  }

  if (pdn_mgr_p->is_default == TRUE &&
      any_other_pdn_up == TRUE)
  {
    throttling_is_allowed = TRUE;
  }
#endif /*FEATURE_DATA_LTE*/

  /* Get the PCSCF rules from APM */  
  req_payload.profile_id = (uint16)pdn_mgr_p->pdp_profile_num;
  req_payload.rule_type = (TRUE == any_other_pdn_up) ? 
	                          DS_PDN_APM_ON_DEMAND_RULE : DS_PDN_APM_ATTACH_RULE;
  (void)ds_pdn_apm_req( DS_PDN_APM_MANDATORY_PCSCF_REQ, 
                        &req_payload );
  DS_MSG1(MSG_LEGACY_HIGH, "APM response to MANDATORY_PCSCF_REQ is %d",
          req_payload.mandatory_pcscf); 

  for (i = DS_PDN_INSTANCE_IPV4; 
       i <= DS_PDN_INSTANCE_IPV6; i++)
  {
    ps_ip_addr_type pscf_addr[DS_MAX_SIP_SERVER_ADDRESSES];
    uint8 count = DS_MAX_SIP_SERVER_ADDRESSES;
    uint8 index = 0;

    inst_p = pdn_mgr_p->inst_ptr[i];
    if (NULL == inst_p)
    {
      continue;
    }

    /* Get all the SIP addressess configured for this Iface and check if a V4 or 
    V6 address is present */
    ds_pdn_mgr_get_sip_serv_addr(pdn_mgr_p, i, pscf_addr, &count);

    for (index = 0; index < count; index ++)
    {
      if (IPV4_ADDR == pscf_addr[index].type)
      {
        v4_pcscf_is_present = TRUE;        
      }
      else if (IPV6_ADDR == pscf_addr[index].type)
      {
        v6_pcscf_is_present = TRUE;        
      }
    }

    switch(inst_p->type)
    {
      case DS_PDN_INSTANCE_IPV4 :
        if ( ipv4_mandatory_and_ipv6_donotcare == req_payload.mandatory_pcscf ||
             ipv4_mandatory_and_ipv6_mandatory == req_payload.mandatory_pcscf )
         { 
            /* V4 address was mandatory by the NW did not supply it */
            if (FALSE == v4_pcscf_is_present)
            {              
              pcscf_addr_is_valid = FALSE;

#ifdef FEATURE_DATA_LTE
              if (throttling_is_allowed
#ifdef FEATURE_DATA_CC_SP_THROTTLING
              &&
                 (ds_3gpp_throt_sm_is_cc_sp_throttling_enabled() != TRUE)
#endif /* FEATURE_DATA_CC_SP_THROTTLING */
                  )
              {
                throt_ident_info.pdp_type = DS_PROFILE_PDP_IPV4;

                ds_3gpp_pdn_throt_perform_reg_throttling(&throt_ident_info);
              }
#endif /*FEATURE_DATA_LTE*/
            }
        }
        break;

      case DS_PDN_INSTANCE_IPV6:
        if ( ipv6_mandatory_and_ipv4_donotcare == req_payload.mandatory_pcscf ||
             ipv4_mandatory_and_ipv6_mandatory == req_payload.mandatory_pcscf )
        { 
          /* V6 address was mandatory by the NW did not supply it. */
          if (FALSE == v6_pcscf_is_present)
          {          
            pcscf_addr_is_valid = FALSE;

#ifdef FEATURE_DATA_LTE
            if (throttling_is_allowed
#ifdef FEATURE_DATA_CC_SP_THROTTLING
            &&
               (ds_3gpp_throt_sm_is_cc_sp_throttling_enabled() != TRUE)
#endif /* FEATURE_DATA_CC_SP_THROTTLING */
            )
            {
              throt_ident_info.pdp_type = DS_PROFILE_PDP_IPV6;

              ds_3gpp_pdn_throt_perform_reg_throttling(&throt_ident_info);
            }
#endif /*FEATURE_DATA_LTE*/
          }
        }
        break;

      default:
        break;
    }
  }

  if (ipv4_mandatory_or_ipv6_mandatory == req_payload.mandatory_pcscf)
  {
    if ( (FALSE == v4_pcscf_is_present) &&
         (FALSE == v6_pcscf_is_present) )
    {
      pcscf_addr_is_valid = FALSE;
    }
  }

  /* No validation required in this case. */
  if (ipv4_dontcare_and_ipv6_donotcare == req_payload.mandatory_pcscf)
  {
    pcscf_addr_is_valid = TRUE;
  }

  if (FALSE == pcscf_addr_is_valid)
  {
    // Post a command to tear down PDN, as PDN clearing at this point will be an issue

    cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_pdn_mgr_s*));

    if(cmd_ptr == NULL || cmd_ptr->cmd_payload_ptr == NULL)
    {
      DS_MSG0(MSG_LEGACY_ERROR,"Memory allocation failed");
      DS_ASSERT(0);
      return pcscf_addr_is_valid;
    }

    cmd_ptr->hdr.cmd_id = DS_CMD_PDN_TEARDOWN_INVALID_PCSCF;
    cmd_ptr->cmd_payload_ptr = pdn_mgr_p ;
    ds_put_cmd(cmd_ptr);
  }

  //Inform DSD about the PCSCF address.
  if (FALSE == any_other_pdn_up)
  {
    if (v6_pcscf_is_present)
    {
      ds_pdn_apm_set_v6_pcscf_addr(&ind_payload, TRUE);
    }
    else 
    {
      ds_pdn_apm_set_v6_pcscf_addr(&ind_payload, FALSE);
    }

    if (v4_pcscf_is_present)
    {
      ds_pdn_apm_set_v4_pcscf_addr(&ind_payload, TRUE);
    }
    else 
    {
      ds_pdn_apm_set_v4_pcscf_addr(&ind_payload, FALSE);
    }    
    
    ds_pdn_apm_set_profile_id(&ind_payload, pdn_mgr_p->pdp_profile_num);
    (void)ds_pdn_apm_ind(DS_PDN_APM_IP_ADDRESS_IND, &ind_payload);  
  }

  *v4_or_v6_pcscf_is_present = (v4_pcscf_is_present | v6_pcscf_is_present);
  return pcscf_addr_is_valid;
}

/*===========================================================================
FUNCTION ds_pdn_mgr_validate_dns_addr

DESCRIPTION
  This function validates if the network returned DNS address is consistent
  with the rules specified in the APM. 
  If the rules defined in APM are not met by the instances in this PDN context;
  PDN connection is torn down.

PARAMETERS 
  pdn_mgr_p

DEPENDENCIES
  None

RETURN VALUE
  boolean - TRUE if DNS addreess is valid according to the rules
                FALSE if DNS adderess is not valid according to the rules
   
SIDE EFFECTS
  None
===========================================================================*/
boolean ds_pdn_mgr_validate_dns_addr
(
  ds_pdn_mgr_s                *pdn_mgr_p,
  boolean                     *v4_or_v6_dns_addr_valid
)
{
  ds_pdn_mgr_inst_type_s      *v4_inst_p = NULL;
  ds_pdn_mgr_inst_type_s      *v6_inst_p = NULL;
  boolean                      v4_dns_addr_valid = FALSE;
  boolean                      v6_dns_addr_valid = FALSE;
  boolean                      dns_addr_valid = TRUE;
  ds_pdn_apm_ind_pay_load      apm_ind_payload = {0};          
  ds_pdn_apm_req_pay_load      apm_req_payload = {0};  
  ds_fwk_s_type               *fw_inst_p = NULL;  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*---------------------------------------------------------------------------
    This function queries APM module if the PDN is an on-demand PDN to check
    whether either DNS address is mandatory,
   
    If either v4 or v6 is mandatory and its invalid then we initiate a PDN
    teardown
   
    For Attach PDN we indicate to APM the status of DNS address, both v4 & v6
  ---------------------------------------------------------------------------*/

  v4_inst_p = pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV4];
  v6_inst_p = pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV6];

  fw_inst_p = ds_fwk_get_inst_ptr_from_index(pdn_mgr_p->fw_index);
  if(!fw_inst_p)
  {
    DS_ERROR_FATAL("NULL FWK ptr !");
    return FALSE;
  }

  if(v4_inst_p)
  {
    if( (fw_inst_p->v4_net_info.primary_dns.ps_s_addr == 0) &&
    	   (fw_inst_p->v4_net_info.secondary_dns.ps_s_addr == 0) )
    {
      v4_dns_addr_valid = FALSE;
    }
    else
    {
      v4_dns_addr_valid = TRUE;
    }
  }

  if(v6_inst_p)
  {
    if( (fw_inst_p->v6_net_info.primary_dns.ps_s6_addr64[0] == 0) &&
    	   (fw_inst_p->v6_net_info.primary_dns.ps_s6_addr64[1] == 0) &&
    	   (fw_inst_p->v6_net_info.secondary_dns.ps_s6_addr64[0] == 0) &&
    	   (fw_inst_p->v6_net_info.secondary_dns.ps_s6_addr64[1] == 0) )
    {
      v6_dns_addr_valid = FALSE;
    }
    else
    {
      v6_dns_addr_valid = TRUE;
    }
  }

  if(FALSE == ds_pdn_mgr_is_any_other_pdn_up(pdn_mgr_p))
  {
    ds_pdn_apm_set_v4_dns_addr(&apm_ind_payload, v4_dns_addr_valid);
    ds_pdn_apm_set_v6_dns_addr(&apm_ind_payload, v6_dns_addr_valid);
    ds_pdn_apm_set_profile_id(&apm_ind_payload, pdn_mgr_p->pdp_profile_num);
    ds_pdn_apm_ind(DS_PDN_APM_IP_ADDRESS_IND, &apm_ind_payload);
  }
  else
  {
    apm_req_payload.profile_id = pdn_mgr_p->pdp_profile_num;
    apm_req_payload.rule_type = DS_PDN_APM_ON_DEMAND_RULE;
    ds_pdn_apm_req(DS_PDN_APM_MANDATORY_DNS_ADDR_REQ, &apm_req_payload);

    if(FALSE == v4_dns_addr_valid &&
       (ipv4_mandatory_and_ipv6_donotcare == apm_req_payload.mandatory_dns_addr ||
        ipv4_mandatory_and_ipv6_mandatory == apm_req_payload.mandatory_dns_addr))
    {
      dns_addr_valid = FALSE;
    }

    if(TRUE == dns_addr_valid &&
       FALSE == v6_dns_addr_valid && 
       (ipv6_mandatory_and_ipv4_donotcare == apm_req_payload.mandatory_dns_addr ||
        ipv4_mandatory_and_ipv6_mandatory == apm_req_payload.mandatory_dns_addr))
    {
      dns_addr_valid = FALSE;
    }

    if(TRUE == dns_addr_valid &&
       FALSE == v6_dns_addr_valid &&
       FALSE == v4_dns_addr_valid &&
       ipv4_mandatory_or_ipv6_mandatory == apm_req_payload.mandatory_dns_addr)
    {
      dns_addr_valid = FALSE;
    }

    if (FALSE == dns_addr_valid)
    {
      // Tear down PDN with reason DS_NET_DOWN_REASON_INVALID_DNS_ADDR
      // Post a command
    }
  }

  *v4_or_v6_dns_addr_valid = (v4_dns_addr_valid | v6_dns_addr_valid);
  return dns_addr_valid;
}/* ds_pdn_mgr_validate_dns_addr */

/*===========================================================================
FUNCTION ds_pdn_mgr_validate_pcscf_dns_address

DESCRIPTION
  This function validates if there is a requirement for an APN to have either
  for DNS or PCSCF address
 
  If the requirement is not met then the function triggers a PDN teardown

PARAMETERS 
  pdn_context_p (IN) :  PDN context object

DEPENDENCIES
  None

RETURN VALUE
  boolean - TRUE if the requirement to have either PCSCF or DNS is met
            FALSE if the above requirement is not met
   
SIDE EFFECTS
  None
===========================================================================*/
boolean ds_pdn_mgr_validate_pcscf_dns_addr
(
  ds_pdn_mgr_s                       *pdn_mgr_p,
  boolean                             dns_addr_valid,
  boolean                             pcscf_addr_valid
)
{
  boolean                             pcscf_or_dns_addr_valid = FALSE;
  ds_pdn_apm_req_pay_load             apm_req_payload = {0};      
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*---------------------------------------------------------------------------
    This function queries APM module if the PDN is an on-demand PDN to check
    whether either of PCSCF or DNS address is mandatory,
   
    If its mandatory and we have neither of them valid (either v4 or v6)
    then we initiate a PDN teardown
  ---------------------------------------------------------------------------*/

  if(FALSE == dns_addr_valid && FALSE == pcscf_addr_valid)
  {
    if(TRUE == ds_pdn_mgr_is_any_other_pdn_up(pdn_mgr_p))
    {
      apm_req_payload.profile_id = pdn_mgr_p->pdp_profile_num;
      apm_req_payload.rule_type = DS_PDN_APM_ON_DEMAND_RULE;
      ds_pdn_apm_req(DS_PDN_APM_MANDATORY_PCSCF_OR_DNS_ADDR_REQ,
                     &apm_req_payload);

      if((ipv4_mandatory_or_ipv6_mandatory == 
                          apm_req_payload.mandatory_pcscf_or_dns_addr))
      {
        // Tear down PDN with reason DS_NET_DOWN_REASON_INVALID_PCSCF_DNS_ADDR);   
        // Post a command
      }
      else
      {
        pcscf_or_dns_addr_valid = TRUE;
      }
    }
    else
    {
      pcscf_or_dns_addr_valid = TRUE;
    }
  }
  else
  {
    apm_req_payload.profile_id = pdn_mgr_p->pdp_profile_num;
    apm_req_payload.rule_type = DS_PDN_APM_ON_DEMAND_RULE;
    ds_pdn_apm_req(DS_PDN_APM_MANDATORY_PCSCF_OR_DNS_ADDR_REQ,
                        &apm_req_payload);
    DS_MSG3(MSG_LEGACY_HIGH,
    	       "Value for PCSCF or DNS address in APM : %d, "
    	       "DNS addr %d, PCSCF addr %d", 
            apm_req_payload.mandatory_pcscf_or_dns_addr,
            dns_addr_valid, pcscf_addr_valid);
    pcscf_or_dns_addr_valid = TRUE;
  }

  return pcscf_or_dns_addr_valid;
}

#ifdef FEATURE_DATA_LTE
/*===========================================================================
FUNCTION  DS_PDN_MGR_IS_LAST_PREVAILING_PDN_IN_LTE

DESCRIPTION
  Indicates if the passed PDN context is the only prevailing one
  in LTE mode. 
  
PARAMETERS
  pdn_mgr_p

DEPENDENCIES
  None.

RETURN VALUE
  True - If it is the last prevailing PDN
  False - Otherwise

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_pdn_mgr_is_last_prevailing_pdn_in_lte
(
  ds_pdn_mgr_s *pdn_mgr_p 
)
{
  if (TRUE == ds_eps_attach_hdlr_get_nw_attach_wo_pdn_support())
  {
    return FALSE;
  }
  
  /*------------------------------------------------------------------------ 
    Return true if all the below conditions are satisfed
    1. System mode is LTE
    2. Attached
    3. No other PDN is UP
    ------------------------------------------------------------------------*/
  if ( (pdn_mgr_p->nw_mode == SYS_SYS_MODE_LTE) &&
       (ds_eps_pdn_attach_sm_get_state() == DS_EPS_ATTACHED) &&
       (!ds_pdn_mgr_is_any_other_pdn_up(pdn_mgr_p)) )
  {
    DS_MSG0(MSG_LEGACY_HIGH, "Last standing PDN in LTE, returning TRUE");
    return TRUE;
  }

  return FALSE;
}
#endif

/*===========================================================================
FUNCTION DS_PDN_MGR_BRING_DOWN_BEARER

DESCRIPTION
  This function brings down default bearer and tears down the PDN instance.
  
PARAMETERS 
  pdn_mgr_p - PDN control block
  instance type - v4/v6/nonIP
  tear_down_reason - reason for initiating PDN tear down.
  
DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_pdn_mgr_bring_down_bearer
( 
  ds_pdn_mgr_s               *pdn_mgr_p,
  ds_pdn_mgr_instance_type_e  inst_type
)
{
  ds_pdn_mgr_inst_type_s  *inst_p = NULL;
  ds_pdn_mgr_inst_type_s  *comp_inst_p = NULL;
  ds_bearer_mgr_s         *def_bearer = NULL;

  if(!ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p))
  {
    return;
  }

  inst_p = pdn_mgr_p->inst_ptr[inst_type];
  if( !inst_p || 
      inst_p->state == DS_INSTANCE_STATE_DOWN)
  {
    return;
  }

  inst_p->state = DS_INSTANCE_STATE_GOING_DOWN;
  switch(inst_type)
  {
    case DS_PDN_INSTANCE_IPV4:
      comp_inst_p = pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV6];
      ds_pdn_flow_manager_inst_down_hdlr(pdn_mgr_p, inst_p);
      break;

    case DS_PDN_INSTANCE_IPV6:
      comp_inst_p = pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV4];
      ds_pdn_flow_manager_inst_down_hdlr(pdn_mgr_p, inst_p);
      break;      

    default:
      break;
  }

  def_bearer = pdn_mgr_p->bearer_ptr_tbl[DS_3GPP_DEFAULT_BEARER_INDEX];
  if(!ds_bearer_mgr_validate_bearer_ptr(def_bearer))
  {    
    if( !comp_inst_p ||
        comp_inst_p->state == DS_INSTANCE_STATE_DOWN)
    {
      ds_pdn_mgr_cleanup_pdn(pdn_mgr_p, inst_type);
    }
    else
    {
      ds_pdn_mgr_cleanup_inst(pdn_mgr_p, inst_type);
    }
  }
  else
  {    
    if( (!comp_inst_p || (inst_type == DS_PDN_INSTANCE_IPV6 &&
          pdn_mgr_p->cache_v4_inst_down == TRUE)) ||
        ( (FALSE == comp_inst_p->in_use) 
#ifdef FEATURE_DATA_LTE
          && (!ds_pdn_mgr_is_last_prevailing_pdn_in_lte(pdn_mgr_p))
#endif                   
        ) ||
       (DS_PDN_MGR_STATE_GOING_DOWN == pdn_mgr_p->state) )
    {
      ds_bearer_mgr_disable_tx_flow(def_bearer, DS_BEARER_CONTROL_MASK);
      ds_bearer_mgr_tear_down_bearer(pdn_mgr_p, def_bearer);
    }
    else
    {
      if(inst_type == DS_PDN_INSTANCE_IPV4)
      {
        ds_apn_table_update_allowed_ip_type(pdn_mgr_p, DS_IPV6_ONLY);
      }
      else if(inst_type == DS_PDN_INSTANCE_IPV6)
      {
        ds_apn_table_update_allowed_ip_type(pdn_mgr_p, DS_IPV4_ONLY);
      }

      ds_pdn_mgr_tear_down_instance(pdn_mgr_p, inst_type,
                                    DS_NET_DOWN_REASON_NOT_SPECIFIED);
    }
  }

  return;  
}/* ds_pdn_mgr_bring_down_bearer */

/*===========================================================================
FUNCTION DS_PDN_MGR_INST_DOWN_HDLR

DESCRIPTION
  This function tears down the PDN instance.
  
PARAMETERS 
  pdn_mgr_p - PDN control block
  instance type - v4/v6/nonIP
  tear_down_reason - reason for initiating PDN tear down.
  
DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_pdn_mgr_inst_down_hdlr
( 
  ds_pdn_mgr_s               *pdn_mgr_p,
  ds_pdn_mgr_instance_type_e  inst_type
)
{
  ds_pdn_mgr_inst_type_s  *inst_p = NULL;
  ds_pdn_mgr_inst_type_s  *comp_inst_p = NULL;
  ds_pdn_v6_state_type     dssnet_state;


  inst_p = pdn_mgr_p->inst_ptr[inst_type];
  if( !inst_p || 
      inst_p->state == DS_INSTANCE_STATE_DOWN)
  {
    return;
  }

  inst_p->state = DS_INSTANCE_STATE_GOING_DOWN;
  switch(inst_type)
  {
    case DS_PDN_INSTANCE_IPV4:
      comp_inst_p = pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV6];
      if( comp_inst_p && 
          comp_inst_p->state != DS_INSTANCE_STATE_DOWN)
      {
        ds_pdn_flow_manager_inst_down_hdlr(pdn_mgr_p, inst_p);
        return;
      }
      break;

    case DS_PDN_INSTANCE_IPV6:
      dssnet_state = inst_p->ds_pdn_v6_sm_cb->state;
      if( (dssnet_state ==  DS_PDN_V6_WAITING_FOR_NET_UP_STATE) ||
          (dssnet_state ==  DS_PDN_V6_NET_UP_STATE) )       
      {
        ds_pdn_v6_process_ip6_stop_ev(inst_p->ds_pdn_v6_sm_cb);
        return;
      }      
      break;

    default:
      break;
  }

  ds_pdn_mgr_bring_down_bearer(pdn_mgr_p, inst_type);
  return;
}/* ds_pdn_mgr_inst_down_hdlr */

/*===========================================================================
FUNCTION DS_PDN_MGR_TEARDOWN_INSTANCES_FOR_PDN

DESCRIPTION
  This function tears down the PDN connection.
  
PARAMETERS 
  pdn_mgr_p - PDN control block
  tear_down_reason - reason for initiating PDN tear down.
  
DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_pdn_mgr_teardown_instances_for_pdn
( 
  ds_pdn_mgr_s            *pdn_mgr_p,
  ds_net_down_reason_type  tear_down_reason
)
{ 
  uint8 index;

  pdn_mgr_p->net_down_reason = tear_down_reason;
  for(index = DS_PDN_INSTANCE_IPV4; index < DS_PDN_INSTANCE_MAX; index++)
  {
    if(pdn_mgr_p->inst_ptr[index] != NULL)
    {
      ds_pdn_mgr_inst_down_hdlr(pdn_mgr_p, index);
    }
  }

  return;
}

/*===========================================================================
FUNCTION  ds_pdn_mgr_remove_unused_pdn

DESCRIPTION
  This function when invoked tears down all pdn connections which does not
  have active clients

PARAMETERS
  pdn_mgr_p

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_pdn_mgr_remove_unused_pdn
(
  ds_pdn_mgr_s *pdn_mgr_p
)
{
  uint8                    pdn_inst       = 0;
  ds_pdn_mgr_s            *temp_pdn_mgr_p = NULL;
  ds_pdn_mgr_inst_type_s  *v4_inst        = NULL;
  ds_pdn_mgr_inst_type_s  *v6_inst        = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_MSG0(MSG_LEGACY_HIGH, "Remove unused pdn function ");

  /*-----------------------------------------------------------------------
  Loop over all PDN contexts to find out which ones we have to tear
  -----------------------------------------------------------------------*/
  for(pdn_inst = 0; pdn_inst < DS_3GPP_MAX_PDN_CONTEXT; pdn_inst++)
  {
    temp_pdn_mgr_p = ds_pdn_manager[pdn_inst];
    /*---------------------------------------------------------------------
    If the PDN context is in DOWN state, continue with the next one
    ---------------------------------------------------------------------*/
    if( (!ds_pdn_mgr_validate_pdn_ptr(temp_pdn_mgr_p)) || 
        (pdn_mgr_p == temp_pdn_mgr_p) ||
        (temp_pdn_mgr_p->state == DS_PDN_MGR_STATE_DOWN)  ||
        (temp_pdn_mgr_p->state == DS_PDN_MGR_STATE_GOING_DOWN) )
    {
      continue;
    }

    v4_inst = temp_pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV4];
    v6_inst = temp_pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV6];

    /* Check the IN USE Flag and in the case of both instances being
       false we teardown the PDN */ 

    if (v4_inst == NULL && v6_inst == NULL)
    { 
       DS_ERROR_LOG_0("Trying to teardown PDN which does not have valid instances ");
       return;
    }
    
    if( (v4_inst && v4_inst->in_use) || 
    	   (v6_inst && v6_inst->in_use) )
    {
      continue;
    }
    else
    {
      DS_MSG1(MSG_LEGACY_HIGH,
      	       "PDN %d is being torn down because its not in use",
      	       temp_pdn_mgr_p->index);

      temp_pdn_mgr_p->state = DS_PDN_MGR_STATE_GOING_DOWN;
      ds_pdn_mgr_teardown_instances_for_pdn
                   (temp_pdn_mgr_p,DS_NET_DOWN_REASON_INTERNAL_CALL_ENDED);
    } 
  } /* loop over all PDN contexts*/
} /* ds_pdn_mgr_remove_unused_pdn */

/*===========================================================================
FUNCTION DS_PDN_MGR_BEARER_UP_IND

DESCRIPTION
  This function completes PDN and instance configuration in response to 
  bearer up ind.

PARAMETERS
  PDN pointer
  bearer pointer

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS  
  None.
===========================================================================*/
void ds_pdn_mgr_bearer_up_ind
( 
  ds_pdn_mgr_s           *pdn_mgr_p,
  ds_bearer_mgr_s        *bearer_mgr_p
)
{ 
  ds_pdn_mgr_inst_type_s        *v4_inst = NULL;
  ds_pdn_mgr_inst_type_s        *v6_inst = NULL;
  ds_pdn_mgr_inst_type_s        *non_ip_inst = NULL;
  ds_profile_pdp_type_e          pdp_type;
  ds_profile_auth_enum_type      requested_auth_type;
  ds_pdn_network_params_info_type  *info_p = NULL;
  boolean                        valid_pcscf_address = FALSE;
#ifdef FEATURE_MODE_TLB
  ds_eps_tlb_per_subs_info_type   *eps_tlb_hndl = NULL;
#endif /* FEATURE_MODE_TLB */
  boolean                        remove_unused_pdn = FALSE;
  boolean                        dns_addr_valid = FALSE;
  boolean                        pcscf_or_dns_addr_valid = FALSE;
  boolean                        v4_or_v6_pcscf_present = FALSE; 
  boolean                        v4_or_v6_dns_present = FALSE; 
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_MSG1(MSG_LEGACY_HIGH,"Bearer up indication for index %d",
          bearer_mgr_p->index);

  info_p = &(pdn_mgr_p->info);
  if(bearer_mgr_p->is_default)
  {
    if(info_p &&
       ((info_p->valid_fields & NET_CFG_PRM_AUTH_PREF_MASK)     ||
        (info_p->valid_fields & NET_CFG_PRM_AUTH_USERNAME_MASK) ||
        (info_p->valid_fields & NET_CFG_PRM_AUTH_PASSWORD_MASK) ))
    {
      requested_auth_type = (ds_profile_auth_enum_type) 
                                       pdn_mgr_p->info.auth_info.auth_type;
    }
    else
    {
      requested_auth_type = pdn_mgr_p->pdp_profile->auth.auth_type;
    }     

    DS_MSG1(MSG_LEGACY_HIGH,
    	       "Read auth_type %d from profile/info", requested_auth_type);
    
    /* Indicate to the auth manager that the bearer activation succeeded*/ 
    ds_pdn_auth_mgr_call_success(pdn_mgr_p->pdp_profile->context.apn, 
                                  requested_auth_type);
  }

#ifdef FEATURE_MODE_TLB
   eps_tlb_hndl = ds_eps_tlb_get_info_hndl();
#endif /* FEATURE_MODE_TLB */

  pdp_type = pdn_mgr_p->pdp_profile->context.pdp_type;

  /* Register with A2 and do TLB setup */  // come back later
  if(pdp_type != DS_PROFILE_PDP_PPP)
  {
#ifdef FEATURE_MODE_TLB
    if((ds_eps_tlb_validate_info_hndl(eps_tlb_hndl)))
    {
      switch(eps_tlb_hndl->tlb_state.mode)
      {
        case LTE_TLB_MODE_A:
        	 dsEpsTlb_set_tlb_mode_A_params(bearer_mgr_p->index);
          ds_EpsTlb_register_a2_to_ps(bearer_mgr_p);
          break;

        case LTE_TLB_MODE_G:
        	 ds_eps_tlb_set_tlb_mode_G_params(bearer_mgr_p->index);
          break;

          case LTE_TLB_MODE_H:
        	 ds_eps_tlb_set_tlb_mode_H_params(bearer_mgr_p->index);
          break;

        default:
          ds_fwk_reg_bearer_with_hw(bearer_mgr_p);
           break;
      }
    }
    else
    {
      ds_fwk_reg_bearer_with_hw(bearer_mgr_p);
    }
#else
     ds_fwk_reg_bearer_with_hw(bearer_mgr_p);
#endif /*FEATURE_MODE_TLB*/
 }

  /*-------------------------------------------------------------------------
    If this is a dedicated bearer no need to bring up the instance.    
    Also process the bearer up indication only if PDN context is not in UP 
    state already.
    This check is needed to filter out any duplicate bearer up indication that
    may be sent during abnormal scenarios.
  -------------------------------------------------------------------------*/
  if((bearer_mgr_p->is_default != TRUE) ||
     (pdn_mgr_p->state == DS_PDN_MGR_STATE_UP) ||
     (pdn_mgr_p->state == DS_PDN_MGR_STATE_GOING_DOWN))
  {
    DS_MSG1(MSG_LEGACY_HIGH, 
    	       "Received dedicated or duplicate bearer up ind. bearer_cnt:%d",
            pdn_mgr_p->num_active_bearer_context);
    return;
  }

  v4_inst = pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV4];
  v6_inst = pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV6];
  non_ip_inst = pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_NON_IP];
  
  /*--------------------------------------------------------------------------
  Send IFACE UP indication for the primary. 
  Do not send IFACE UP in case if TLB Loopback mode A is active
  --------------------------------------------------------------------------*/
#ifdef FEATURE_MODE_TLB
  if ( (pdn_mgr_p->nw_mode == SYS_SYS_MODE_GSM) ||
  	    (( pdn_mgr_p->nw_mode == SYS_SYS_MODE_LTE &&
  	      bearer_mgr_p->is_in_tlb == FALSE ) ||
       ( ds_eps_tlb_validate_info_hndl(eps_tlb_hndl) &&
         eps_tlb_hndl->tlb_state.mode != LTE_TLB_MODE_A &&
         eps_tlb_hndl->tlb_state.mode != LTE_TLB_MODE_G &&
         eps_tlb_hndl->tlb_state.mode != LTE_TLB_MODE_H )))
#endif /* FEATURE_MODE_TLB */
  {
    if (v4_inst && !pdn_mgr_p->cache_v4_inst_down) 
    {
      ds_pdn_mgr_instance_up_ind(pdn_mgr_p, v4_inst); 
    }    
    
    if (v6_inst) 
    {      
      /*--------------------------------------------------------------------
          Valid IID for V6 has been received. IPV6 prefix is neede for this
          Iface to start to bring up the interface. Send Link up event to
          DSSNET6 SM which will initiate router solicitation for Prefix.
         ----------------------------------------------------------------------*/  
      if (pdn_mgr_p->is_restore_mode) 
      {
         v6_inst->ds_pdn_v6_sm_cb->user_data_p = (void*)(TRUE);
      }

      /* Bind the default flow for v6 to bearer before instance up for RS/RA */
      if(ds_flow_is_valid(v6_inst->flow->default_flow_ptr))
      {
        v6_inst->flow->default_flow_ptr->bearer_ptr = bearer_mgr_p;
      }
      /* Activate the flow for v6 as RS/RA needs to be transmitted */
      ds_flow_context_activate_ind(v6_inst->flow->default_flow_ptr);    	 
      ds_pdn_v6_process_link_up_ev(v6_inst->ds_pdn_v6_sm_cb);      
    }
    
    if (non_ip_inst) 
    {
      ds_pdn_mgr_instance_up_ind(pdn_mgr_p, non_ip_inst);
    }
  }
    
  //Validate PCSCF address.
  valid_pcscf_address = ds_pdn_mgr_validate_pcscf_address(pdn_mgr_p,
                                                     &v4_or_v6_pcscf_present);
  //Validate DNS address.  
  dns_addr_valid = ds_pdn_mgr_validate_dns_addr(pdn_mgr_p,
                                                &v4_or_v6_dns_present);
  //Validate PCSCF or DNS address.
  pcscf_or_dns_addr_valid = ds_pdn_mgr_validate_pcscf_dns_addr(
                                                        pdn_mgr_p,
                                                        v4_or_v6_dns_present,
                                                        v4_or_v6_pcscf_present);
    
  /*----------------------------------------------------------------------------
    Move the state of the PDN context to UP
  ----------------------------------------------------------------------------*/    
  if (pdn_mgr_p->state == DS_PDN_MGR_STATE_COMING_UP)
  {
    pdn_mgr_p->state = DS_PDN_MGR_STATE_UP;
    DS_MSG1(MSG_LEGACY_HIGH, "Moving PDN %d to UP state",pdn_mgr_p->index);
    
    if(pdn_mgr_p->pdp_profile->inactivity_timer_val > 0)
    {
      DS_MSG1(MSG_LEGACY_HIGH, "Starting PDN Inactivity timer: %d seconds",
              pdn_mgr_p->pdp_profile->inactivity_timer_val);

      ps_timer_start(pdn_mgr_p->pdn_inactivity_timer, 
                          (pdn_mgr_p->pdp_profile->inactivity_timer_val)*1000);

      /*------------------------------------------------------------------------
        Start the polling timer so that we can query the lower layers about
        data activity when this expires.
      ------------------------------------------------------------------------*/
      ps_timer_start(pdn_mgr_p->polling_timer, DS_3GPP_POLLING_INTERVAL);
    } /* pdn_inactivity_timer > 0 */

  }
  
  /*
    Enter the below code only if NV 70334 is set which needs to removes unused
    pdn with no client associated with them. PCSCF address is checked so that
    the up-coming PDN is not torn down 
  */
#ifdef FEATURE_DATA_LTE
   v4_inst = pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV4];
   remove_unused_pdn = ds_pdn_nv_manager_get_remove_unused_pdn();
   if ( (remove_unused_pdn)  &&
        (v4_inst && (v4_inst->in_use)) && 
        (valid_pcscf_address == TRUE) && 
        (dns_addr_valid == TRUE) &&
        (pcscf_or_dns_addr_valid == TRUE) )
  {
    ds_pdn_mgr_remove_unused_pdn(pdn_mgr_p);
  }
#endif /* FEATURE_DATA_LTE*/

#ifdef FEATURE_DATA_RPM
  if(ds_3gpp_is_rpm_enabled())
  {
    ds_3gpp_rpm_handle_pdp_act_accept((byte*)pdn_mgr_p->pdp_profile->context.apn);
  }
#endif

  return;
} /* ds_pdn_mgr_bearer_up_ind */

/*===========================================================================
FUNCTION ds_pdn_mgr_execute_cached_deactivation()

DESCRIPTION

PARAMETERS
  pdn context
  bearer context

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_mgr_execute_cached_deactivation
(
  ds_pdn_mgr_s          *pdn_mgr_p,
  ds_bearer_mgr_s       *bearer_mgr_p
)
{
  if(!(ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p) && 
       ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p)))
  {
    DS_MSG0(MSG_LEGACY_ERROR, "Invalid input parameters");
    return;
  }
  
  if(bearer_mgr_p->eps_bearer && (bearer_mgr_p->eps_bearer->deact_req_cached == TRUE))
  {
    DS_MSG0(MSG_LEGACY_HIGH, "Deactivation request is cached, deactivate now");
    
    if(bearer_mgr_p->eps_bearer->cached_deact_params != NULL)
    {
      ds_eps_pdn_mgr_bearer_deactivate_cnf(bearer_mgr_p->eps_bearer->cached_deact_params,
                                         bearer_mgr_p, pdn_mgr_p);
      DS_SYSTEM_HEAP_MEM_FREE(bearer_mgr_p->eps_bearer->cached_deact_params);
    }
    bearer_mgr_p->eps_bearer->deact_req_cached = FALSE;
  }
  return;
}

/*===========================================================================
FUNCTION ds_pdn_mgr_is_bearer_deactivated_by_network

DESCRIPTION
  This function is used to check if the bearer is deactivated by the network
  Whenever the bearer is deactivated by the network PDN contexts
  1. Force OTA teardown must not be set
  2. UE initiated teardown must not be set

PARAMETERS
  pdn context
  bearer context

DEPENDENCIES
  None.

RETURN VALUE
  boolean

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_mgr_is_bearer_deactivated_by_network
(
   ds_pdn_mgr_s          *pdn_mgr_p,
   ds_bearer_mgr_s       *bearer_mgr_p
)
{
  boolean ret_val = FALSE;
  
  if(((bearer_mgr_p->state == DS_BEARER_MGR_STATE_UP) ||
      (bearer_mgr_p->state == DS_BEARER_MGR_STATE_UP_DORMANT) ||
      (bearer_mgr_p->state == DS_BEARER_MGR_STATE_UP_DORMANT_REESTAB)) &&
     (!pdn_mgr_p->ue_initiated_tear_down) &&
     (!pdn_mgr_p->force_ota_teardown) &&
     (pdn_mgr_p->num_active_bearer_context == 1))
  {
    ret_val = TRUE;
  }

  DS_MSG1(MSG_LEGACY_HIGH,
  	       "is_bearer_deactivated_by_network returns %d", ret_val);
  return ret_val;
} /* ds_pdn_mgr_is_bearer_deactivated_by_network */

/*===========================================================================
FUNCTION DS_PDN_MGR_PROCESS_TECH_SUSPEND_IND

DESCRIPTION
  This function processes tech suspension indication from NAS.

PARAMETERS
 Tech suspension information

DEPENDENCIES
  None.

RETURN VALUE  
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_pdn_mgr_process_tech_suspend_ind
( 
  mm_wwan_tech_suspend_ind_s_type  *info_ptr
)
{
  uint8                loop_index = 0;
  ds_bearer_mgr_s     *bearer_mgr_p = NULL;
  ds_pdn_event_info_u_type    event_info;
  ds_pdn_mgr_instance_type_e  inst_type;
  /*------------------------------------------------------------------------------------*/

  if(!info_ptr)
  {
    return;
  }

  DS_MSG1(MSG_LEGACY_HIGH, "WWAN Tech suspend set to %d", 
          info_ptr->tech_suspend);

  ds_pdn_mgr_wwan_tech_suspend = info_ptr->tech_suspend;

  /*--------------------------------------------------------------------
    Once WWAN tech is loaded, check if we have any cached bearer pending for PDCP 
    deregistration, if so process and free them. The bearers will be cached if deactivation 
    happens when PDN gets disconnected during tech suspension in connected mode.
    --------------------------------------------------------------------*/
  if(ds_pdn_mgr_wwan_tech_suspend == FALSE)
  {
    for(loop_index = 0;loop_index < DS_3GPP_MAX_BEARER_CONTEXT; loop_index++)
    {
      bearer_mgr_p = ds_bearer_mgr_get_cached_instance_ptr(loop_index);
      if(ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p))
      {
        ds_eps_bearer_mgr_deregister_ul_with_pdcp(bearer_mgr_p);
      }
    }
  }

  for(loop_index = 0;loop_index < DS_3GPP_MAX_BEARER_CONTEXT; loop_index++)
  {
    bearer_mgr_p = ds_bearer_mgr_get_instance_ptr(loop_index);
    if(ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p))
    {
      if(info_ptr->tech_suspend)
      {
        ds_bearer_mgr_disable_tx_flow(bearer_mgr_p, 
                                      DS_BEARER_TECH_SUSPENSION_MASK);
      }
      else
      {
        ds_bearer_mgr_enable_tx_flow(bearer_mgr_p, 
                                     DS_BEARER_TECH_SUSPENSION_MASK);
      }

      memset(&event_info,0,sizeof(ds_pdn_event_info_u_type));
      event_info.tech_suspension_status = info_ptr->tech_suspend;
      
      for(inst_type = DS_PDN_INSTANCE_IPV4;
          inst_type < DS_PDN_INSTANCE_MAX;
          inst_type++)
      {
        ds_fwk_notify_events( bearer_mgr_p->pdn_mgr_ptr->fw_index,
                              ds_pdn_mgr_convert_pdn_inst_to_fw_ip_type(inst_type),
                              PDN_WWAN_TECH_SUSPENSION_EV,
                              event_info); 
      }
    }
  }

  return;
} /* ds_pdn_mgr_process_tech_suspend_ind */

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */

/*===========================================================================
FUNCTION DS_PDN_MGR_IS_WWAN_TECH_SUSPENDED

DESCRIPTION
  This function return the status whether WWAN tech is suspended.

PARAMETERS
 None.

DEPENDENCIES
  None.

RETURN VALUE  
  TRUE - IF WWAN tech is suspended.
  FALSE - otherwise.

SIDE EFFECTS
  None
===========================================================================*/
boolean ds_pdn_mgr_is_wwan_tech_suspended
(
  void
)
{
  return ds_pdn_mgr_wwan_tech_suspend;
} /* ds_pdn_mgr_is_wwan_tech_suspended */

/*===========================================================================
FUNCTION DS_PDN_MGR_CLEAR_IRAT_INFO

DESCRIPTION
  This function clears the IRAT information.

PARAMETERS
 None.

DEPENDENCIES
  None.

RETURN VALUE  
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_pdn_mgr_clear_irat_info
( 
  void
)
{
  memset(&ds_pdn_mgr_irat_info, 0, sizeof(ds_pdn_mgr_irat_info_s));
  return;
} /* ds_pdn_mgr_complete_irat */

/*===========================================================================
FUNCTION DS_PDN_MGR_IS_IRAT_IN_PRORESS

DESCRIPTION
  This function retrun the status if IRAT procedure is ongoing.

PARAMETERS
 None.

DEPENDENCIES
  None.

RETURN VALUE  
  TRUE - IF IRAT is in progress.
  FALSE - otherwise.

SIDE EFFECTS
  None
===========================================================================*/
boolean ds_pdn_mgr_is_irat_in_progress
( 
  void
)
{
  return ds_pdn_mgr_irat_info.in_progress;
} /* ds_pdn_mgr_is_irat_in_progress */

/*===========================================================================
FUNCTION DS_PDN_MGR_COMPLETE_IRAT

DESCRIPTION
  This function completes the IRAT procedure.

PARAMETERS
 None.

DEPENDENCIES
  None.

RETURN VALUE  
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_pdn_mgr_complete_irat
( 
  void
)
{
  if( ds_pdn_mgr_irat_info.in_progress &&
      ds_pdn_mgr_irat_info.trat == emm_get_active_rat())
  {    
    DS_MSG1(MSG_LEGACY_HIGH, "IRAT successful to TRAT %d", 
            ds_pdn_mgr_irat_info.trat);  

    ds_pdn_mgr_active_rat = ds_pdn_mgr_irat_info.trat;
    memset(&ds_pdn_mgr_irat_info, 0, sizeof(ds_pdn_mgr_irat_info_s));
  }

  return;
} /* ds_pdn_mgr_complete_irat */

/*===========================================================================
FUNCTION DS_PDN_MGR_PROCESS_CONTEXT_TRANSFER_IND

DESCRIPTION
  This function processes context transfer indication from NAS.

PARAMETERS
 Context transfer information

DEPENDENCIES
  None.

RETURN VALUE  
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_pdn_mgr_process_context_transfer_ind
( 
  emm_bearer_ctxt_transfer_ind_type  *info_ptr
)
{
  if(!info_ptr)
  {
    return;
  }

  DS_MSG2(MSG_LEGACY_HIGH, "Context transfer from SRAT %d to TRAT %d", 
          ds_pdn_mgr_active_rat, info_ptr->rat);

  ds_pdn_mgr_irat_info.in_progress = TRUE;
  ds_pdn_mgr_irat_info.srat = ds_pdn_mgr_active_rat;
  ds_pdn_mgr_irat_info.trat = info_ptr->rat;

  // Trat this indication as PS data suspend for all the bearers.
  ds_pdn_hdlr_ps_data_suspend_resume_hdlr(TRUE);

  return;
} /* ds_pdn_mgr_process_tech_suspend_ind */

/*===========================================================================
FUNCTION DS_PDN_MGR_TEARDOWN_PDN_PER_PROFILE

DESCRIPTION
  This function tears down the PDN Connections
  using the passed profile number.

PARAMETERS
 Profile Number
 DOWN Reason to be given to Client Applications

DEPENDENCIES
  None.

RETURN VALUE  
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_pdn_mgr_teardown_pdn_per_profile
( 
  uint8                         profile_num,
  ds_net_down_reason_type       down_reason
)
{
  uint8             loop_index = 0;
  ds_pdn_mgr_s     *pdn_mgr_p = NULL;
  /*------------------------------------------------------------------------------------*/

  for(loop_index = 0;loop_index < DS_3GPP_MAX_PDN_CONTEXT; loop_index++)
  {
    pdn_mgr_p = ds_pdn_manager[loop_index];
    if (ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p))
    {
      if( pdn_mgr_p->pdp_profile_num == profile_num )
      {
        DS_MSG1(MSG_LEGACY_HIGH,
        	       "Tearing Down PDN with Profile: %d",profile_num);

        pdn_mgr_p->state = DS_PDN_MGR_STATE_GOING_DOWN;
        ds_pdn_mgr_teardown_instances_for_pdn(pdn_mgr_p, down_reason);
      }
    }
  }

  return;
} /* ds_pdn_mgr_teardown_pdn_per_profile */


/*===========================================================================
FUNCTION DS_PDN_MGR_PENDING_MO_EX_DATA_IND_HDLR
 
DESCRIPTION
  This function Handles pending_mo_ex_data indication from NAS.

PARAMETERS 
  pending_mo_ex_data
 
DEPENDENCIES 
  None. 
  
RETURN VALUE 
  boolean
 
SIDE EFFECTS 
  
===========================================================================*/


void ds_pdn_mgr_pending_mo_ex_data_ind_hdlr
(
  emm_process_pending_mo_exception_data_s_type *pending_mo_ex_data
)
{
  if(pending_mo_ex_data == NULL)
  {
    DS_MSG0(MSG_LEGACY_HIGH, "Invalid data");
	return;
  }
  if(pending_mo_ex_data->is_mo_exception_to_be_cleared)
  {
    ds_pdn_cp_data_cleanup_inst_after_force_attach_failure(
		                                 DS_NET_DOWN_REASON_NOT_SPECIFIED);
  }
  else if(ds_eps_pdn_attach_sm_get_state() == DS_EPS_ATTACHED)
  {
    ds_pdn_cp_data_resume_inst_up_for_force_attach();    
  }
  return;
 
}

/*===========================================================================
FUNCTION DS_PDN_MGR_DS_TO_NAS_SEND_MSG
 
DESCRIPTION
  This function sends messages from PDN manager to NAS.

PARAMETERS 
  Message - void*
 
DEPENDENCIES 
  None. 
  
RETURN VALUE 
  boolean
 
SIDE EFFECTS 
  
===========================================================================*/
void ds_pdn_mgr_ds_to_nas_send_msg
(
  void*         msg_p
)
{
  gs_status_T  gs_status;

  gs_status = gs_send_message( GS_QUEUE_MM, msg_p, TRUE );

  if( gs_status != GS_SUCCESS )
  {
    DS_ERROR_LOG_1("Failed to send msg (status = =d)", gs_status);
  }
}

/*===========================================================================
FUNCTION DS_PDN_MGR_NAS_TO_DS_MSG_HDLR
 
DESCRIPTION
  This function handles messages from NAS to DS.

PARAMETERS 
  Message - void*
 
DEPENDENCIES 
  None. 
  
RETURN VALUE 
  boolean
 
SIDE EFFECTS 
  None
===========================================================================*/
void ds_pdn_mgr_nas_to_ds_msg_hdlr
(
  void*             msg_p
)
{
  IMH_T                                        msg_hdr;
  ds_mm_cmd_e_type                             cmd_type;
  ds_command_type                              *cmd_ptr = NULL;
  ds_eps_attach_sm_info_type                   *attach_info_p = NULL;
  emm_get_pdn_connectivity_request_ind_s_type  *pdn_conn_info_p = NULL;
  emm_pdn_connectivity_reject_ind_s_type       *pdn_conn_rej_p = NULL;
  emm_pdn_connectivity_failure_ind_s_type      *pdn_conn_fail_p = NULL;
  emm_act_default_bearer_context_request_ind_s_type 
                                                *act_bearer_info = NULL;
  emm_act_dedicated_bearer_context_request_ind_s_type 
                                                *act_ded_bearer_info = NULL;
  emm_deact_bearer_context_request_ind_s_type   *deact_bearer_info = NULL;
  emm_drb_setup_ind_s_type                      *drb_setup_info = NULL;
  emm_drb_released_ind_s_type                   *drb_release_info = NULL;
  emm_drb_reestablish_reject_ind_s_type         *drb_reject_info = NULL;
  eps_bearer_id_T                               *bearer_id = NULL;
  emm_attach_complete_ind_type                  *attach_complete_info = NULL;
  mm_pdp_activate_cnf_T                         *pdp_act_cnf = NULL;
  ds_bearer_deact_params_s                  *deact_param = NULL;
  mm_pdp_deactivate_cnf_T                       *pdp_deact_cnf = NULL;
  mm_pdp_activate_rej_T                         *pdp_deact_rej = NULL;
  mm_pdp_deactivate_ind_T                       *pdp_deact_ind = NULL;
  mm_pdp_activate_rej_T                         *pdp_act_rej = NULL;
  emm_data_transport_ind_type                   *emm_data_transport_ind = NULL;
  emm_data_transport_cnf_type                   *emm_data_transport_cnf = NULL;
  emm_tau_complete_ind_type                     *emm_tau_complete_ind = NULL;
  emm_ds_t3402_changed_ind_s_type               *emm_t3402_changed_ind = NULL;
  emm_bearer_ctxt_transfer_ind_type             *emm_cntxt_transfer_ind = NULL;
  emm_modify_bearer_context_request_ind_s_type  *emm_mod_bearer_ind = NULL;
  mm_restore_psm_ctxt_req_T                     *restore_psm_ctxt_req = NULL;
  emm_restore_bearer_ctxt_rsp_s_type            *restore_bearer_ctxt_res = NULL;
  mm_restore_pdp_ctxt_rsp_s_type                *restore_pdp_cntxt_res = NULL;
#ifdef FEATURE_NBIOT_NTN
  mm_ntn_ds_context_ind_T                     *store_ntn_cntxt_ind = NULL;
#endif /* FEATURE_NBIOT_NTN */
  emm_plmn_change_ind_type                      *plmn_change_indication = NULL;
  mm_wwan_tech_suspend_ind_s_type               *wwan_tech_suspend_ind = NULL;
  emm_timer_t3448_status_ind_type               *t3448_change_indication = NULL;
  emm_process_pending_mo_exception_data_s_type  *pending_nmo_ex_data_indication = NULL;
  emm_timer_barring_status_ind_type             *barring_status_indication=NULL;
  emm_cpsr_status_ind_type                      *cpsr_status_ind = NULL;
  
  if(msg_p == NULL)
  {
    DS_ERROR_LOG_0("NULL message from NAS");
    return;
  }

  memscpy(&msg_hdr, sizeof(IMH_T), msg_p, sizeof(IMH_T));
  cmd_type = msg_hdr.message_id;

  switch(cmd_type)
  {
    case EMM_GET_PDN_CONNECTIVITY_REQUEST_IND:
    {      
      /* Get DS command buffer */
      cmd_ptr = ds_allocate_cmd_buf(
                          sizeof(ds_eps_attach_sm_info_type));
      if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
      {
        goto func_exit;
      }

      attach_info_p = (ds_eps_attach_sm_info_type*)cmd_ptr->cmd_payload_ptr;

      pdn_conn_info_p = (emm_get_pdn_connectivity_request_ind_s_type*)msg_p;

      attach_info_p->seq_num = pdn_conn_info_p->seq_num;
      attach_info_p->responding_to_page = pdn_conn_info_p->responding_to_page;
      attach_info_p->attach_wo_pdn_supported = 
                                pdn_conn_info_p->attach_without_pdn_supported; 

      cmd_ptr->hdr.cmd_id = DS_CMD_PDN_GET_PDN_CONN_REQ_IND;
      ds_put_cmd(cmd_ptr);
      break;
    }

    case EMM_ACT_DEFAULT_BEARER_CONTEXT_REQUEST_IND:
    {      
      /* Get DS command buffer */
      cmd_ptr = ds_allocate_cmd_buf(
                     sizeof(emm_act_default_bearer_context_request_ind_s_type));
      if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
      {
        goto func_exit;
      }

      act_bearer_info = (emm_act_default_bearer_context_request_ind_s_type*)
                                                       cmd_ptr->cmd_payload_ptr;
      *act_bearer_info = 
                     *(emm_act_default_bearer_context_request_ind_s_type*)msg_p;

      cmd_ptr->hdr.cmd_id = DS_CMD_PDN_ACT_DEFAULT_BEARER_REQ;
      ds_put_cmd(cmd_ptr);
      break;
    }

    case EMM_ACT_DEDICATED_BEARER_CONTEXT_REQUEST_IND:
    {      
      /* Get DS command buffer */
      cmd_ptr = ds_allocate_cmd_buf(
                   sizeof(emm_act_dedicated_bearer_context_request_ind_s_type));
      if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
      {
        goto func_exit;
      }

      act_ded_bearer_info = 
                         (emm_act_dedicated_bearer_context_request_ind_s_type*)
                                                       cmd_ptr->cmd_payload_ptr;
      *act_ded_bearer_info = 
                   *(emm_act_dedicated_bearer_context_request_ind_s_type*)msg_p;

      cmd_ptr->hdr.cmd_id = DS_CMD_PDN_ACT_DEDICATED_BEARER_REQ;
      ds_put_cmd(cmd_ptr);
      break;
    }

    case EMM_DEACT_BEARER_CONTEXT_REQUEST_IND:
    {      
      /* Get DS command buffer */
      cmd_ptr = ds_allocate_cmd_buf(
                     sizeof(emm_deact_bearer_context_request_ind_s_type));
      if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
      {
        goto func_exit;
      }

      deact_bearer_info = (emm_deact_bearer_context_request_ind_s_type*)
                                                       cmd_ptr->cmd_payload_ptr;
      *deact_bearer_info = *(emm_deact_bearer_context_request_ind_s_type*)msg_p;

      cmd_ptr->hdr.cmd_id = DS_CMD_PDN_DEACT_BEARER_REQ;
      ds_put_cmd(cmd_ptr);
      break;
    }

    case EMM_PDN_CONNECTIVITY_REJECT_IND:
    {      
      /* Get DS command buffer */
      cmd_ptr = ds_allocate_cmd_buf(
                     sizeof(emm_pdn_connectivity_reject_ind_s_type));
      if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
      {
        goto func_exit;
      }

      pdn_conn_rej_p = (emm_pdn_connectivity_reject_ind_s_type*)
                                                       cmd_ptr->cmd_payload_ptr;
      *pdn_conn_rej_p = *(emm_pdn_connectivity_reject_ind_s_type*)msg_p;

      cmd_ptr->hdr.cmd_id = DS_CMD_PDN_CONN_REJ_IND;
      ds_put_cmd(cmd_ptr);
      break;
    }

    case EMM_PDN_CONNECTIVITY_FAILURE_IND:
    {      
      /* Get DS command buffer */
      cmd_ptr = ds_allocate_cmd_buf(
                     sizeof(emm_pdn_connectivity_failure_ind_s_type));
      if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
      {
        goto func_exit;
      }

      pdn_conn_fail_p = (emm_pdn_connectivity_failure_ind_s_type*)
                                                       cmd_ptr->cmd_payload_ptr;
      *pdn_conn_fail_p = *(emm_pdn_connectivity_failure_ind_s_type*)msg_p;

      cmd_ptr->hdr.cmd_id = DS_CMD_PDN_CONN_FAIL_IND;
      ds_put_cmd(cmd_ptr);
      break;
    }
        
    case EMM_DRB_SETUP_IND:
    {      
      /* Get DS command buffer */
      cmd_ptr = ds_allocate_cmd_buf(sizeof(emm_drb_setup_ind_s_type));
      if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
      {
        goto func_exit;
      }

      drb_setup_info = (emm_drb_setup_ind_s_type*)cmd_ptr->cmd_payload_ptr;
      *drb_setup_info = *(emm_drb_setup_ind_s_type*)msg_p;

      cmd_ptr->hdr.cmd_id = DS_CMD_PDN_RAB_REEST_IND;
      ds_put_cmd(cmd_ptr);
      break;
    }

    case EMM_ACT_DRB_RELEASED_IND:
    {      
      /* Get DS command buffer */
      cmd_ptr = ds_allocate_cmd_buf(sizeof(eps_bearer_id_T));
      if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
      {
        goto func_exit;
      }

      bearer_id = (eps_bearer_id_T*)cmd_ptr->cmd_payload_ptr;
      drb_release_info = (emm_drb_released_ind_s_type*)msg_p;
      
      *bearer_id = drb_release_info->eps_bearer_id;
      cmd_ptr->hdr.cmd_id = DS_CMD_PDN_RAB_RELEASE_IND;
      ds_put_cmd(cmd_ptr);
      break;
    }

    case EMM_DRB_REESTABLISH_REJECT_IND:
    {      
      /* Get DS command buffer */
      cmd_ptr = ds_allocate_cmd_buf(sizeof(emm_drb_reestablish_reject_ind_s_type));
      if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
      {
        goto func_exit;
      }
    
      drb_reject_info = (emm_drb_reestablish_reject_ind_s_type*)cmd_ptr->cmd_payload_ptr;
      *drb_reject_info = *(emm_drb_reestablish_reject_ind_s_type*)msg_p;

      cmd_ptr->hdr.cmd_id = DS_CMD_PDN_RAB_REJECT_IND;
      ds_put_cmd(cmd_ptr);
      break;
    }   	

    case EMM_ATTACH_COMPLETE_IND:
    {      
      /* Get DS command buffer */
      cmd_ptr = ds_allocate_cmd_buf(sizeof(emm_attach_complete_ind_type));
      if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
      {
        goto func_exit;
      }

      attach_complete_info = (emm_attach_complete_ind_type*)
                                                      cmd_ptr->cmd_payload_ptr;
      *attach_complete_info = *(emm_attach_complete_ind_type*)msg_p;

      cmd_ptr->hdr.cmd_id = DS_CMD_PDN_ATTACH_COMPLETE_IND;
      ds_put_cmd(cmd_ptr);
      break;
    }

    case EMM_MODIFY_BEARER_CONTEXT_REQUEST_IND:
    {      
      /* Get DS command buffer */
      cmd_ptr = ds_allocate_cmd_buf(sizeof(
                               emm_modify_bearer_context_request_ind_s_type));
      if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
      {
        goto func_exit;
      }

      emm_mod_bearer_ind = (emm_modify_bearer_context_request_ind_s_type*)
                                                       cmd_ptr->cmd_payload_ptr;
      *emm_mod_bearer_ind = 
                         *(emm_modify_bearer_context_request_ind_s_type*)msg_p;

      cmd_ptr->hdr.cmd_id = DS_CMD_PDN_MODIFY_BEARER_REQUEST_IND;
      ds_put_cmd(cmd_ptr);
      break;
    }
    
    case EMM_DATA_TRANSPORT_IND:
    {      
      /* Get DS command buffer */
      cmd_ptr = ds_allocate_cmd_buf(sizeof(emm_data_transport_ind_type));
      if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
      {
        goto func_exit;
      }

      emm_data_transport_ind = (emm_data_transport_ind_type*)cmd_ptr->cmd_payload_ptr;
      *emm_data_transport_ind = *(emm_data_transport_ind_type*)msg_p;

      cmd_ptr->hdr.cmd_id = DS_CMD_PDN_EMM_DATA_TRANSPORT_IND;
      ds_put_cmd(cmd_ptr);
      break;
    }

    
    case EMM_DATA_TRANSPORT_CNF:
    {      
      /* Get DS command buffer */
      cmd_ptr = ds_allocate_cmd_buf(sizeof(emm_data_transport_cnf_type));
      if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
      {
        goto func_exit;
      }

      emm_data_transport_cnf = (emm_data_transport_cnf_type*)cmd_ptr->cmd_payload_ptr;
      *emm_data_transport_cnf = *(emm_data_transport_cnf_type*)msg_p;

      cmd_ptr->hdr.cmd_id = DS_CMD_PDN_EMM_DATA_TRANSPORT_CNF;
      ds_put_cmd(cmd_ptr);
      break;
    }

    case EMM_TAU_COMPLETE_IND:
    {
      cmd_ptr = ds_allocate_cmd_buf(sizeof(emm_tau_complete_ind_type));
      if( cmd_ptr == NULL || (cmd_ptr->cmd_payload_ptr == NULL) )
      {
        goto func_exit;
      }

      emm_tau_complete_ind = (emm_tau_complete_ind_type*)cmd_ptr->cmd_payload_ptr;
      *emm_tau_complete_ind = *(emm_tau_complete_ind_type*)msg_p;
      
      cmd_ptr->hdr.cmd_id = DS_CMD_PDN_TAU_COMPLETE_IND;
      ds_put_cmd(cmd_ptr);
      break;
    }

    case EMM_T3402_CHANGED_IND:
    {
      cmd_ptr = ds_allocate_cmd_buf(sizeof(emm_ds_t3402_changed_ind_s_type));
      if( cmd_ptr == NULL || (cmd_ptr->cmd_payload_ptr == NULL) )
      {
        goto func_exit;
      }

      emm_t3402_changed_ind = (emm_ds_t3402_changed_ind_s_type*)
                                               cmd_ptr->cmd_payload_ptr;
      *emm_t3402_changed_ind = *(emm_ds_t3402_changed_ind_s_type*)msg_p;
      
      cmd_ptr->hdr.cmd_id = DS_CMD_PDN_T3402_CHANGED_IND;
      ds_put_cmd(cmd_ptr);
      break;
    }

    case EMM_BEARER_CTXT_TRANSFER_IND:
    {
      cmd_ptr = ds_allocate_cmd_buf(sizeof(emm_bearer_ctxt_transfer_ind_type));
      if( cmd_ptr == NULL || (cmd_ptr->cmd_payload_ptr == NULL) )
      {
        goto func_exit;
      }

      emm_cntxt_transfer_ind = (emm_bearer_ctxt_transfer_ind_type*)
                                               cmd_ptr->cmd_payload_ptr;
      *emm_cntxt_transfer_ind = *(emm_bearer_ctxt_transfer_ind_type*)msg_p;
      
      cmd_ptr->hdr.cmd_id = DS_CMD_CONTEXT_TRANSFER_IND;
      ds_put_cmd(cmd_ptr);
      break;
    }
    
    case MM_RESTORE_CTXT_PSM_REQ:
    {

    if (ds_pdn_psm_get_ulog_handle() != NULL)
    {
       ULOG_RT_PRINTF_0(ds_pdn_psm_get_ulog_handle(),
            "Restore_cntxt_psm_req");
    }

		/* Get DS command buffer */
      cmd_ptr = ds_allocate_cmd_buf(sizeof(mm_restore_psm_ctxt_req_T));
      if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
      {
        goto func_exit;
      }

       restore_psm_ctxt_req = (mm_restore_psm_ctxt_req_T*)cmd_ptr->cmd_payload_ptr;
      *restore_psm_ctxt_req = *(mm_restore_psm_ctxt_req_T*)msg_p;

      cmd_ptr->hdr.cmd_id = DS_CMD_PDN_PSM_RESTORE_CNTX_REQ;
      ds_put_cmd(cmd_ptr);
      break;
  }

    case EMM_RESTORE_BEARER_CTXT_RSP:
    {
      /* Get DS command buffer */
      cmd_ptr = ds_allocate_cmd_buf(
                     sizeof(emm_restore_bearer_ctxt_rsp_s_type));
      if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
      {
        goto func_exit;
      }
      restore_bearer_ctxt_res = (emm_restore_bearer_ctxt_rsp_s_type*)
                                                       cmd_ptr->cmd_payload_ptr;
      *restore_bearer_ctxt_res = 
                     *(emm_restore_bearer_ctxt_rsp_s_type*)msg_p;

      cmd_ptr->hdr.cmd_id = DS_CMD_PDN_RESTORE_BEARER_CNTX_RES;
      ds_put_cmd(cmd_ptr);
      break; 
    }

    case MM_RESTORE_PDP_CTXT_RSP:
    {
      /* Get DS command buffer */
      cmd_ptr = ds_allocate_cmd_buf(
                     sizeof(mm_restore_pdp_ctxt_rsp_s_type));
      if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
      {
        goto func_exit;
      }
      restore_pdp_cntxt_res = (mm_restore_pdp_ctxt_rsp_s_type*)
                                                       cmd_ptr->cmd_payload_ptr;
      *restore_pdp_cntxt_res = 
                     *(mm_restore_pdp_ctxt_rsp_s_type*)msg_p;

      cmd_ptr->hdr.cmd_id = DS_CMD_PDN_RESTORE_PDP_CNTX_RES;
      ds_put_cmd(cmd_ptr);
      break; 
    }

#ifdef FEATURE_NBIOT_NTN
    case MM_NTN_DS_CTXT_IND:
    {
      /* Get DS command buffer */
      cmd_ptr = ds_allocate_cmd_buf(
                     sizeof(mm_ntn_ds_context_ind_T));
      if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
      {
        goto func_exit;
      }
      store_ntn_cntxt_ind = (mm_ntn_ds_context_ind_T*)
                                                       cmd_ptr->cmd_payload_ptr;
      *store_ntn_cntxt_ind = 
                    *(mm_ntn_ds_context_ind_T*)msg_p;
      cmd_ptr->hdr.cmd_id = DS_CMD_PDN_MM_STORE_NTN_IND;
      ds_put_cmd(cmd_ptr);
      break; 
    }
#endif /* FEATURE_NBIOT_NTN */

    case EMM_DETACH_IND:
    {
      cmd_ptr = ds_allocate_cmd_buf(DS_CMD_PAYLOAD_SIZE_ZERO);
      if( cmd_ptr == NULL )
      {
        goto func_exit;
      }

      cmd_ptr->hdr.cmd_id = DS_CMD_PDN_EMM_DETACH_IND;
      ds_put_cmd(cmd_ptr);
      break;
    }
 
    case MM_PDP_ACTIVATE_CNF:
    {
      cmd_ptr = ds_allocate_cmd_buf(sizeof(mm_pdp_activate_cnf_T));
      if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
      {
        goto func_exit;
      }
 
      pdp_act_cnf = (mm_pdp_activate_cnf_T*)cmd_ptr->cmd_payload_ptr;
      *pdp_act_cnf = *(mm_pdp_activate_cnf_T*)msg_p;
 
      cmd_ptr->hdr.cmd_id = DS_CMD_PDP_ACTIVATE_CNF;
      ds_put_cmd(cmd_ptr);
      break;
    }

    case MM_PDP_DEACTIVATE_CNF:
    {
      cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_bearer_deact_params_s));
      if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
      {
        goto func_exit;
      }

      deact_param = (ds_bearer_deact_params_s*)cmd_ptr->cmd_payload_ptr;
      pdp_deact_cnf = (mm_pdp_deactivate_cnf_T*)msg_p;

      deact_param->connection_id = pdp_deact_cnf->connection_id;

      /* Set this to internal cause as call tear down is initiated by DS/app */
      deact_param->cause_type = SM_INTERNAL_CAUSE;
 
      cmd_ptr->hdr.cmd_id = DS_CMD_PDP_DEACTIVATE_IND;
      ds_put_cmd(cmd_ptr);
      break;
    }

    case MM_PDP_DEACTIVATE_REJ:
    {
      cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_bearer_deact_params_s));
      if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
      {
        goto func_exit;
      }
    
      deact_param = (ds_bearer_deact_params_s*)cmd_ptr->cmd_payload_ptr;
      pdp_deact_rej = (mm_pdp_activate_rej_T*)msg_p;

      deact_param->connection_id = pdp_deact_rej->connection_id;
      deact_param->cause_type = pdp_deact_rej->cause_type;
      deact_param->cause = pdp_deact_rej->cause;

      cmd_ptr->hdr.cmd_id = DS_CMD_PDP_DEACTIVATE_IND;
       ds_put_cmd(cmd_ptr);
       break;
     }

    case MM_PDP_DEACTIVATE_IND:
    {
      cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_bearer_deact_params_s));
      if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
      {
        goto func_exit;
      }
 
      deact_param = (ds_bearer_deact_params_s*)cmd_ptr->cmd_payload_ptr;
      pdp_deact_ind = (mm_pdp_deactivate_ind_T*)msg_p;

      deact_param->connection_id = pdp_deact_ind->connection_id;
      deact_param->cause_type = pdp_deact_ind->cause_type;
      deact_param->cause = pdp_deact_ind->cause;
      deact_param->backoff_timer = pdp_deact_ind->backoff_timer;
 
      cmd_ptr->hdr.cmd_id = DS_CMD_PDP_DEACTIVATE_IND;
      ds_put_cmd(cmd_ptr);
      break;
    }

    case MM_PDP_ACTIVATE_REJ:
    {
      cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_bearer_deact_params_s));
      if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
      {
        goto func_exit;
      }
 
      deact_param = (ds_bearer_deact_params_s*)cmd_ptr->cmd_payload_ptr;      
      pdp_act_rej = (mm_pdp_activate_rej_T*)msg_p;
      
      deact_param->connection_id = pdp_act_rej->connection_id;
      deact_param->cause_type = pdp_act_rej->cause_type;
      deact_param->cause = pdp_act_rej->cause;
      deact_param->backoff_timer = pdp_act_rej->backoff_timer;
      deact_param->pdp_reject_ind = pdp_act_rej->pdp_reject_param;
      deact_param->reattempt_indicator = pdp_act_rej->reattempt_indicator;      
      
      cmd_ptr->hdr.cmd_id = DS_CMD_PDP_DEACTIVATE_IND;
      ds_put_cmd(cmd_ptr);
      break;
    }

    case MM_UNBLOCK_ALL_APN_IND:
    {
      cmd_ptr = ds_allocate_cmd_buf(DS_CMD_PAYLOAD_SIZE_ZERO);
      if(cmd_ptr == NULL)
      {
        goto func_exit;
      }
  
      cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_PDN_THROT_UNBLOCK_ALL_APN;
      ds_put_cmd(cmd_ptr);
      break;
    }

    case EMM_PLMN_CHANGE_IND:
    {
      /* Get DS command buffer */
      cmd_ptr = ds_allocate_cmd_buf(sizeof(emm_plmn_change_ind_type));
      if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
      {
        goto func_exit;
      }
      plmn_change_indication = (emm_plmn_change_ind_type*)
                  cmd_ptr->cmd_payload_ptr;
      *plmn_change_indication = 
                 *(emm_plmn_change_ind_type*)msg_p;

       cmd_ptr->hdr.cmd_id = DS_CMD_PLMN_CHANGE_IND;
       ds_put_cmd(cmd_ptr);
       break; 
     }

    case MM_WWAN_TECH_STATUS_IND:
    {
      cmd_ptr = ds_allocate_cmd_buf(sizeof(mm_wwan_tech_suspend_ind_s_type));
      if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
      {
        goto func_exit;
      }
 
      wwan_tech_suspend_ind = 
                     (mm_wwan_tech_suspend_ind_s_type*)cmd_ptr->cmd_payload_ptr;      
      *wwan_tech_suspend_ind = *(mm_wwan_tech_suspend_ind_s_type*)msg_p;
            
      cmd_ptr->hdr.cmd_id = DS_CMD_PDN_WWAN_TECH_SUSPEND_IND;
      ds_put_cmd(cmd_ptr);
      break;
    }
    
    case EMM_TIMER_T3448_STATUS_IND:
    {
      /* Get DS command buffer */
      cmd_ptr = ds_allocate_cmd_buf(
			   sizeof(emm_timer_t3448_status_ind_type));
      if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
      {
        goto func_exit;
      }
       t3448_change_indication = (emm_timer_t3448_status_ind_type*)
									 cmd_ptr->cmd_payload_ptr;
      *t3448_change_indication = 
				   *(emm_timer_t3448_status_ind_type*)msg_p;
		
       cmd_ptr->hdr.cmd_id = DS_CMD_PDN_T3448_STATUS_IND;
       ds_put_cmd(cmd_ptr);
       break;
     }
    
     case EMM_PROCESS_PENDING_MO_EXCEPTION_DATA_IND:
     {
       /* Get DS command buffer */
       cmd_ptr = ds_allocate_cmd_buf(
                               sizeof(emm_process_pending_mo_exception_data_s_type));
       if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
       {
         goto func_exit;
       }
       pending_nmo_ex_data_indication = (emm_process_pending_mo_exception_data_s_type*)
                                         cmd_ptr->cmd_payload_ptr;
       *pending_nmo_ex_data_indication = 
                                 *(emm_process_pending_mo_exception_data_s_type*)msg_p;
			
       cmd_ptr->hdr.cmd_id = DS_CMD_PDN_PENDING_MO_EX_DATA_IND;
       ds_put_cmd(cmd_ptr);
       break;
     }

     case EMM_TIMER_BARRING_STATUS_IND:
     {
      /* Get DS command buffer */
      cmd_ptr = ds_allocate_cmd_buf(
             sizeof(emm_timer_barring_status_ind_type));
      if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
      {
        goto func_exit;
      }
       barring_status_indication = (emm_timer_barring_status_ind_type*)
                                  cmd_ptr->cmd_payload_ptr;
      *barring_status_indication = 
                         *(emm_timer_barring_status_ind_type*)msg_p;
       cmd_ptr->hdr.cmd_id = DS_CMD_PDN_BARRING_STATUS_IND;
       ds_put_cmd(cmd_ptr);
       break;
     }

    case EMM_CPSR_STATUS_IND:
    {
      /* Get DS command buffer */
      cmd_ptr = ds_allocate_cmd_buf(sizeof(emm_cpsr_status_ind_type));
      if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
      {
        goto func_exit;
      }
      
      cpsr_status_ind = (emm_cpsr_status_ind_type*)cmd_ptr->cmd_payload_ptr;
      *cpsr_status_ind = *(emm_cpsr_status_ind_type*)msg_p;
  
      cmd_ptr->hdr.cmd_id = DS_CMD_PDN_CPSR_STATUS_IND;
      ds_put_cmd(cmd_ptr);
      break;
    }

    default :
      break;
  }

  return;

func_exit:

  DS_ASSERT(0);
  return;  
}

/*===========================================================================
FUNCTION DS_PDN_MGR_PROCESS_CMDS
 
DESCRIPTION
  This function processes the commands posted to PDN manager.

PARAMETERS 
  Message - void*
 
DEPENDENCIES 
  None. 
  
RETURN VALUE 
  boolean
 
SIDE EFFECTS 
  None
===========================================================================*/
void ds_pdn_mgr_process_cmds
(
  ds_command_type  *cmd_ptr
)
{
#ifdef FEATURE_MODE_TLB
  ds_eps_tlb_per_subs_info_type *eps_tlb_hndl = NULL;
  boolean                        tlb_i_mode = FALSE;
#endif /* FEATURE_MODE_TLB */
  ds_pdn_mgr_instance_cmd_type *cmd_info = NULL;

  if(!cmd_ptr)
  {
    DS_ASSERT(0);
    return;
  }

  switch(cmd_ptr->hdr.cmd_id)
  {
    case DS_CMD_PDN_INSTANCE_UP:  
      cmd_info = (ds_pdn_mgr_instance_cmd_type*)cmd_ptr->cmd_payload_ptr;
      ds_pdn_mgr_instance_up_hdlr(cmd_info->pdn_mgr_p, cmd_info->inst_type,
                                  cmd_info->info_ptr, FALSE);
      break;

    case DS_CMD_PDN_GET_PDN_CONN_REQ_IND:
      ds_eps_get_pdn_conn_ind_hdlr((ds_eps_attach_sm_info_type*)
                                                     cmd_ptr->cmd_payload_ptr);
      break;

    case DS_CMD_PDN_ACT_DEFAULT_BEARER_REQ:
      (void)ds_eps_pdn_mgr_default_bearer_activate_ind(
                    (emm_act_default_bearer_context_request_ind_s_type*)
                                                     cmd_ptr->cmd_payload_ptr);
      break;

    case DS_CMD_PDN_ACT_DEDICATED_BEARER_REQ:
      ds_eps_pdn_mgr_dedicated_bearer_activate_ind(
                    (emm_act_dedicated_bearer_context_request_ind_s_type*)
                                                     cmd_ptr->cmd_payload_ptr);
      break;

    case DS_CMD_PDN_DEACT_BEARER_REQ:
      ds_eps_pdn_mgr_bearer_deactivate_ind_hdlr(
                          (emm_deact_bearer_context_request_ind_s_type*)
                                                     cmd_ptr->cmd_payload_ptr);
      break;

    case DS_CMD_PDN_CONN_REJ_IND:
    	 ds_eps_pdn_mgr_pdn_conn_rej_ind_hdlr(
                          (emm_pdn_connectivity_reject_ind_s_type*)
                                                     cmd_ptr->cmd_payload_ptr);
      break;

    case DS_CMD_PDN_CONN_FAIL_IND:
    	 ds_eps_pdn_mgr_pdn_conn_fail_ind_hdlr(
                          (emm_pdn_connectivity_failure_ind_s_type*)
                                                     cmd_ptr->cmd_payload_ptr);
      break;

    case DS_CMD_PDN_RAB_REEST_IND:
      ds_bearer_mgr_rab_re_estab_ind_hdlr(
                          (emm_drb_setup_ind_s_type*)cmd_ptr->cmd_payload_ptr);
      break;

    case DS_CMD_PDN_DORM_REORIG:
    	 ds_bearer_mgr_proc_reorig_dorm_cmd((ds_bearer_mgr_s*)
 	 	                           *((ds_bearer_mgr_s**)cmd_ptr->cmd_payload_ptr));
    	 break;

    case DS_CMD_PDN_DORM_REORIG_TMR_EXP:
    	 ds_bearer_mgr_proc_reorig_dorm_tmr_exp_cmd();
    	 break;

    case DS_CMD_PDN_RAB_RELEASE_IND:
      ds_bearer_mgr_rab_release_ind_hdlr(
                          *((uint8*)cmd_ptr->cmd_payload_ptr));
      break;

    case DS_CMD_PDN_RAB_REJECT_IND:
    	 ds_bearer_mgr_rab_re_estab_rej_hdlr(
    	 	      (emm_drb_reestablish_reject_ind_s_type*)cmd_ptr->cmd_payload_ptr);
    	 break;

    case DS_CMD_PDN_ATTACH_COMPLETE_IND:
      ds_eps_pdn_mgr_attach_complete_ind_hdlr(
                      (emm_attach_complete_ind_type*)cmd_ptr->cmd_payload_ptr);
      break;

    case DS_CMD_PDN_EPS_BEARER_GO_NULL:  
      ds_eps_bearer_mgr_go_null_hdlr(*((uint8*)cmd_ptr->cmd_payload_ptr));
      break;

    case DS_CMD_PDN_MODIFY_BEARER_REQUEST_IND:
      ds_pdn_flow_manager_bearer_modification_ind(
              (emm_modify_bearer_context_request_ind_s_type*)
                                                    cmd_ptr->cmd_payload_ptr);
      break;

    case DS_CMD_PDN_FLOW_FLTR_ID_CONFLICT:
      ds_pdn_flow_manager_process_filter_id_conflict(
               (ds_pdn_flow_manager_cmd_info_type*)cmd_ptr->cmd_payload_ptr);
      break;

    case DS_CMD_PDN_FLOW_FLTR_PRECED_CONFLICT:
      ds_pdn_flow_manager_process_filter_preced_conflict(
               (ds_pdn_flow_manager_cmd_info_type*)cmd_ptr->cmd_payload_ptr);
      break;

    case DS_CMD_PDN_EMM_DATA_TRANSPORT_IND:
#ifdef FEATURE_MODE_TLB
  
      eps_tlb_hndl = ds_eps_tlb_get_info_hndl();
  
      if( (ds_eps_tlb_validate_info_hndl(eps_tlb_hndl)) &&
          (eps_tlb_hndl->tlb_state.mode == LTE_TLB_MODE_I) )
      {
        tlb_i_mode = TRUE;
      }

      if(tlb_i_mode)
      {
        ds_eps_tlb_process_lte_data_transport_ind_mode_G_I(
		       (emm_data_transport_ind_type*)cmd_ptr->cmd_payload_ptr);
      }
      else
#endif /* FEATURE_MODE_TLB */
      {
        ds_pdn_cp_data_process_lte_data_transport_ind(
                 (emm_data_transport_ind_type*)cmd_ptr->cmd_payload_ptr);
      }
      break;

    case DS_CMD_PDN_EMM_DATA_TRANSPORT_CNF:
#ifdef FEATURE_MODE_TLB
      eps_tlb_hndl = ds_eps_tlb_get_info_hndl();
  
      if( (ds_eps_tlb_validate_info_hndl(eps_tlb_hndl)) &&
          (eps_tlb_hndl->tlb_state.mode == LTE_TLB_MODE_I) )
      {
        tlb_i_mode = TRUE;
      }

      if(tlb_i_mode)
      {
        ds_eps_tlb_process_lte_data_transport_cnf_mode_I(
			 (emm_data_transport_cnf_type*)cmd_ptr->cmd_payload_ptr);
      }
      else
#endif /* FEATURE_MODE_TLB */
      {
        ds_pdn_cp_data_process_lte_data_transport_cnf(
                        (emm_data_transport_cnf_type*)cmd_ptr->cmd_payload_ptr);
      }
      break;

    case DS_CMD_PDN_TAU_COMPLETE_IND:
      ds_pdn_cp_data_tau_complete_ind_hdlr(
                        (emm_tau_complete_ind_type*)cmd_ptr->cmd_payload_ptr);
      break;
      
    case DS_CMD_PDN_T3402_CHANGED_IND:
      ds_eps_pdn_set_t3402_timer_val(
                  (emm_ds_t3402_changed_ind_s_type*)cmd_ptr->cmd_payload_ptr);
      break;

    case DS_CMD_CONTEXT_TRANSFER_IND:
      ds_pdn_mgr_process_context_transfer_ind(
              (emm_bearer_ctxt_transfer_ind_type*)cmd_ptr->cmd_payload_ptr);
      break;
      
    case DS_CMD_PDN_EMM_DETACH_IND:
     ds_eps_attach_hdlr_detach_ind_hdlr();
     break;

    case DS_CMD_PDN_REMOVE_UNUSED_PDN:
      ds_pdn_mgr_remove_unused_pdn((ds_pdn_mgr_s*)
                                *((ds_pdn_mgr_s**)cmd_ptr->cmd_payload_ptr));
      break;
    case DS_CMD_PDN_CLAT_SM_CREATE:
      ds_pdn_post_clat_sm_create_cmd_hdlr((ds_pdn_mgr_s*)cmd_ptr->cmd_payload_ptr);
      break;

    case DS_CMD_PDN_RATE_CTRL_INTERVAL_TMR_EXP:
      ds_pdn_rate_ctrl_process_interval_timer_expiry(cmd_ptr->cmd_payload_ptr);
      break;

    case DS_CMD_PDN_CP_DATA_RETRY_TIMER_EXP:
      ds_pdn_cp_data_process_retry_timer_expiry(cmd_ptr->cmd_payload_ptr);
      break;

    case DS_CMD_PDN_CP_DATA_RAI_TIMER_EXP:
      ds_pdn_cp_data_process_rai_timer_expiry();
      break;

    case DS_CMD_PDN_CP_DATA_L2_NOTIFY:
      ds_pdn_cp_data_process_l2_notify_cmd(
                                *((uint8*)cmd_ptr->cmd_payload_ptr));
      break;

    case DS_CMD_PDN_PSM_READY:
      ds_pdn_psm_ready_cmd(cmd_ptr);
      break;

    case DS_CMD_PDN_PSM_ENTER:
     ds_pdn_psm_enter_cmd(cmd_ptr);
     break;

    case DS_CMD_PDN_PSM_RESTORE_CNTX_REQ:
      if (ds_pdn_psm_get_ulog_handle() != NULL)
      {
         ULOG_RT_PRINTF_0(ds_pdn_psm_get_ulog_handle(),
              "DS_CMD_PDN_PSM_RESTORE_CNTX_REQ processing");
      }
       ds_pdn_process_psm_restore_cntx_req(
         (mm_restore_psm_ctxt_req_T *)(cmd_ptr->cmd_payload_ptr));
      break;

    case DS_CMD_PDN_RESTORE_BEARER_CNTX_RES:
      ds_pdn_process_restore_bearer_ctxt_rsp_cmd(cmd_ptr);
      break;

    case DS_CMD_PDN_RESTORE_PDP_CNTX_RES:
      ds_pdn_process_restore_bearer_ctxt_rsp_cmd(cmd_ptr);
      break;

#ifdef FEATURE_NBIOT_NTN
    case DS_CMD_PDN_MM_STORE_NTN_IND:
      ds_pdn_process_store_reset_ntn_cntxt_cmd(
        (mm_ntn_ds_context_ind_T *)cmd_ptr->cmd_payload_ptr);
      break;
#endif /* FEATURE_NBIOT_NTN */

    case DS_CMD_PDN_WWAN_TECH_SUSPEND_IND:
      ds_pdn_mgr_process_tech_suspend_ind(
                 (mm_wwan_tech_suspend_ind_s_type*)cmd_ptr->cmd_payload_ptr);
      break;

    case DS_CMD_PDP_ACTIVATE_CNF:
    	 ds_gprs_mgr_pdp_activate_cnf_hdlr(
          	 	              (mm_pdp_activate_cnf_T*)cmd_ptr->cmd_payload_ptr);
    	 break;    	

    case DS_CMD_PDP_DEACTIVATE_IND:
    	 ds_gprs_mgr_pdp_deactivate_hdlr(
    	 	            (ds_bearer_deact_params_s*)cmd_ptr->cmd_payload_ptr);
    	 break;    	

    case DS_CMD_GSNDCP_REG_CNF:
    	 ds_gprs_bearer_mgr_pdp_ip_req_reg_cnf_hdlr(
                  (ds_gprs_gsndcp_reg_cnf_ev_type*)cmd_ptr->cmd_payload_ptr);
    	 break;

    case DS_CMD_GSNDCP_RESUME_REG_CNF:
    	 ds_gprs_bearer_mgr_resume_sndcp_reg_req_cnf_hdlr(
                  (ds_gprs_gsndcp_reg_cnf_ev_type*)cmd_ptr->cmd_payload_ptr);
    	 break;

    case DS_CMD_MSGR_RECV:
    	 dsmsgr_process_ds_cmd(cmd_ptr);
    	 break;

    case DS_CMD_MMGSDI_SIM_INFO:
    	 ds3g_mmgsdi_process_cmd(cmd_ptr);
    	 break;
	
    case DS_CMD_CM_SS_EV_SRV_CHG:
      dssysmgr_process_cm_ss_srv_chg_cmd(cmd_ptr);
      break;

    case DS_CMD_PDN_NW_CIOT_CAP_CHG:
    	 ds_pdn_cp_data_process_ciot_status_chg(
  	 	                       *((sys_neg_ciot_cap*)cmd_ptr->cmd_payload_ptr));
    	 break;
    
    case DS_CMD_CM_PH_EV_INFO:
      dssysmgr_process_cm_ph_event_cmd(cmd_ptr);
      break;

    case DS_CMD_CM_PWROFF_LPM_IND:
    	 ds_pdn_mgr_process_pwroff_lpm_cb();
    	 break;

    case DS_CMD_CM_SUB_NOT_AVAIL_IND:
    	 ds_pdn_mgr_process_sub_not_avail_cb();
    	 break;

    case DS_CMD_PDNMGR_CM_PH_EV:
       ds_pdn_mgr_ph_ev_cmd_hdlr(cmd_ptr);
       break;

    case DS_CMD_PDN_INACTIVITY_TMR_EXP:
      ds_pdn_mgr_process_pdn_inactivity_cmd(
                                 ((ds_pdn_mgr_s*)cmd_ptr->cmd_payload_ptr));
      break;

    case DS_CMD_PDN_POLLING_TMR_EXP:
      ds_pdn_mgr_process_polling_tmr_exp_cmd(
                                 ((ds_pdn_mgr_s*)cmd_ptr->cmd_payload_ptr));
      break;

    case DS_CMD_PDN_APN_PARAM_CHG_TMR_EXP:
      ds_pdn_mgr_process_apn_param_chg_tmr_exp_cmd(
                                ((uint8)cmd_ptr->cmd_payload_ptr));
      break;

    case DS_CMD_PDN_PROFILE_PARAM_CHG_CMD:
      ds_pdn_mgr_profile_change_cmd_hdlr(
          ((ds_pdn_mgr_profile_change_s*)cmd_ptr->cmd_payload_ptr));
      break;

    case DS_CMD_3GPP_PDN_THROT_UNBLOCK_APN_TIMER_EXP:
      ds_pdn_throt_spec_unblock_apn_timer(cmd_ptr->cmd_payload_ptr);
      break;

    case DS_CMD_3GPP_PDN_THROT_TIMESTAMP_APN_THROT_TMR:
      ds_pdn_throt_spec_timestamp_apn_throt_tmr_cmd_hdlr(
                                          cmd_ptr->cmd_payload_ptr);
      break;

    case DS_CMD_3GPP_PDN_THROT_SAMPLING_TIMER_EXP:
      ds_pdn_throt_spec_sampling_timer_exp_cmd_hdlr();
      break;

    case DS_CMD_3GPP_PDN_THROT_UNBLOCK_ALL_APN:
      ds_pdn_throt_spec_unblock_esm_backoff_throttling();
      break;

    case DS_CMD_3GPP_PDN_THROT_CLEAR_APN_REJ_T3402_TMR:
      ds_3gpp_dam_t3402_timer_expiry_cmd_hdlr();
      break;

    case DS_CMD_3GPP_PDN_LIMIT_PDN_CONN_TMR_EXP:
      ds_3gpp_pdn_limit_process_pdn_conn_tmr_expiry_cmd(
                                         cmd_ptr->cmd_payload_ptr);
      break;

    case DS_CMD_3GPP_PDN_LIMIT_PDN_REQ_THROT_TMR_EXP:
      ds_3gpp_pdn_limit_process_pdn_req_throt_tmr_expiry_cmd(
                                         cmd_ptr->cmd_payload_ptr);
      break;

    case DS_CMD_3GPP_PDN_LIMIT_PDN_WAIT_TMR_EXP:
      ds_3gpp_pdn_limit_process_pdn_wait_tmr_expiry_cmd(
                                         cmd_ptr->cmd_payload_ptr);
      break;

    case DS_CMD_3GPP_PDN_LIMIT_SAMPLING_TIMER_EXP:
      ds_3gpp_pdn_limit_process_sampling_timer_exp_cmd();
      break;

    case DS_CMD_3GPP_PDN_LIMIT_PDN_REQ_TMR_START:
      ds_3gpp_pdn_limit_process_pdn_req_tmr_start_cmd(
                                         cmd_ptr->cmd_payload_ptr);
      break;

    case DS_CMD_3GPP_THROT_TMR_EXP:
      ds_3gppi_throt_sm_timer_exp_cmd_hdlr(
                            (unsigned long)cmd_ptr->cmd_payload_ptr);
      break;

    case DS_CMD_PDN_TEARDOWN_INVALID_PCSCF:
      ds_pdn_mgr_teardown_instances_for_pdn(
	                   (ds_pdn_mgr_s*)cmd_ptr->cmd_payload_ptr,
                       DS_NET_DOWN_REASON_INVALID_PCSCF_ADDR);
      break;
      
    case DS_CMD_3GPP_EPS_THROT_TMR_EXP_CMD:
#if (defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS))
    case DS_CMD_3GPP_RAU_COMPLTE_CMD:
#endif /* FEATURE_DATA_WCDMA_PS || FEATURE_GSM_GPRS */
      ds_3gpp_pdn_throt_sm_process_cmds(cmd_ptr);
      break;
     
    case DS_CMD_PLMN_CHANGE_IND:
      ds_pdn_hdlr_plmn_change_hdlr((emm_plmn_change_ind_type*)cmd_ptr->cmd_payload_ptr);
         //ds_pdn_hdlr_set_current_plmn((emm_plmn_change_ind_type*)cmd_ptr->cmd_payload_ptr);
    break;
    
    case DS_CMD_3GPP_SYS_SEL_PREF_ERR:
      ds_3gpp_plmn_sys_sel_pref_err_cmd_hdlr();
      break;
    
    case DS_CMD_3GPP_SYS_SEL_PREF_NO_ERR:
      ds_3gpp_plmn_sys_sel_pref_no_err_cmd_hdlr();
#ifdef FEATURE_DATA_LTE
      ds_eps_pdn_cntxt_sys_sel_pref_no_err_cmd_hdlr
        (cmd_ptr->cmd_payload_ptr);
#endif /* FEATURE_DATA_LTE */
     break;
    
    case DS_CMD_3GPP_SYS_SEL_RETRY_TIMER_EXP:
      ds_3gpp_plmn_sys_sel_retry_timer_expiry_cmd_hdlr();
     break;

    case DS_CMD_PDN_T3448_STATUS_IND:
      ds_pdn_mgr_set_T3448_staus_ind((emm_timer_t3448_status_ind_type*)cmd_ptr->cmd_payload_ptr);
      break;

    case DS_CMD_PDN_BARRING_STATUS_IND:
      ds_pdn_mgr_set_barring_staus_ind((emm_timer_barring_status_ind_type*)cmd_ptr->cmd_payload_ptr);
      break;

    case DS_CMD_3GPP_RPM_SAMPLING_TIMER_EXP:
      ds_3gpp_rpm_sampling_tmr_exp_hdlr();
      break;

    case DS_CMD_3GPP_RPM_LR3_TIMER_EXP:
      ds_3gpp_rpm_lr3_tmr_exp_hdlr();
      break;

    case DS_CMD_3GPP_RPM_HOURLY_TIMER_EXP:
      ds_3gpp_rpm_hourly_tmr_exp_hdlr(cmd_ptr);
    	 break;
    
    case DS_CMD_3GPP_RPM_MMGSDI_READ_PARAM_FAIL:
      ds_3gpp_rpm_mgsdi_read_fail_cmd_hdlr(cmd_ptr);
      break;

    case DS_CMD_3GPP_RPM_MMGSDI_READ_PARAM_CNF:
      ds_3gpp_rpm_mmgsdi_read_resp_cmd_hdlr(cmd_ptr);
      break;

    case DS_CMD_PDN_PENDING_MO_EX_DATA_IND:
      ds_pdn_mgr_pending_mo_ex_data_ind_hdlr((emm_process_pending_mo_exception_data_s_type*)
	                                          (cmd_ptr->cmd_payload_ptr));
      break;
    case DS_CMD_PDN_DSM_MEM_CTRL_EV:
      ds_bearer_mgr_set_mem_ctrl_ev_posted_flag(FALSE);
      ds_bearer_mgr_process_mem_fc_event((ds_bearer_mgr_event_type  *)cmd_ptr->cmd_payload_ptr);
      break;

#ifdef FEATURE_MODE_TLB
    case DS_CMD_TLB_MODE_B_HANDLER:
      dsEpsTlb_modeB_hdlr_cmd(cmd_ptr);
      break;
#endif /* FEATURE_MODE_TLB */

    case DS_CMD_PDN_CPSR_STATUS_IND:
      ds_pdn_cp_data_process_cpsr_status_cmd((emm_cpsr_status_ind_type*)
                                             cmd_ptr->cmd_payload_ptr);
      break;

    default:
      break;
  }
}

/*===========================================================================
FUNCTION ds_pdn_mgr_handle_v6_inst_down

DESCRIPTION
  This function does additional handling when v6 instance goes down  

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS  
===========================================================================*/
void ds_pdn_mgr_handle_v6_inst_down
(
  ds_pdn_mgr_s             *pdn_mgr_p,
  ds_net_down_reason_type   down_reason
)
{
  ds_pdn_mgr_inst_type_s  *inst_p = NULL;
  ds_pdn_mgr_inst_type_s  *v4_inst_p = NULL;

  if(!ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p))
  {
    return;
  }

  DS_MSG1(MSG_LEGACY_HIGH, "Entering ds_pdn_mgr_handle_v6_inst_down for PDN %d",
          pdn_mgr_p->index);

  inst_p = pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV6];
  if(!inst_p)
  {
    return;
  }


  if(ds_pdn_get_clat_state(pdn_mgr_p) == DS_PDN_CLAT_UP || 
          ds_pdn_get_clat_state(pdn_mgr_p) == DS_PDN_CLAT_COMING_UP)
  {
    ds_pdn_teardown_clat_sm(pdn_mgr_p);
  }
  
  ds_pdn_mgr_cleanup_inst(pdn_mgr_p, inst_p->type);

  /*-------------------------------------------------------------
  Get the v4 instance. It is possible for this to be NULL
  --------------------------------------------------------------*/
  v4_inst_p = pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV4];

 /*----------------------------------------------------------------
   Check for Dual IP case. The v4 instance is set to down only after
   v6 instance is down. 
  -----------------------------------------------------------------*/
  if(v4_inst_p)
  {
    if(pdn_mgr_p->state == DS_PDN_MGR_STATE_GOING_DOWN)
    {
      /*---------------------------------------------------------------------------
      If it is already down, no need to post down twice. 
      ---------------------------------------------------------------------------*/
      if( v4_inst_p->state != DS_INSTANCE_STATE_DOWN)
      {
        ds_pdn_mgr_tear_down_instance(pdn_mgr_p, DS_PDN_INSTANCE_IPV4, down_reason);
      }
    }
  }
  
  /*--------------------------------------------------------------------
  Perform clean up of apn table and pdn table. We need to 
  check for pdn_mgr state here because:
  - it is possible that for a Single IP v6 call RA failure happens and 
  control reaches here
  - In such a case we should not tear down pdn
  - If we did so, then eventually when we get a bearer_down_ind,
  we will not have a valid pdn to work with.
  -------------------------------------------------------------------*/
  if(pdn_mgr_p->state == DS_PDN_MGR_STATE_GOING_DOWN) 
  {
    ds_pdn_mgr_cleanup_pdn(pdn_mgr_p, DS_PDN_INSTANCE_MAX);
  }
}

/*===========================================================================
FUNCTION DS_PDN_MGR_HANDLE_V6_EV_CBACK()

DESCRIPTION
  Called when the IP6 comes up/down.  This posts an appropriate event to the
  dssnet6 state machine.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_pdn_mgr_handle_v6_ev_cback
(
  ds_pdn_v6_sm_cb_type    *instance,
  ds_pdn_v6_sm_event_type  cur_event
)
{
  ds_pdn_mgr_inst_type_s     *inst_p = NULL;
  ds_pdn_mgr_s               *pdn_mgr_p = NULL;
  ds_pdn_apm_ind_pay_load     ind_payload = {0};  
#ifdef FEATURE_DATA_LTE
  ds_command_type            *cmd_ptr = NULL;
  boolean                     throttling_is_allowed = FALSE;
                            /* throttling_is_allowed flag ensures that we 
                               don't increment throttling counter for LTE
                                     Attach Failure scenarios */
  ds_pdn_mgr_s              **data_ptr = NULL;                                     
  boolean                     remove_unused_pdn = FALSE;
#endif /* FEATURE_DATA_LTE */  
  ds_bearer_mgr_s            *bearer_mgr_p = NULL;
  uint32                      rat_mask = DS_3GPP_THROT_RAT_MASK_GLOBAL;
                             /* RATs on which throttling needs to be unblocked*/
  ds_3gpp_throt_rat_e_type    rat_type = DS_3GPP_THROT_RAT_GLOBAL;
                             /* RAT on which throttling needs to be
                                performed */
  ds_3gpp_throttle_sm_cb_data_type  cb_data;
  ds_3gpp_pdn_throt_ident_info_type throt_ident_info;
  uint8                             psm_index =0;
  uint8                             prefix_len =0;
  uint64                            elapsed_time = 0;
  uint32                            prefix_valid_life_time = 0;
  uint32                            prefix_pref_life_time = 0;
  uint32                            router_life_time = 0;
  uint8                             bearer_id = 0;
  ds_ip_config_type                 ip_config;
  ds_pdn_event_info_u_type          event_info;
  ds_3gpp_pdn_throttle_sm_type      throt_sm_type_p = DS_3GPP_PDN_THROT_TYPE_NOT_SPECIFIED;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*------------------------------------------------------------------------- 
    Validate Input arguments
    -------------------------------------------------------------------------*/
  if ((instance == NULL))
  {
    DS_ERROR_LOG_0("NULL instance ptr");
    return;
  }

  pdn_mgr_p = instance->pdn_mgr_p;
  if(!ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p))
  {
    return;
  }

  inst_p = pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV6];
  if(!inst_p)
  {
    return;
  }
  
  DS_MSG3(MSG_LEGACY_HIGH,"Recd DS PDN V6 event %d, state %d, on PDN %d",
          cur_event, instance->state, pdn_mgr_p->index);


  ds_3gpp_throt_fill_cb_data (&cb_data, NULL, NULL, (void*)pdn_mgr_p);

  if (ds_pdn_nv_manager_get_enable_pdn_throt_per_rat())
  {
  
      rat_type = ds_3gpp_pdn_throt_sm_sys_mode_to_rat_type
                   (pdn_mgr_p->nw_mode);

      rat_mask |= ds_3gpp_pdn_throt_sm_sys_mode_to_rat_mask
                   (pdn_mgr_p->nw_mode);
  }

  ds_3gpp_pdn_throt_fill_ident_info_ex
  (
     &throt_ident_info, 
     (byte*)pdn_mgr_p->pdp_profile->context.apn,
     FALSE,
     rat_type,
     DS_3GPP_PDN_THROT_PROC_GLOBAL,
     DS_3GPP_PDN_THROT_TYPE_DEFAULT,
     DS_PROFILE_PDP_IPV6,
     FALSE,
     DS_NET_DOWN_REASON_IPV6_ERR_PREFIX_UNAVAILABLE,
     pdn_mgr_p,
     &cb_data
  );

  switch (cur_event)
  {
    case DS_PDN_V6_NET_DOWN_EV:
      switch(instance->state)
      {
        case DS_PDN_V6_WAITING_FOR_NET_UP_STATE:

          /*----------------------------------------------------------------- 
            We allow throttling in the following cases:
            == ===== ========== == === ========= =====
           1. Given PDN does not have is_default flag set.
           2. Given PDN has is_default flag set, but there is 
              another PDN which is already in UP State. 
           3. VZW Configuration is not enabled.
            -----------------------------------------------------------------*/
#ifdef FEATURE_DATA_LTE
        if (pdn_mgr_p->is_default == FALSE || 
             ds_pdn_nv_manager_get_regular_pdn_throttling_nv())
        {
          throttling_is_allowed = TRUE;
        }

        if (pdn_mgr_p->is_default == TRUE &&
            ds_pdn_mgr_is_any_other_pdn_up(pdn_mgr_p) == TRUE)
        {
          throttling_is_allowed = TRUE;
        }

        if (throttling_is_allowed
#ifdef FEATURE_DATA_CC_SP_THROTTLING
       && (ds_3gpp_throt_sm_is_cc_sp_throttling_enabled() != TRUE)
#endif /* FEATURE_DATA_CC_SP_THROTTLING */
)
        {
            /*--------------------------------------------------------------- 
           Get the ps_down_reason and use it to reset the cause code.
           Stateless autoconfig failed, call throttling control
              ---------------------------------------------------------------*/
            ds_3gpp_pdn_throt_perform_reg_throttling(&throt_ident_info);
        }
#endif /*FEATURE_DATA_LTE*/
        /*------------------------------------------------------------------- 
              update apn table to allow only v4 call
              ------------------------------------------------------------------*/
          ds_apn_table_update_allowed_ip_type(pdn_mgr_p, DS_IPV4_ONLY);
          ds_pdn_apm_set_invalid_ipv6_addr(&ind_payload,
                                DS_NET_DOWN_REASON_IPV6_ERR_PREFIX_UNAVAILABLE);  
          break;

        case DS_PDN_V6_NET_UP_STATE:
      
#ifdef FEATURE_DATA_LTE
          /*----------------------------------------------------------------- 
          Get the ps_down_reason and use it to reset the cause code.
          Stateless autoconfig failed, call throttling control
            -----------------------------------------------------------------*/
#ifdef FEATURE_DATA_CC_SP_THROTTLING
         if(ds_3gpp_throt_sm_is_cc_sp_throttling_enabled() != TRUE)
         {
#endif /* FEATURE_DATA_CC_SP_THROTTLING */
            ds_3gpp_pdn_throt_perform_reg_throttling(&throt_ident_info);
#ifdef FEATURE_DATA_CC_SP_THROTTLING
         }
#endif /* FEATURE_DATA_CC_SP_THROTTLING */
#endif /*FEATURE_DATA_LTE*/
        /*----------------------------------------------------------------- 
              Update apn table to allow only v4 call
              -----------------------------------------------------------------*/
          ds_apn_table_update_allowed_ip_type(pdn_mgr_p, DS_IPV4_ONLY);
          ds_pdn_apm_set_invalid_ipv6_addr(&ind_payload,
                                DS_NET_DOWN_REASON_IPV6_ERR_PREFIX_UNAVAILABLE);
          ds_pdn_mgr_bring_down_bearer(pdn_mgr_p, DS_PDN_INSTANCE_IPV6);
          break;

        case DS_PDN_V6_WAITING_FOR_NET_DOWN_STATE:
          /*----------------------------------------------------------------- 
            This is the case of client initiated call end (or) Call end due
            to LPM mode.
            -----------------------------------------------------------------*/
          if(lpm_in_progress == FALSE && sub_not_avail_in_progress == FALSE)
          {
            /*--------------------------------------------------------------- 
              This is a case of Client initiated tear down. Continue
              bringing down physlink 
              ---------------------------------------------------------------*/
            ds_pdn_mgr_bring_down_bearer(pdn_mgr_p, DS_PDN_INSTANCE_IPV6);
          }
            
          break;

        default:
          break;
      }/* End switch(cur_state)*/
      break;

    case DS_PDN_V6_NET_UP_EV:
      /*--------------------------------------------------------------------- 
        V6 is up, Clean Throttle State for v6
        ---------------------------------------------------------------------*/

      if((ds_pdn_nv_manager_is_iot_rpm_enabled() == FALSE) ||
         (ds_3gpp_pdn_throt_get_sm_type((byte*)pdn_mgr_p->pdp_profile->context.apn,
                                         FALSE,
                                         DS_3GPP_PDN_THROT_PROC_GLOBAL,
                                         DS_3GPP_THROT_RAT_GLOBAL,
                                         &throt_sm_type_p) == FALSE) ||
          (throt_sm_type_p != DS_3GPP_PDN_THROT_TYPE_RPM))
      {
        ds_3gpp_pdn_throt_clear_throttle_state(
                                  (byte*)pdn_mgr_p->pdp_profile->context.apn, 
                                             DS_PROFILE_PDP_IPV6,
                                             rat_mask,
                                             DS_3GPP_THROT_PROC_MASK_GLOBAL);
      }

      ds_pdn_apm_set_ipv6_addr(&ind_payload, TRUE);
      ds_pdn_psm_ready_ind();
      
      #ifdef FEATURE_DATA_LTE      
      remove_unused_pdn = ds_pdn_nv_manager_get_remove_unused_pdn();
      if (remove_unused_pdn)
      {
        cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_pdn_mgr_s*));
        if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
        {
          DS_ASSERT(0);
          return;
        }

        cmd_ptr->hdr.cmd_id = DS_CMD_PDN_REMOVE_UNUSED_PDN;
        data_ptr = (ds_pdn_mgr_s**)cmd_ptr->cmd_payload_ptr;
        *data_ptr = pdn_mgr_p;

        ds_put_cmd(cmd_ptr);
      }
      #endif /* FEATURE_DATA_LTE */
      break;
      
    case DS_PDN_V6_NET_SOL_WAIT_EV:
      if(pdn_mgr_p->is_restore_mode == TRUE)
      {
        switch(instance->state)
        {
          case DS_PDN_V6_WAITING_FOR_NET_UP_STATE: 
            bearer_mgr_p =
              (ds_bearer_mgr_s*)pdn_mgr_p->
                              bearer_ptr_tbl[DS_3GPP_DEFAULT_BEARER_INDEX];
            if (!ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p))
            {
              return;
            }
 
            if(!bearer_mgr_p->eps_bearer)
            {
              return; 
            }
            bearer_id = bearer_mgr_p->bearer_id;

            psm_index = ds_pdn_psm_info_find_index_by_bearer_id(bearer_id);
            if (psm_index != DS_3GPP_MAX_BEARER_CONTEXT) 
            {
               memset(&ip_config,0,sizeof(ds_ip_config_type));
   
               ds_pdn_psm_info_fill_ipv6_conifg(psm_index,
                                           &ip_config,
                                           &prefix_len);
   
               elapsed_time = ds_pdn_psm_get_elapsed_time();
   
               elapsed_time = (elapsed_time/1000);
   
               if (ip_config.prefix_valid_life_time != 0xFFFFFFF) 
               {
                 prefix_valid_life_time = ip_config.prefix_valid_life_time - elapsed_time;
               }
               else
               {
                  prefix_valid_life_time = 0xFFFFFFF;
               }
               
               if (ip_config.prefix_pref_life_time != 0xFFFFFFF)
               {
                 prefix_pref_life_time  = ip_config.prefix_pref_life_time - elapsed_time; 
               }
               else
               {
                 prefix_pref_life_time = 0xFFFFFFF;
               }
   
               router_life_time = ip_config.router_life_time - elapsed_time;
   
               if (prefix_valid_life_time <= 0) 
               {
                  prefix_valid_life_time = 0;
               }
   
               if (prefix_pref_life_time <= 0) 
               {
                  prefix_pref_life_time = 0;
               }
   
               if (router_life_time <= 0) 
               {
                  router_life_time = 0;
               }
   
               /*---------------------------------------------------------------------
                  Set V6 IP Addr to Iface
               --------------------------------------------------------------------------*/
               if (-1 == ps_apply_v6_prefix(
                          ds_fwk_get_inst_ptr_from_index(pdn_mgr_p->fw_index),
                          ip_config.v6_gateway_iid,
                          ip_config.ip_addr.addr.v6.in6_u.u6_addr64[0],
                          prefix_valid_life_time,
                          prefix_pref_life_time,
                          prefix_len))
               {
                  DS_MSG0(MSG_LEGACY_HIGH,
                                  "Apply V6 Prefix to iface failed");
               }
   
               /*--------------------------------------------------------------------
               Applied prefix successfully Generate a valid RA indication
                  --------------------------------------------------------------------*/
                memset(&event_info, 0, sizeof(ds_pdn_event_info_u_type));
                event_info.ra_lifetime = router_life_time;
                ip6_sm_valid_ra_cback(PDN_VALID_RA_EV, event_info,
                                   pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV6]->ip6_sm);
   
              /*Apply Ipv6 additional iid's like privacy iid etc */
   
              ds_pdn_psm_info_apply_extended_ipv6_conifg(psm_index,
                                           pdn_mgr_p);
   
   
            }
            break;
          default:
            break;
        }
      }
      break;

    default:
      break;
  }

  if (( TRUE == (pdn_mgr_p->is_default) &&
       ind_payload.ind_field_mask != 0) )
  {
    ds_pdn_apm_set_profile_id(&ind_payload, pdn_mgr_p->pdp_profile_num);
    (void)ds_pdn_apm_ind(DS_PDN_APM_IP_ADDRESS_IND, &ind_payload);
  }

  return;
} /* ds_pdn_mgr_handle_v6_ev_cback() */

/*===========================================================================
FUNCTION DS_PDN_MGR_IS_DEFAULT_TX_FLOW_ENABLED
 
DESCRIPTION
  This function returns the TX flow status of the default bearer for this PDN

PARAMETERS 
  PDN pointer
 
DEPENDENCIES 
  None. 
  
RETURN VALUE 
  boolean
 
SIDE EFFECTS 
  None
===========================================================================*/
boolean ds_pdn_mgr_is_default_tx_flow_enabled
(
  ds_pdn_mgr_s     *pdn_mgr_p
)
{  
  if(ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p))
  {
    return ds_bearer_mgr_is_tx_flow_enabled(
                  pdn_mgr_p->bearer_ptr_tbl[DS_3GPP_DEFAULT_BEARER_INDEX]);
  }
    
  return FALSE;
}

/*===========================================================================
FUNCTION DS_PDN_MGR_VALIDATE_PDN_FOR_CALL_ORIG_TYPE
 
DESCRIPTION
  This function validates PDN corresponding to type of call being originated.

PARAMETERS 
  Call origination PDP type, PDN pointers
 
DEPENDENCIES 
  None. 
  
RETURN VALUE 
  boolean - TRUE if PDN exists for the passed PDP type, and fill the corresponding PDN ptr in pdn_mgr_p
                 FALSE otherwise
 
SIDE EFFECTS 
  None
===========================================================================*/
boolean ds_pdn_mgr_validate_pdn_for_call_orig_type
(
  ds_profile_pdp_type_e call_type_orig,
  ds_pdn_mgr_s       *v4_pdn_p,
  ds_pdn_mgr_s       *v6_pdn_p,
  ds_pdn_mgr_s       *non_ip_pdn_p,
  ds_pdn_mgr_s       **pdn_mgr_p
)
{
  switch(call_type_orig)
  {
    case DS_PROFILE_PDP_IPV4:
      if(ds_pdn_mgr_validate_pdn_ptr(v4_pdn_p))
      {
        *pdn_mgr_p = v4_pdn_p;
        return TRUE;
      }
      break;

    case DS_PROFILE_PDP_IPV6:
      if(ds_pdn_mgr_validate_pdn_ptr(v6_pdn_p))
      {
        *pdn_mgr_p = v6_pdn_p;
        return TRUE;
      }
      break;

    case DS_PROFILE_PDP_NON_IP:
      if(ds_pdn_mgr_validate_pdn_ptr(non_ip_pdn_p))
      {
        *pdn_mgr_p = non_ip_pdn_p;
        return TRUE;
      }
      break;

    default:
      break;
  }

  return FALSE;
}/* ds_pdn_mgr_validate_pdn_for_call_orig_type */

/*===========================================================================
  FUNCTION DS_PDN_MGR_LOOKUP_ALWAYS_SHARE_PDN()

  DESCRIPTION
    This function leads to always share PDN lookup.

  PARAMETERS
    Policy Info

  RETURN VALUE
     valid framework index -- if matches with any of the existing pdn
     -1 -- if not matching

  DEPENDENCIES
    Used by BIP.

  SIDE EFFECTS
    None
===========================================================================*/
ds_fwk_index_type ds_pdn_mgr_lookup_always_share_pdn
(
  ds_policy_info_type  policy_info
)
{
  ds_pdn_mgr_s  *pdn_mgr_p     = NULL;
  uint8          index         = 0;
  uint8          default_prof_num = NULL;
  uint8          attach_prof_num = NULL;
  uint8          always_share_index = 0;
  uint8          default_prof_index = 0;
  uint8          attach_prof_index = 0;
  uint8          any_prof_index = 0;
  boolean        always_share_found = FALSE;
  boolean        default_prof_found = FALSE;
  boolean        attach_prof_found = FALSE;
  boolean        any_prof_found = FALSE;
  ds_profile_pdp_type_e pdp_type = DS_PROFILE_PDP_MAX;
/*-------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------
   Priority for chosing PDN is as follows:
	 1) Use deafult PDN
	 2) Use attach PDN
	 3) Use any other PDN
  -------------------------------------------------------------------*/
  default_prof_num = ds_profile_get_profile_number_for_embeded_calls();
  attach_prof_num = ds_eps_pdn_get_attach_prof();

  for(index = 0; index < DS_3GPP_MAX_PDN_CONTEXT; index++)
  {
    pdn_mgr_p = ds_pdn_mgr_get_pdn_ptr_from_index(index);
    if(!ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p))
    {
     	continue;
    }

    if( pdn_mgr_p->state == DS_PDN_MGR_STATE_UP ||
    	   pdn_mgr_p->state == DS_PDN_MGR_STATE_COMING_UP )
    {
      pdp_type = ds_apn_table_get_pdn_pdp_type(pdn_mgr_p);
      switch(pdp_type)
      {
        case DS_PROFILE_PDP_IPV4:
        	 if(policy_info.family != IPV4_ADDR)
        	 {
        	  	continue;
        	 }
        	 break;

        case DS_PROFILE_PDP_IPV6:
        	 if(policy_info.family != IPV6_ADDR)
        	 {
        	  	continue;
        	 }
        	 break;

        case DS_PROFILE_PDP_NON_IP:
        	 if(policy_info.family != NON_IP_ADDR)
        	 {
        	  	continue;
        	 }
        	 break;    

        default:
        	 break;
      }

      /* If PDN is already being shared, use it again */
      if(pdn_mgr_p->is_always_shared == TRUE)
      {
        always_share_index = index;
        always_share_found = TRUE;
        break;
      }
      /* Default PDN has highest priority */
      else if(pdn_mgr_p->pdp_profile_num == default_prof_num)
      {
        default_prof_index = index;
        default_prof_found = TRUE;
      }
      /* Attach PDN has 2nd priority */
      else if(pdn_mgr_p->pdp_profile_num == attach_prof_num)
      {
        attach_prof_index = index;
        attach_prof_found = TRUE;
      }
      /* If neither default nor attach PDN was found, then use the last
         available PDN */
      else
      {
        any_prof_index = index;
        any_prof_found = TRUE;
      }
    }
  }

  if(always_share_found == TRUE)
  {
    return ds_pdn_manager[always_share_index]->fw_index;
  }
  else if(default_prof_found == TRUE)
  {
    ds_pdn_manager[default_prof_index]->is_always_shared = TRUE;
    return ds_pdn_manager[default_prof_index]->fw_index;
  }
  else if(attach_prof_found == TRUE)
  {
    ds_pdn_manager[attach_prof_index]->is_always_shared = TRUE;
    return ds_pdn_manager[attach_prof_index]->fw_index;
  }
  else if(any_prof_found == TRUE)
  {
    ds_pdn_manager[any_prof_index]->is_always_shared = TRUE;
    return ds_pdn_manager[any_prof_index]->fw_index;
  }

  return PDN_MGR_INVALID_FW_INDEX;
}/* ds_pdn_mgr_lookup_always_share_pdn */

/*===========================================================================
  FUNCTION DS_PDN_MGR_LOOKUP_PDN()

  DESCRIPTION
    This function leads to PDN lookup by framework.

  PARAMETERS
    apn, ip_type

  RETURN VALUE
     valid framework index -- if matches with any of the existing pdn
     -1 -- if not matching

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
ds_fwk_index_type ds_pdn_mgr_lookup_pdn
(
  ds_policy_info_type  policy_info
)
{
  ds_pdn_mgr_apn_table_s *apn_table_p = NULL;
  ds_profile_ident_info   prof_ident  = {{0}};
  ds_profile_type_s       prof_info   = {{0}};
  
  /* Look out for always shared PDN if requested by client */
  if((strncasecmp((char*)policy_info.name, 
                   (char *)DS_PROFILE_ALWAYS_SHARE_APN,
                   DS_PROFILE_APN_STRING_LEN+1) == 0))
  {
    return ds_pdn_mgr_lookup_always_share_pdn(policy_info);
  }
  memscpy(prof_ident.apn,DS_PROFILE_APN_STRING_LEN+1,
  	      policy_info.name, DS_SYS_MAX_APN_LEN );
		  
  switch(policy_info.family)
  {
    case IP_ANY_ADDR:
    case IPV4_ADDR:
      prof_ident.pdp_type = DS_PROFILE_PDP_IPV4;
      break;

    case IPV6_ADDR:
      prof_ident.pdp_type = DS_PROFILE_PDP_IPV6;
      break;

    case IFACE_UNSPEC_ADDR_FAMILY:
      prof_ident.pdp_type = DS_PROFILE_PDP_IPV4;
      break;

    case NON_IP_ADDR:
      prof_ident.pdp_type = DS_PROFILE_PDP_NON_IP;
      break;

    default:
      DS_MSG0(MSG_LEGACY_ERROR, "Invalid pdp type passed");
      return PDN_MGR_INVALID_FW_INDEX;
  }

  if((ds_profile_get_profile_id(prof_ident) == INVALID_PROFILE_NUMBER) && 
     (strlen(policy_info.name) == 0) &&
     (ds_apn_table_get_apn_table_ptr((char*)policy_info.name, 
                                     &apn_table_p) == -1))
  {
    ds_profile_get_default_profile_setting(&prof_info);
    memscpy(policy_info.name,
	        DS_SYS_MAX_APN_LEN, prof_info.context.apn,
                strlen(prof_info.context.apn));
    policy_info.length = strlen((char*)policy_info.name);
  }

  /* for NULL apn name check if any override apn is configured via efs */
  if(policy_info.length == 0)
  {
    policy_info.length = ds_pdn_mgr_update_null_override_apn(policy_info.name);
  }

  /* Fallback to existing PDN otherwise */
  if(ds_apn_table_get_apn_table_ptr((char*)policy_info.name, 
                                    &apn_table_p) != -1)
  {
    if(ds_pdn_mgr_validate_pdn_ptr(apn_table_p->v4_pdn_ptr))
    {
      return apn_table_p->v4_pdn_ptr->fw_index;
    }
    else if(ds_pdn_mgr_validate_pdn_ptr(apn_table_p->v6_pdn_ptr))
    {
      return apn_table_p->v6_pdn_ptr->fw_index;
    }
    else if(ds_pdn_mgr_validate_pdn_ptr(apn_table_p->non_ip_pdn_ptr))
    {
      return apn_table_p->non_ip_pdn_ptr->fw_index;
    }
  }

  return PDN_MGR_INVALID_FW_INDEX;
}/* ds_pdn_mgr_lookup_pdn */

/*===========================================================================
  FUNCTION DS_PDN_MGR_BRINGUP_PDN()

  DESCRIPTION
    This function leads to PDN bringup.
    this functions returns immediately with the info such as pdn_ptr and cause codes(incase of failure).
    pdn_ptr will be updated in ds_fwk strcutures for further operations.
    however clients needs to wait for PDN events for PDN UP,DOWN status.

  PARAMETERS
    ds_fwk_idx,apn, ip_type,info

  RETURN VALUE
      0  -- if bringup is successful.(pdn_ptr will be updated in info out param)
     -1 -- if bringup is failed.(failure reason will be updated in info out param)
      102 -Operation would block.(pdn_ptr will be updated in info out param)
      
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
int32 ds_pdn_mgr_bringup_pdn
(
  ds_fwk_index_type               ds_fwk_idx,
  ds_policy_info_type             policy_info,
  ds_fwk_response_info_s_type*    response_info
)
{
  boolean                 roaming_status = FALSE;
  sys_sys_mode_e_type     curr_mode = SYS_SYS_MODE_NONE;
  ds_profile_ident_info   prof_ident = {{0}};
  int8                    prof_num = 0;
  ds_profile_type_s       prof_info = {{0}};
  ds_profile_pdp_type_e   call_type_orig;
  ds_local_err_val_e_type local_err = DS_LOCAL_ERR_VAL_NONE;
#ifdef FEATURE_DATA_LTE
  //ds_eps_tlb_per_subs_info_type    *tlb_hndl = NULL;
#endif
  ds_apn_ip_support_type_e       ip_support;
  ds_pdn_mgr_s                  *pdn_mgr_p = NULL;
  ds_pdn_mgr_s                  *v4_pdn_p = NULL;
  ds_pdn_mgr_s                  *v6_pdn_p = NULL;
  ds_pdn_mgr_s                  *non_ip_pdn_p = NULL;
  ds_pdn_mgr_inst_type_s        *inst_p = NULL;
  ds_pdn_mgr_inst_type_s        *comp_inst_p = NULL;
  ds_pdn_mgr_instance_type_e     inst_type = DS_PDN_INSTANCE_MAX;
  ds_pdn_mgr_instance_type_e     comp_inst_type = DS_PDN_INSTANCE_MAX;
  boolean                        single_bearer = FALSE;
  boolean                        multi_pdn_same_apn = FALSE;
  ds_command_type               *cmd_ptr;
  ds_pdn_mgr_instance_cmd_type  *pdn_cmd_info;
  ds_pdn_mgr_apn_table_s        *apn_table_p = NULL; 
  boolean                        new_pdn = FALSE;
  ds_pdn_cp_data_mo_ex_force_attach_result_type
                                 force_attach_result = 
                               DS_PDN_CP_DATA_MO_EXCP_FORCE_ATTACH_UNSUPPORTED;

  roaming_status = ds_pdn_roaming_get_status();
  curr_mode = ds_pdn_hdlr_get_current_network_mode();
  ds_3gpp_mo_exp_activity_status_ind_type  data_activity_start;
  boolean                        v4_clat_needed = FALSE;

  DS_MSG2(MSG_LEGACY_HIGH,"Roaming status %d in bring up of call on mode %d",
                          roaming_status, curr_mode);

  /* Look out for always shared PDN if requested by client */
  if((strncasecmp((char*)policy_info.name, 
                   (char *)DS_PROFILE_ALWAYS_SHARE_APN,
                   DS_PROFILE_APN_STRING_LEN+1) == 0))
  {
    /* Passed FWK index was already slected during look up,
           so try to get APN from corresponding PDN, else
           fallback to default profile APN */
    pdn_mgr_p = ds_pdn_mgr_get_pdn_from_fwk_index(ds_fwk_idx);
    if(ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p))
    {
      memscpy(prof_ident.apn,
               DS_PROFILE_APN_STRING_LEN, pdn_mgr_p->pdp_profile->context.apn,
               strlen(pdn_mgr_p->pdp_profile->context.apn));
    }
    else
    {
      ds_profile_get_default_profile_setting(&prof_info);

    memscpy(prof_ident.apn,
             DS_PROFILE_APN_STRING_LEN, prof_info.context.apn,
             strlen(prof_info.context.apn));

    }
  }
  else
  {
    memscpy(prof_ident.apn,
             DS_PROFILE_APN_STRING_LEN, policy_info.name,
             strlen(policy_info.name));

  }
  
  switch(policy_info.family)
  {
    case IP_ANY_ADDR:
    case IPV4_ADDR:
      prof_ident.pdp_type = DS_PROFILE_PDP_IPV4;
      break;

    case IPV6_ADDR:
      prof_ident.pdp_type = DS_PROFILE_PDP_IPV6;
      break;

    case IFACE_UNSPEC_ADDR_FAMILY:
      prof_ident.pdp_type = DS_PROFILE_PDP_IPV4;
      break;

    case NON_IP_ADDR:
      prof_ident.pdp_type = DS_PROFILE_PDP_NON_IP;
      break;

    default:
      response_info->netdown_reason = DS_NET_DOWN_REASON_IP_VERSION_MISMATCH;
      local_err = DS_LOCAL_ERR_VAL_0;
      goto func_exit;
  }
  
  prof_num = ds_profile_get_profile_id(prof_ident); 
  if(prof_num == INVALID_PROFILE_NUMBER)
  { 
    prof_num = ds_profile_get_profile_number_for_embeded_calls();
    if(ds_apn_table_get_apn_table_ptr(prof_ident.apn, &apn_table_p) != -1)
    {
      memset(prof_ident.apn, '\0', DS_PROFILE_APN_STRING_LEN + 1);
      memscpy(prof_ident.apn,
               DS_PROFILE_APN_STRING_LEN, apn_table_p->apn,
               strlen(apn_table_p->apn));

      if(strlen(prof_ident.apn) == 0 &&
         strlen(apn_table_p->resolved_apn) > 0)
      {
        memset(prof_ident.apn, '\0', DS_PROFILE_APN_STRING_LEN + 1);
        memscpy(prof_ident.apn,
		              DS_PROFILE_APN_STRING_LEN, 
                apn_table_p->resolved_apn,
             		 strlen(apn_table_p->resolved_apn));
      }   
    }
    else
    {
      /* If the profile is not present, use default profile parameters.
         Copy the APN passed by client later. If APN is NULL, use the 
         APN from default profile */

      ds_profile_get_default_profile_setting(&prof_info);

      if(strlen(policy_info.name) == 0)
      {
        memscpy(prof_ident.apn, DS_PROFILE_APN_STRING_LEN,
                prof_info.context.apn,	strlen(prof_info.context.apn));
      }     
    }

    if(prof_num == INVALID_PROFILE_NUMBER)
    {      
      response_info->netdown_reason = DS_NET_DOWN_REASON_INVALID_PROFILE;
      local_err = DS_LOCAL_ERR_VAL_0;
      goto func_exit;
    }
  } 

  /* Store the APN to be used in policy info */
  memscpy(policy_info.name, DS_PROFILE_APN_STRING_LEN,
  	       (char*)prof_ident.apn, DS_PROFILE_APN_STRING_LEN);
  policy_info.length = strlen((char*)policy_info.name);

  /* for NULL apn name check if any override apn is configured via efs */
  if(policy_info.length == 0)
  {
    policy_info.length = ds_pdn_mgr_update_null_override_apn(policy_info.name);
  }

  if( ds_profile_setting_from_profile_id(prof_num, &prof_info) != 
                                                  DS_PROFILE_RESULT_SUCCESS)
  {
    response_info->netdown_reason = DS_NET_DOWN_REASON_INVALID_PROFILE;
    local_err = DS_LOCAL_ERR_VAL_1;
    goto func_exit;
  }
                                                  

  if( curr_mode != SYS_SYS_MODE_LTE &&
      curr_mode != SYS_SYS_MODE_GSM && ! ds_pdn_mgr_ue_is_in_thermal_level3_mode() && 
	  !ds_apn_table_is_pdn_associated_for_apn(policy_info.name)
#ifdef FEATURE_GPS_WWAN_PRIORITY_SEARCH
          &&!dssysmgr_get_gps_search_status()
#endif /* FEATURE_GPS_WWAN_PRIORITY_SEARCH */
      )
  {
    response_info->netdown_reason = DS_NET_DOWN_REASON_UNPREFERRED_RAT;    
    local_err = DS_LOCAL_ERR_VAL_2;
    goto func_exit;
  }

  //ds3gpp_compare_pdp_profile_info

#if 0  
#ifdef FEATURE_DATA_LTE
    tlb_hndl = ds_eps_tlb_get_info_hndl(resolved_subs_id);
    if (ds_eps_tlb_validate_info_hndl(tlb_hndl))
    {
       if((!(policy_info_ptr->tlb_mode)) &&
       ( tlb_hndl->tlb_state.mode == LTE_TLB_MODE_A || 
         tlb_hndl->tlb_state.mode == LTE_TLB_MODE_B || 
         tlb_hndl->tlb_state.mode == LTE_TLB_MODE_G ||
         tlb_hndl->tlb_state.mode == LTE_TLB_MODE_I ||
         tlb_hndl->tlb_state.mode == LTE_TLB_MODE_H ))
       {
         response_info->netdown_reason = 
                       DS_NET_DOWN_REASON_TEST_LOOPBACK_MODE_A_OR_B_ENABLED;    
         local_err = DS_LOCAL_ERR_VAL_3;
         goto func_exit;
       }
    }
#endif
#endif

  if(roaming_status)
  {
    if(prof_info.roaming_disallowed)
    {      
      response_info->netdown_reason = 
                             DS_NET_DOWN_REASON_APN_DISALLOWED_ON_ROAMING;    
      local_err = DS_LOCAL_ERR_VAL_4;
      goto func_exit;
    }
  }

  prof_info.context.pdp_type = ds_pdn_mgr_determine_pdp_type(&prof_info.context,
                                                             roaming_status);  
  DS_MSG2(MSG_LEGACY_HIGH,"Profile PDP type : %d, Policy family : %d",
                          prof_info.context.pdp_type, policy_info.family);
  switch(prof_info.context.pdp_type)
  {
    case DS_PROFILE_PDP_IPV4: 
    {
      switch(policy_info.family)
      {
        case IPV4_ADDR:
        case IP_ANY_ADDR:
        case IFACE_UNSPEC_ADDR_FAMILY:
          call_type_orig = DS_PROFILE_PDP_IPV4;
          break;
       
        case NON_IP_ADDR:
        case IPV6_ADDR:
        default:
          call_type_orig = DS_PROFILE_PDP_MAX;
          break;           
      }
    }
    break;
     
    case DS_PROFILE_PDP_IPV6: 
    {
      switch(policy_info.family)
      {
        case IPV6_ADDR:
        case IP_ANY_ADDR:
        case IFACE_UNSPEC_ADDR_FAMILY:
          call_type_orig = DS_PROFILE_PDP_IPV6;            
          break;
            
        case IPV4_ADDR:
          if(ds_pdnmgr_is_prof_clat_enabled(prof_num))
          {
            call_type_orig = DS_PROFILE_PDP_IPV6;
            v4_clat_needed = TRUE;
          }
          else
          {
            call_type_orig = DS_PROFILE_PDP_MAX;
          }
          break;
        case NON_IP_ADDR:
        default:
          call_type_orig = DS_PROFILE_PDP_MAX;
          break;
      }
    }
    break;
  
    case DS_PROFILE_PDP_IPV4V6:
      switch(policy_info.family)
      {
        case IPV6_ADDR:
          call_type_orig = DS_PROFILE_PDP_IPV6;            
          break;
        case IPV4_ADDR:
          call_type_orig = DS_PROFILE_PDP_IPV4;
          break;
        case IP_ANY_ADDR:
        case IFACE_UNSPEC_ADDR_FAMILY:
          call_type_orig = DS_PROFILE_PDP_IPV4; // check this later
          break;
  
        case NON_IP_ADDR:
        default:
          call_type_orig = DS_PROFILE_PDP_MAX;
          break;
      }    
      break;
  
    case DS_PROFILE_PDP_NON_IP:
     /*------------------------------------------------------------------------
          In case of NON-IP call, the policy will have the ip_family as V4.
        ------------------------------------------------------------------------*/
      if(NON_IP_ADDR == policy_info.family)
      {
        call_type_orig = DS_PROFILE_PDP_NON_IP;
      }
      else
      {
        call_type_orig = DS_PROFILE_PDP_MAX;
      }
      break;
  
    default:
      call_type_orig = DS_PROFILE_PDP_MAX;
      break;
  }

  if(call_type_orig == DS_PROFILE_PDP_MAX)
  {  
    response_info->netdown_reason = 
                           DS_NET_DOWN_REASON_IP_VERSION_MISMATCH;    
    local_err = DS_LOCAL_ERR_VAL_5;
    goto func_exit;
  }

  if( call_type_orig == DS_PROFILE_PDP_NON_IP &&
      (SYS_RAT_LTE_NB1_RADIO_ACCESS != ds_pdn_mgr_get_active_rat()
#ifdef FEATURE_NBIOT_NTN 
      && SYS_RAT_LTE_NB1_NTN_RADIO_ACCESS != ds_pdn_mgr_get_active_rat()
#endif /* FEATURE_NBIOT_NTN */
       ))
  {  
    response_info->netdown_reason = 
                           DS_NET_DOWN_REASON_NON_IP_NOT_SUPPORTED;    
    local_err = DS_LOCAL_ERR_VAL_6;
    goto func_exit;
  }

  if(ds_pdn_profile_validate_apn_name((char*)policy_info.name,
                                       policy_info.length) == FALSE)
  {  
    response_info->netdown_reason = 
                           DS_NET_DOWN_REASON_APN_FORMAT_INVALID;    
    local_err = DS_LOCAL_ERR_VAL_7;
    goto func_exit;
  }
  
  /* MO exceptional Handling */
  if ((TRUE == policy_info.mo_excp_data_cap) && 
      (FALSE == ds_pdn_cp_data_is_mo_exception_data_supported())) 
  {
    response_info->netdown_reason = 
                           DS_NET_DOWN_REASON_MO_EXCEPTIONAL_NOT_SUPPORTED;    
    local_err = DS_LOCAL_ERR_VAL_8;
    goto func_exit;
  }
  
  /*---------------------------------------------------------------------
  If call is already up for the requested PDP type, return sucess with PDN pointer.
  If call is in coming up state, return E_WOULD_BLOCK and fill PDN pointer.
  If call is in going down state, return DS_ENETCLOSEINPROGRESS.
  ----------------------------------------------------------------------*/
  if(ds_apn_table_is_pdn_associated_for_apn((char*)policy_info.name) == TRUE)     
  {
    if(call_type_orig == DS_PROFILE_PDP_IPV4)
    {
      ds_apn_table_get_v4_pdn((char*)policy_info.name, &pdn_mgr_p);
      if(ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p))
      {
        inst_p = pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV4];
        DS_MSG1(MSG_LEGACY_HIGH," v4 pdn ptr valid and inst state %d ",inst_p->state);
      }
      else if(ds_pdnmgr_is_prof_clat_enabled(prof_num))
      {
        DS_MSG0(MSG_LEGACY_HIGH," v4 pdn ptr is not valid, fetch v6 pdn ptr ");
        ds_apn_table_get_v6_pdn((char*)policy_info.name, &pdn_mgr_p);
        if(ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p))
        {
          if(pdn_mgr_p->is_restore_mode && pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV4] == NULL)
          {
            /* For PSM case v4 instance will be NULL for v6only type PDN */
            DS_MSG0(MSG_LEGACY_HIGH," PSM scenario. Setting appropriate params");
            inst_p = pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV6];
            call_type_orig = DS_PROFILE_PDP_IPV6;
            v4_clat_needed = TRUE;
          }
          else
          {
            inst_p = pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV4];
          }
        }
      }
    } 
    else if(call_type_orig == DS_PROFILE_PDP_IPV6)
    {
      ds_apn_table_get_v6_pdn((char*)policy_info.name, &pdn_mgr_p);
      if(ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p))
      {
        inst_p = pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV6];
      }
    }     
    else if(call_type_orig == DS_PROFILE_PDP_NON_IP)
    {
      ds_apn_table_get_non_ip_pdn((char*)policy_info.name, &pdn_mgr_p);
      if(ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p))
      {
        inst_p = pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_NON_IP];
      }
    } 

    if(inst_p != NULL)
    {
      /*---------------------------------------------------------------
        Policy request for MO exception which is supported but the already existing PDN
        was not marked for MO exception data. For this, we need to set it to the 
        existing PDN.
      ---------------------------------------------------------------*/
      if( (TRUE == policy_info.mo_excp_data_cap) &&
          (TRUE == ds_pdn_cp_data_is_mo_exception_data_supported()) )
      {
        if( (pdn_mgr_p->lte_params != NULL) &&
            (pdn_mgr_p->lte_params->mo_exception_data == FALSE) )
        {
          pdn_mgr_p->lte_params->mo_exception_data = TRUE;
          pdn_mgr_p->lte_params->allow_force_attach = TRUE;
          memset(&data_activity_start, 0, 
                 sizeof(ds_3gpp_mo_exp_activity_status_ind_type));
          
          data_activity_start.act_status = TRUE;
          (void) ds_msgr_snd_msg_send_ext( 
                     DS_3GPP_MO_EXP_ACTIVITY_STATUS_IND,
                     MSGR_DS_3GPP, 
                    (msgr_hdr_struct_type*)(&data_activity_start),
                     sizeof(ds_3gpp_mo_exp_activity_status_ind_type));
          DS_MSG0(MSG_LEGACY_HIGH," mo_ex data activity start");
        }
      }   
      
    /*---------------------------------------------------------------
        If the instance is not NULL, and in down state, it means that it was 
        allocated as part of call bring up, but not yet brought up. So, it should be 
        treated as coming up instance here.
      ---------------------------------------------------------------*/
      if( inst_p->state == DS_INSTANCE_STATE_DOWN ||
      	   inst_p->state == DS_INSTANCE_STATE_COMING_UP )
      {
        /* MO Exception Handling */
        if( (inst_p->state == DS_INSTANCE_STATE_COMING_UP) &&
            (TRUE == policy_info.mo_excp_data_cap) &&
            (TRUE == ds_pdn_cp_data_is_mo_exception_data_supported()) )
        {
          if (ds_eps_pdn_attach_sm_get_state() == DS_EPS_PROCESSING_GET_PDN_CONN_IND)
          {
            force_attach_result = ds_pdn_cp_data_trigger_mo_ex_force_attach(pdn_mgr_p);
          } 
        } 
       
        inst_p->in_use = TRUE;
		      pdn_mgr_p->fw_index = ds_fwk_idx;
        response_info->pdn_ptr = pdn_mgr_p;
        DS_MSG2(MSG_LEGACY_HIGH," Create dummy v4 instance as v6 instance %d is in coming up state: v4_clat_needed: %d",
                                 inst_p->type,v4_clat_needed);
        /* Bringup dummy v4 instance */
        if(v4_clat_needed)
        {
          pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV4] = 
                   ds_pdn_mgr_allocate_new_instance(pdn_mgr_p, DS_PDN_INSTANCE_IPV4);
        }
        return DS_EWOULDBLOCK;
      }
      else if(inst_p->state == DS_INSTANCE_STATE_UP)
      {
        inst_p->in_use = TRUE;
        pdn_mgr_p->fw_index = ds_fwk_idx;
        response_info->pdn_ptr = pdn_mgr_p;
        DS_MSG2(MSG_LEGACY_HIGH,"create dummy v4 instance as v6 instance %d is UP state: v4_clat_needed: %d",
                                 inst_p->type,v4_clat_needed);
        /* Bringup dummy v4 instance */
        if(v4_clat_needed)
        {
          pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV4] = 
                   ds_pdn_mgr_allocate_new_instance(pdn_mgr_p, DS_PDN_INSTANCE_IPV4);
        }
        return 0;
      }
      else if(inst_p->state == DS_INSTANCE_STATE_GOING_DOWN)
      {
        response_info->netdown_reason = DS_NET_DOWN_REASON_CLOSE_IN_PROGRESS;
        return DS_ENETCLOSEINPROGRESS;
      }
    }
  }

  if((ds_apn_table_get_apn_ip_support_info((char*)policy_info.name, 
                                             &ip_support) == TRUE) &&
     (ds_apn_table_is_pdn_associated_for_apn((char*)policy_info.name) == TRUE)
    )
  { 
    DS_MSG1(MSG_LEGACY_HIGH, "Found an entry in apn table. IP Support: %d",
                             ip_support);     
    
    ds_apn_table_get_v4_pdn((char*)policy_info.name, &v4_pdn_p);
    ds_apn_table_get_v6_pdn((char*)policy_info.name, &v6_pdn_p);
    ds_apn_table_get_non_ip_pdn((char*)policy_info.name, &non_ip_pdn_p);

    switch(ip_support)
    {
      case DS_IPV4V6_NON_IP:
        if(ds_pdn_mgr_validate_pdn_for_call_orig_type(call_type_orig,
                                                      v4_pdn_p,
                                                      v6_pdn_p,
                                                      non_ip_pdn_p,
                                                      &pdn_mgr_p) == FALSE)
        {
          response_info->netdown_reason = 
                                      DS_NET_DOWN_REASON_IP_VERSION_MISMATCH;    
          local_err = DS_LOCAL_ERR_VAL_9;
          goto func_exit;           
        }
        break;

      case DS_IPV4V6:
        if(call_type_orig != DS_PROFILE_PDP_NON_IP)
        {
          if(ds_pdn_mgr_validate_pdn_for_call_orig_type(call_type_orig,
                                                        v4_pdn_p,
                                                        v6_pdn_p,
                                                        non_ip_pdn_p,
                                                        &pdn_mgr_p) == FALSE)
          {
            response_info->netdown_reason = 
                                        DS_NET_DOWN_REASON_IP_VERSION_MISMATCH;    
            local_err = DS_LOCAL_ERR_VAL_10;
            goto func_exit;           
          }           
        }
        break;

      /*----------------------------------------------------------------
        For single bearer cause codes, if PDN exists for the requested 
        PDP type in call origination, use it. Else, for dual IP case, 
        if companion instance is not present, bring it up with new PDN.
      ----------------------------------------------------------------*/
      case DS_SINGLE_BEARER:
        if(call_type_orig == DS_PROFILE_PDP_IPV4) 
             
        {
          if(ds_pdn_mgr_validate_pdn_ptr(v4_pdn_p) == TRUE)
          {
            pdn_mgr_p = v4_pdn_p;
          }
          
          single_bearer = TRUE;
          break;
        }
        else if(call_type_orig == DS_PROFILE_PDP_IPV6)              
        {
          if(ds_pdn_mgr_validate_pdn_ptr(v6_pdn_p) == TRUE)
          {
            pdn_mgr_p = v6_pdn_p;
          }
          
          single_bearer = TRUE;
          break;
        }
        else if(call_type_orig == DS_PROFILE_PDP_NON_IP)              
        {
          if(ds_pdn_mgr_validate_pdn_ptr(non_ip_pdn_p))
          {
            pdn_mgr_p = non_ip_pdn_p;
          }

          break;
        }
        break;

      case DS_IPV4_ONLY:
      case DS_IPV4_NON_IP:
        if(ds_pdn_mgr_validate_pdn_for_call_orig_type(call_type_orig,
                                                      v4_pdn_p,
                                                      v6_pdn_p,
                                                      non_ip_pdn_p,
                                                      &pdn_mgr_p) == FALSE)
        {
          if( call_type_orig == DS_PROFILE_PDP_IPV6 )
          {
            if(ds_pdn_nv_manager_get_multi_pdn_same_apn_nv() == FALSE)
            {
              response_info->netdown_reason = 
                                DS_NET_DOWN_REASON_ERR_PDN_IPV6_CALL_DISALLOWED;    
              local_err = DS_LOCAL_ERR_VAL_13;
              goto func_exit;           
            }
            else
            {
              multi_pdn_same_apn = TRUE;
            }
          }
        }           
        break;
        
      case DS_IPV6_ONLY:
      case DS_IPV6_NON_IP:
        if(ds_pdn_mgr_validate_pdn_for_call_orig_type(call_type_orig,
                                                      v4_pdn_p,
                                                      v6_pdn_p,
                                                      non_ip_pdn_p,
                                                      &pdn_mgr_p) == FALSE)
        {
          if( call_type_orig == DS_PROFILE_PDP_IPV4 )
          {
            if(ds_pdn_nv_manager_get_multi_pdn_same_apn_nv() == FALSE)
            {
              response_info->netdown_reason = 
                                DS_NET_DOWN_REASON_ERR_PDN_IPV4_CALL_DISALLOWED;    
              local_err = DS_LOCAL_ERR_VAL_14;
              goto func_exit;           
            }
            else
            {
              multi_pdn_same_apn = TRUE;
            }
          }
        }           
        break;

      case DS_NON_IP:
        if(call_type_orig == DS_PROFILE_PDP_NON_IP)
        {
          if(ds_pdn_mgr_validate_pdn_ptr(non_ip_pdn_p) == FALSE)
          {
            response_info->netdown_reason = 
                                      DS_NET_DOWN_REASON_IP_VERSION_MISMATCH;    
            local_err = DS_LOCAL_ERR_VAL_15;
            goto func_exit; 
          }
          else
          {
            pdn_mgr_p = non_ip_pdn_p;
          }
        }
        break;

      default:
        break;
    }  
  }

  if(ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p) == FALSE)
  {
    pdn_mgr_p = ds_pdn_mgr_allocate_new_pdn_context( );
    if(ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p) == FALSE)
    {
      response_info->netdown_reason = DS_NET_DOWN_REASON_INSUFFICIENT_RESOURCES;    
      local_err = DS_LOCAL_ERR_VAL_15;
      goto func_exit; 
    }
    
    new_pdn = TRUE;
    pdn_mgr_p->pdp_profile_num = prof_num; 
    pdn_mgr_p->apn_type = policy_info.apn_type;
    
    (void)ds_profile_setting_from_profile_id(pdn_mgr_p->pdp_profile_num, 
                                             pdn_mgr_p->pdp_profile);

    /* Copy the APN in policy info to PDN profile context */
    memset(pdn_mgr_p->pdp_profile->context.apn, 
           '\0', DS_PROFILE_APN_STRING_LEN + 1);
    memscpy(pdn_mgr_p->pdp_profile->context.apn, DS_PROFILE_APN_STRING_LEN,
            policy_info.name, strlen(policy_info.name));
      
    /* Store the framework index in PDN */
    pdn_mgr_p->fw_index = ds_fwk_idx;

    /* Store the auth params from client, if valid in PDN. We are not 
       allowing client to create profile. And hence, we copy the required
       auth parameters from client in local profile cache of this PDN that
       needs to be exchanged OTA. */
    if(policy_info.authentication_params.auth_param_valid)
    {
      pdn_mgr_p->pdp_profile->auth = 
                        	policy_info.authentication_params.auth_params;
    }

    if( (TRUE == policy_info.mo_excp_data_cap) &&
        (TRUE == ds_pdn_cp_data_is_mo_exception_data_supported()) )
    {
      if( (pdn_mgr_p->lte_params != NULL) &&
          (pdn_mgr_p->lte_params->mo_exception_data == FALSE) )
      {
        pdn_mgr_p->lte_params->mo_exception_data = TRUE;
        pdn_mgr_p->lte_params->allow_force_attach = TRUE;
      }
    }    

    if(v4_clat_needed == TRUE)
      pdn_mgr_p->cache_v4_inst_down = TRUE;
  }

  (void)ds_pdn_mgr_process_pdp_profile_info(pdn_mgr_p, &ip_support);
  if(call_type_orig == DS_PROFILE_PDP_IPV4)
  {
    inst_type = DS_PDN_INSTANCE_IPV4;
    comp_inst_type = DS_PDN_INSTANCE_IPV6;
  }
  else if(call_type_orig == DS_PROFILE_PDP_IPV6)
  {
    inst_type = DS_PDN_INSTANCE_IPV6;
    comp_inst_type = DS_PDN_INSTANCE_IPV4;
  }
  else if(call_type_orig == DS_PROFILE_PDP_NON_IP)
  {
    inst_type = DS_PDN_INSTANCE_NON_IP;
  }

  inst_p = pdn_mgr_p->inst_ptr[inst_type] = 
                     ds_pdn_mgr_allocate_new_instance(pdn_mgr_p, inst_type);
  if(inst_p == NULL)
  {
    local_err = DS_LOCAL_ERR_VAL_16;
    goto func_exit; 
  }

  if((pdn_mgr_p->pdp_profile->context.pdp_type == DS_PROFILE_PDP_IPV4V6 || //tbd_aks:clat changes
      v4_clat_needed == TRUE ) &&
      single_bearer == FALSE &&
      multi_pdn_same_apn == FALSE &&
      pdn_mgr_p->inst_ptr[comp_inst_type] == NULL )
  {
    comp_inst_p = pdn_mgr_p->inst_ptr[comp_inst_type] = 
                   ds_pdn_mgr_allocate_new_instance(pdn_mgr_p, comp_inst_type);    
    if(comp_inst_p == NULL)
    {
      local_err = DS_LOCAL_ERR_VAL_17;
      goto func_exit; 
    }
  } 

  if(single_bearer)
  {
    if(inst_type == DS_PDN_INSTANCE_IPV4)
    {
      pdn_mgr_p->pdp_profile->context.pdp_type = DS_PROFILE_PDP_IPV4;
      ip_support = DS_IPV4_SINGLE_BEARER;
    }
    else if(inst_type == DS_PDN_INSTANCE_IPV6)
    {
      pdn_mgr_p->pdp_profile->context.pdp_type = DS_PROFILE_PDP_IPV6;
      ip_support = DS_IPV6_SINGLE_BEARER;
    }
  }
  else if(multi_pdn_same_apn)
  {
    if(inst_type == DS_PDN_INSTANCE_IPV4)
    {
      pdn_mgr_p->pdp_profile->context.pdp_type = DS_PROFILE_PDP_IPV4;
      ip_support = DS_IPV4_SINGLE_BEARER;
    }
    else if(inst_type == DS_PDN_INSTANCE_IPV6)
    {
      pdn_mgr_p->pdp_profile->context.pdp_type = DS_PROFILE_PDP_IPV6;
      ip_support = DS_IPV6_SINGLE_BEARER;
    }
  }

  if( FALSE == ds_apn_table_add_pdn((char*)policy_info.name, 
                                        ip_support, pdn_mgr_p) )
  {    
    local_err = DS_LOCAL_ERR_VAL_18;
    goto func_exit; 
  }

  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_pdn_mgr_instance_cmd_type));
  if(cmd_ptr == NULL)
  {    
    local_err = DS_LOCAL_ERR_VAL_19;
    goto func_exit; 
  }

  /* Set the in_use flag for this instance */
  inst_p->in_use = TRUE;

  cmd_ptr->hdr.cmd_id = DS_CMD_PDN_INSTANCE_UP;
  pdn_cmd_info = (ds_pdn_mgr_instance_cmd_type*)cmd_ptr->cmd_payload_ptr;
  pdn_cmd_info->pdn_mgr_p = pdn_mgr_p;
  pdn_cmd_info->inst_type = inst_type;
  pdn_cmd_info->info_ptr = (ds_pdn_network_params_info_type *)policy_info.user_info_ptr;  
  
  ds_put_cmd(cmd_ptr);

  response_info->pdn_ptr = pdn_mgr_p;
  return DS_EWOULDBLOCK;

func_exit:
  if(local_err != DS_LOCAL_ERR_VAL_NONE)
  {
    DS_ERROR_LOG_2("Bring up failed with down reason =x, local_err =d",
                   response_info->netdown_reason, local_err);

    /* Clean up the PDN only if it is allocated newly for this client bring up */
    if( ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p) &&
        new_pdn == TRUE )
    {
      ds_pdn_mgr_cleanup_pdn(pdn_mgr_p, DS_PDN_INSTANCE_MAX);
    }
  }   

  return -1;
}/* ds_pdn_mgr_bringup_pdn */


/*===========================================================================
  FUNCTION DS_PDN_MGR_TEARDOWN_PDN()

  DESCRIPTION
    This function leads to PDN teardown.
    teardown function returns immediately from MH and clients needs to wait for pdn_down 
    events for cleanup.  as part of PDN down events ds framework posts events to its clients 
    and deletes the ds_framework instance incase of v4,v6,non-ip pdn's are invalid.
    
  PARAMETERS
    apn, ip_type

  RETURN VALUE
      0  -- if teardown is successful.
     -1 -- if teardown is failed.
      102 -Operation would block.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
int32 ds_pdn_mgr_teardown_pdn
(
  ds_pdn_mgr_s                  *pdn_ptr,
  ip_addr_enum_type             ip_type,
  ds_fwk_response_info_s_type*  response_info
)
{
  ds_pdn_mgr_s                *pdn_mgr_p = NULL;
  ds_pdn_mgr_instance_type_e   inst_type;
  ds_pdn_mgr_inst_type_s      *inst_p = NULL;
  ds_pdn_mgr_inst_type_s      *comp_inst_p = NULL;
  boolean                      comp_inst_in_use = FALSE;
  boolean                      ota_teardown = FALSE;
  sys_sys_mode_e_type          mode = SYS_SYS_MODE_NONE;

  pdn_mgr_p = (ds_pdn_mgr_s*)pdn_ptr;
  if(ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p) == FALSE)
  {
    DS_ERROR_LOG_0("NULL PDN");
    return -1;
  }

  switch(ip_type)
  {
    case IPV4_ADDR:
    case IP_ANY_ADDR:
    case IFACE_UNSPEC_ADDR_FAMILY:
      inst_type = DS_PDN_INSTANCE_IPV4;
      response_info->ip_type = IPV4_ADDR;
      break;

    case IPV6_ADDR:
      inst_type = DS_PDN_INSTANCE_IPV6;
      response_info->ip_type = IPV6_ADDR;
      break;

    case NON_IP_ADDR:
      inst_type = DS_PDN_INSTANCE_NON_IP;
      response_info->ip_type = NON_IP_ADDR;
      break;

    default:
      DS_ASSERT(0);
      return -1;
  }

  inst_p = pdn_mgr_p->inst_ptr[inst_type];
  if(!inst_p)
  {
    DS_MSG2(MSG_LEGACY_HIGH, "Instance %d for PDN %d already down",
                             inst_type, pdn_mgr_p->index);
    response_info->netdown_reason = pdn_mgr_p->net_down_reason;
    return 0;
  }
  else if(inst_p->state == DS_INSTANCE_STATE_GOING_DOWN)
  {
    DS_MSG2(MSG_LEGACY_HIGH, "Instance %d for PDN %d already down",
                             inst_type, pdn_mgr_p->index);
    return DS_EWOULDBLOCK;
  }

  comp_inst_p = ds_pdn_mgr_get_companion_inst(pdn_mgr_p, inst_type);
  if(comp_inst_p && comp_inst_p->in_use)
  {
    comp_inst_in_use = TRUE;
  }
  DS_MSG2(MSG_LEGACY_HIGH, "Comp Instance in use flag: %d  cache_v4_inst_down %d",
                            comp_inst_in_use,pdn_mgr_p->cache_v4_inst_down);
  
  mode = pdn_mgr_p->nw_mode;

  if(!comp_inst_in_use || pdn_mgr_p->cache_v4_inst_down)
  {
    pdn_mgr_p->ue_initiated_tear_down = TRUE;

    if(mode != SYS_SYS_MODE_LTE)
    {
      ota_teardown = TRUE;
    }
    else if(ds_pdn_roaming_is_apn_disallowed(
                          pdn_mgr_p->pdp_profile->context.apn))
    {
      ds_pdn_roaming_clear_wait_timer_entry(pdn_mgr_p->pdp_profile_num);
      ota_teardown = TRUE;
    }
    else if(ds_eps_attach_hdlr_get_nw_attach_wo_pdn_support())
    {
      ota_teardown = TRUE;
    }
    else /* LTE specific handling */
    {
      if(pdn_mgr_p->is_default)
      {
        if( ds_pdn_nv_manager_get_allow_default_pdn_teardown() &&
            ds_pdn_mgr_is_any_other_pdn_up(pdn_mgr_p) )
        {
          ota_teardown = TRUE;
        }
      }
      else /* Non default PDN on LTE */
      {
        if( ds_pdn_nv_manager_get_always_connect_def_pdn_flag() ||
            ds_pdn_mgr_is_any_other_pdn_up(pdn_mgr_p) )
        {
          ota_teardown = TRUE;
        }
      }

      if( pdn_mgr_p->force_ota_teardown ||
          pdn_mgr_p->ims_sip_reg_failure_flag )
      {
        ota_teardown = TRUE;
      }
    }
  }

  /* Enable flow on default bearer */
  ds_bearer_mgr_enable_tx_flow(
                 pdn_mgr_p->bearer_ptr_tbl[DS_3GPP_DEFAULT_BEARER_INDEX],
                 DS_BEARER_CONTROL_MASK);

  inst_p->in_use = FALSE;

  if(ota_teardown)
  {
    pdn_mgr_p->state = DS_PDN_MGR_STATE_GOING_DOWN;
    ds_pdn_mgr_teardown_instances_for_pdn(pdn_mgr_p, 
                                          DS_NET_DOWN_REASON_CLIENT_END);    
    return DS_EWOULDBLOCK;
  }
  else
  {
    if(ds_pdn_cp_data_is_force_attach_in_progress() &&
       comp_inst_in_use == FALSE)
    {
      ds_pdn_cp_data_skip_pdn_req_after_force_attach(pdn_mgr_p);
    }
    DS_MSG0(MSG_LEGACY_HIGH,"No OTA tear down");
    if(pdn_mgr_p->cache_v4_inst_down && 
       pdn_mgr_p->pdp_profile->context.pdp_type == DS_PROFILE_PDP_IPV6)
    {
      DS_MSG0(MSG_LEGACY_HIGH,"Cleanup Dummy v4 instance");
      ds_pdn_mgr_cleanup_inst(pdn_mgr_p,DS_PDN_INSTANCE_IPV4);
    }
    return 0;
  }  
}/* ds_pdn_mgr_teardown_pdn */

/*===========================================================================
FUNCTION DS_PDN_MGR_GET_PDN_PTR

DESCRIPTION
  This function fetches the PDN Manager Pointer corresponding to the
  given index.
 
PARAMETERS
  PDN Index

DEPENDENCIES
  None.

RETURN VALUE
  PDN Manager Pointer.

SIDE EFFECTS
  None.
===========================================================================*/
ds_pdn_mgr_s* ds_pdn_mgr_get_pdn_ptr
(
  uint8 index
)
{
  return ds_pdn_manager[index];
}
/*===========================================================================
FUNCTION DS_PDN_MGR_READ_NAS_CONFIG_FILE
 
DESCRIPTION
  This function reads NAS config file
 

PARAMETERS 
 
DEPENDENCIES 
  None. 
  
RETURN VALUE 
  Boolean 
 
SIDE EFFECTS 
  None   
===========================================================================*/
boolean ds_pdn_mgr_read_nas_config_file
(
  ds3g_mmgsdi_tag_e_type    tag_id,
  void                      *tag_value_p
)
{
  byte    *efnasconfig = NULL;
  boolean  ret_val = FALSE;


  if ( (ret_val = ds3g_mmgsdi_read_nas_config_file(tag_id, tag_value_p)) 
         == FALSE)
  {
    DS_MSG0(MSG_LEGACY_HIGH, "Try to fetch EF NAS Config Info from EFS");
    
    efnasconfig = ds_pdn_nv_manager_get_efnas_config();

    if (efnasconfig != NULL)
    {
      if(!ds3g_mmgsdi_read_nas_config_tag
           ((uint8 *)efnasconfig, DS_PDN_EFNAS_CONFIG_SIZE, tag_id, tag_value_p))
      {
        DS_ERROR_LOG_1("Requested tag =x was not found, Exit", tag_id);
      }
      else
      {
        ret_val = TRUE;
      }
    }
    else
    {
      DS_ERROR_LOG_0("EFNAS CONFIG is not configured, Exit");
    }
  }

  return ret_val;
} /* ds_pdn_mgr_read_nas_config_file */

/*===========================================================================
FUNCTION  ds_pdn_mgr_get_low_priority_signaling_bit

DESCRIPTION
  The function decides the low priority signaling bit's value.
  ie
  Low priority signaling bit will be set if UE is configured for
  Low priority signaling with the below exceptions
 
  Low priority signaling will not be set for access Class from the SIM is 11-15
   
PARAMETERS
  ds_pdn_mgr_s - PDN context pointer
 
DEPENDENCIES
  None.

RETURN VALUE 
  Boolean
 
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_mgr_get_low_priority_signaling_bit
(
  ds_pdn_mgr_s       *pdn_mgr_p
)
{
  boolean                 low_prio_bit     = FALSE;
  uint32                  efacc_class      = 0;
  sys_plmn_id_s_type      current_plmn_id;
  sys_plmn_id_s_type      hplmn_id;
  uint16                  mcc_hplmn        = 0;
  uint16                  mnc_hplmn        = 0;
  uint16                  mcc_current_plmn = 0;
  uint16                  mnc_current_plmn = 0;
  ds_local_err_val_e_type local_err = DS_LOCAL_ERR_VAL_NONE;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(!pdn_mgr_p)
  {
    return FALSE;
  }
  
  /*---------------------------------------------------------------------- 
    Low priority signaling is supported for only REL 10 and upwards
    ----------------------------------------------------------------------*/
  if(!ds_pdn_nv_manager_get_nv_lte_rel_10())
  {
    DS_ERROR_LOG_0("PRE REL 10, Low priority signaling not supported");
    return FALSE;
  }

  do
  {
    /*------------------------------------------------------------------- 
      Get the low priority signaling bit from MMGSDI.
      If it reurns FALSE
        Try to read the low priority signaling bit from NAS EFS
      IF it returns TRUE
        Check the low priority signaling bit(low_prio_bit)
     
      If the low_prio bit is TRUE only then make other checks else exit out
      of the while loop
      -------------------------------------------------------------------*/

    if (ds_pdn_mgr_read_nas_config_file(DS3G_MMGSDI_TAG_LOW_PRIORITY_SIG, 
                                        (void*)&low_prio_bit) == FALSE)
    {
      low_prio_bit = FALSE;
      local_err = DS_LOCAL_ERR_VAL_0;
      break;
    }

    /*------------------------------------------------------------------- 
      At this point every effort to read the low priority signaling is
      completed, IF low prio bit is FALSE, exit from the while loop
      Only if low priority signaling bit is set then proceed to the
      below conditions to see if the low priority signaling bit can be
      overridden
    -------------------------------------------------------------------*/
    if (!low_prio_bit)
    {
      break;
    }

    /*-------------------------------------------------------------------- 
      No low priority signaling if
       1. If this is an attach request and NAS is responding to
          a page request
       2. AC is between 11-15 (Spl rules apply)
    --------------------------------------------------------------------*/

    if (ds_eps_attach_sm_get_responding_to_page() == TRUE)
    {
      low_prio_bit = FALSE;
      local_err = DS_LOCAL_ERR_VAL_1;
      break;
    }
     
    /*------------------------------------------------------------------- 
      Get the Access class
      If the access class is 12-13-14
          Check the MCC of the Home PLMN and current Serving PLMN
          If match occurs, low_prio_bit cannot be set
      If the access class is 11 or 15
          Check the current PLMN with EHPLMN(if configured),
          If that does not match, check with current HPLMN.
          If either matches no low priority signaling
      -------------------------------------------------------------------*/
    if(ds3g_mmgsdi_get_efacc_class(&efacc_class))
    {
      ds_pdn_hdlr_get_current_plmn(&current_plmn_id);

      ds_pdn_hdlr_get_hplmn_id(&hplmn_id);

      if ((efacc_class & DS_PDN_AC_CLASS_12_to_14) != 0)
      {
        /*-------------------------------------------------------------- 
          Get the MCC of the Home PLMN country and compare it with
          current PLMN's MCC, If a match occurs no low priority signaling
          -------------------------------------------------------------*/
        ds_pdn_hdlr_extract_mcc_mnc_from_plmn_id(&current_plmn_id,
                                             &mcc_current_plmn,
                                             &mnc_current_plmn);

        ds_pdn_hdlr_extract_mcc_mnc_from_plmn_id(&hplmn_id,
                                             &mcc_hplmn,
                                             &mnc_hplmn);

        if((mcc_hplmn == mcc_current_plmn) && (mcc_hplmn != 0))
        {
          low_prio_bit = FALSE;          
          local_err = DS_LOCAL_ERR_VAL_2;
        }

        break;
      }

      if((efacc_class & DS_PDN_AC_CLASS_11_or_15)!= 0)
      {
        DS_MSG0(MSG_LEGACY_HIGH, "AC class is 11 or 15");
        
        /*--------------------------------------------------------- 
          See if current PLMN is a part of EHPLMN list
          If it is, low_prio bit is set to FALSE
          ---------------------------------------------------------*/
        if(ds_pdn_hdlr_is_plmn_in_ehplmn_list(current_plmn_id))
        {
          low_prio_bit = FALSE;
          local_err = DS_LOCAL_ERR_VAL_3;
          break;
        }
        /*---------------------------------------------------------- 
          Now check if the PLMN is a part of Home PLMN
          ----------------------------------------------------------*/
        if(sys_plmn_match(current_plmn_id,hplmn_id))
        {
          low_prio_bit = FALSE;
          local_err = DS_LOCAL_ERR_VAL_4;
          break;
        }
      }
    }

  }while(0);

  DS_MSG2(MSG_LEGACY_HIGH, "low_prio_bit is %d, local_err = %d",
  	       low_prio_bit, local_err);
  
  return low_prio_bit;
}/* ds_pdn_mgr_get_low_priority_signaling_bit */


/*===========================================================================
FUNCTION  DS_PDN_MGR_SYS_IOCTL_CBACK

DESCRIPTION
  The function will be registered with ds_sys_ioctl_registeri_cback() during init
  and gets called when client query for this
   
PARAMETERS
  ioctl_name ioctl name 
  arg_val_ptr used to return thr ewquested value memory for this pointer will be 
              maintained by clients
  ps_error    used to return if there is any error.
DEPENDENCIES
  None.

RETURN VALUE 
  Boolean
 
SIDE EFFECTS
  None.

===========================================================================*/

int16 ds_pdn_mgr_sys_ioctl_cback
(
  ds_sys_ioctl_enum_type   ioctl_name,
  void                     *arg_val_ptr,
  int16                    *ps_errno
)
{
  ds_pdn_mgr_s                  *pdn_mgr_p = NULL;
  boolean                        pdn_mgr_l2_rai_val = FALSE;
  
  switch (ioctl_name)
  {
    case DS_SYS_IOCTL_3GPP_PDN_THROTTLE_INFO:
 	    if(ds_3gpp_pdn_cntx_get_throttle_info(arg_val_ptr, ps_errno) < 0)
      {
        *ps_errno = DS_EINVAL;
         return -1;
      }
      break;
      
    case DS_SYS_IOCTL_3GPP_LTE_ATTACH_PDN_PARAMS:
   	  if(ds_eps_pdn_mgr_get_lte_attach_params(arg_val_ptr, ps_errno) < 0)
   	  {
        *ps_errno = DS_EINVAL;
   		   return -1;
   	  }
   	  break;
   	  
    case DS_SYS_IOCTL_3GPP_PSM_RESTORATION_STATE:
      pdn_mgr_p = ds_pdn_mgr_get_def_pdn();
      if(ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p))
      {
        *(boolean*)arg_val_ptr = pdn_mgr_p->is_restore_mode;
      }
      else
      {
        *(boolean*)arg_val_ptr = FALSE;;
      }
      break;
   	  
     case DS_SYS_IOCTL_3GPP_GET_APN_MSISDN_INFO:
      if(!ds_pdn_mgr_get_msisdn_info(arg_val_ptr, ps_errno))
      {
        *ps_errno = DS_EINVAL;
         return -1;
      }
      break;
     case DS_SYS_IOCTL_3GPP_SET_L2_ENABLE_RAI:
      pdn_mgr_l2_rai_val = ((ds_sys_ioctl_l2_rai_info*)arg_val_ptr)->rai_val;
      if (pdn_mgr_enable_l2_rai != pdn_mgr_l2_rai_val)
      {
        l2_static_enable_rai(pdn_mgr_l2_rai_val);
        pdn_mgr_enable_l2_rai = pdn_mgr_l2_rai_val;
      }
      break;
    default:
      *ps_errno = DS_EINVAL;
      return -1;
  }
  
  return 0;
}/* ds_pdn_mgr_sys_ioctl_cback */

/*===========================================================================
FUNCTION  DS_PDN_MGR_PDN_IOCTL_CBACK

DESCRIPTION
  The function will be registered with FWK during init
  and gets called when client query for this
   
PARAMETERS
  PDN pointer
  IP type
  ioctl_name ioctl name 
  arg_val_ptr used to return thr requested value memory for this pointer will be 
              maintained by clients
  ps_error    used to return if there is any error.
  
DEPENDENCIES
  None.

RETURN VALUE 
  Boolean
 
SIDE EFFECTS
  None.
===========================================================================*/

int ds_pdn_mgr_pdn_ioctl_cback
(
 ds_pdn_mgr_s            *pdn_mgr_p,
 ip_addr_enum_type        ip_type,
 ds_pdn_ioctl_type        ioctl_name,
 void                    *argval_ptr,
 sint15                  *ps_errno
)
{
  ds_local_err_val_e_type                 local_err = DS_LOCAL_ERR_VAL_NONE;
  ds_pdn_mgr_instance_type_e              inst_type = DS_PDN_INSTANCE_MAX;
  ds_pdn_mgr_inst_type_s                 *inst_p = NULL;
  boolean                                 do_silent_redial = FALSE;
  ds_bearer_mgr_s                        *def_bearer = NULL;
  ds_apn_rate_ctrl_context_s             *apn_rate_ctrl_info = NULL;
  ds_pdn_cp_data_splmn_rate_ctrl_info_s  *splmn_rate_ctrl_info = NULL;
  ds_pdn_ioctl_3gpp_get_lte_reestab_calltype_type
                                         *get_reestab_type = NULL;
  ds_pdn_ioctl_3gpp_set_lte_reestab_calltype_type
                                         *set_reestab_type = NULL;
  ds_pdn_cp_data_subseq_data_info_s      *subseq_data_status = NULL;

  inst_type = ds_pdn_mgr_convert_fw_ip_type_to_pdn_inst(ip_type);  
  if( !ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p) ||
  	   inst_type == DS_PDN_INSTANCE_MAX )
  {    
    local_err = DS_LOCAL_ERR_VAL_0;
    goto func_exit;
  }
  	
  DS_MSG3(MSG_LEGACY_HIGH, "Received IOCTL %d for PDN %d , instance type %d", 
  	       ioctl_name, pdn_mgr_p->index, inst_type);

  switch(ioctl_name)
  {
    case DS_PDN_IOCTL_3GPP_FORCE_PDN_DISCONNECT:
    {
      pdn_mgr_p->ims_sip_reg_failure_flag = TRUE;
    	 break;
    }

    case DS_PDN_IOCTL_SET_SILENT_REDIAL:
    {
      /*------------------------------------------------------------- 
              Do Silent redial is extracted from the ioctl
         --------------------------------------------------------------*/
      do_silent_redial = *(boolean*)argval_ptr;
      DS_MSG1(MSG_LEGACY_HIGH, "Do silent redial is %d", do_silent_redial);

      
      if(do_silent_redial)
      {
        
        ps_timer_cancel(pdn_mgr_p->lte_params->apn_param_chg_timer);
      /*------------------------------------------------------------- 
            On receiving the DO_SILENT_REDIAL IOCTL, MH will have to perform 
            silent redial i.e send out a PDN disconnect and bring up the PDN again. 
         --------------------------------------------------------------*/        
        ds_eps_apn_param_change_cmd_hdlr(pdn_mgr_p->pdp_profile_num);
      }
      else
      {       
     /*------------------------------------------------------------- 
         Set silent redial = FALSE indicates the 
         client would tear down the call. MH would not have to worry about 
         tearing down the call. 
         However set the force_ota_teardown to TRUE. 
         Force OTA teardown is used for the last pdn, IF not for this boolean, 
         we will never be able to tear down the call.
        --------------------------------------------------------------*/
        pdn_mgr_p->force_ota_teardown= TRUE;        
      }
      break;	
    }
    
    case DS_PDN_IOCTL_SET_SUBSEQUENT_DATA_TRANSFER_STATUS:
    {
      def_bearer = pdn_mgr_p->bearer_ptr_tbl[DS_3GPP_DEFAULT_BEARER_INDEX];
      if(!ds_bearer_mgr_validate_bearer_ptr(def_bearer) ||
         argval_ptr == NULL)
      {
        local_err = DS_LOCAL_ERR_VAL_1;
        goto func_exit;
      }
      subseq_data_status = (ds_pdn_cp_data_subseq_data_info_s*)argval_ptr;

      DS_MSG3(MSG_LEGACY_HIGH,
              "Received RAI NAT mask %d for RAI %d , NAT %d", 
              subseq_data_status->subseq_data_mask,
              subseq_data_status->rai_val,
              subseq_data_status->next_activity_timer);
      if(subseq_data_status->subseq_data_mask & DS_PDN_CP_DATA_RAI)
      {
        ds_pdn_cp_data_set_release_assistance_ind(def_bearer->index,
                                                  subseq_data_status->rai_val);
      }
      if(subseq_data_status->subseq_data_mask & DS_PDN_CP_DATA_NAT)
      {
        ds_pdn_cp_data_set_next_act_timer_ind(
                                      subseq_data_status->next_activity_timer);
      }
      break;
    }

    case DS_PDN_IOCTL_GET_SUBSEQUENT_DATA_TRANSFER_STATUS:
    {
      def_bearer = pdn_mgr_p->bearer_ptr_tbl[DS_3GPP_DEFAULT_BEARER_INDEX];
      if(!ds_bearer_mgr_validate_bearer_ptr(def_bearer) ||
         argval_ptr == NULL)
      {
        local_err = DS_LOCAL_ERR_VAL_2;
        goto func_exit;
      }

      *(ds_pdn_rai_ind_enum_type*)argval_ptr = 
             ds_pdn_cp_data_get_release_assistance_ind(def_bearer->index);
      break;
    }
    
    case DS_PDN_IOCTL_3GPP_MO_EXCP_IN_USE:
    {
      ds_pdn_apn_rate_ctrl_set_mo_ex_in_use(pdn_mgr_p,
                     *((ds_pdn_ioctl_mo_excp_usage_enum_type*)argval_ptr));
      break;
    }

    case DS_PDN_IOCTL_GET_SPLMN_RATE_CONTROL_INFO:
    {
      def_bearer = pdn_mgr_p->bearer_ptr_tbl[DS_3GPP_DEFAULT_BEARER_INDEX];
      if(!ds_bearer_mgr_validate_bearer_ptr(def_bearer) ||
         argval_ptr == NULL)
      {
        local_err = DS_LOCAL_ERR_VAL_3;
        goto func_exit;
      }
      splmn_rate_ctrl_info = (ds_pdn_cp_data_splmn_rate_ctrl_info_s*)argval_ptr;
      ds_pdn_cp_data_get_splmn_rate_ctrl_info(def_bearer->index,
                                              splmn_rate_ctrl_info);
      splmn_rate_ctrl_info->pkt_counter = 
                    ds_pdn_rate_ctrl_get_current_data_pkt_count(
                                             &splmn_rate_ctrl_info->rate_ctrl_info);
      break;
    }

    case DS_PDN_IOCTL_GET_APN_RATE_CONTROL_INFO:
    {
      def_bearer = pdn_mgr_p->bearer_ptr_tbl[DS_3GPP_DEFAULT_BEARER_INDEX];
      if(!ds_bearer_mgr_validate_bearer_ptr(def_bearer) ||
         argval_ptr == NULL)
      {
        local_err = DS_LOCAL_ERR_VAL_4;
        goto func_exit;
      }

      apn_rate_ctrl_info = 
            ds_pdn_apn_rate_ctrl_get_instance_from_apn_index(
                                                    def_bearer->apn_index);
      if(apn_rate_ctrl_info != NULL)
      {
        apn_rate_ctrl_info->pkt_counter = 
                  ds_pdn_rate_ctrl_get_current_data_pkt_count(
                                             &apn_rate_ctrl_info->rate_ctrl_info);
        *((ds_apn_rate_ctrl_context_s*)argval_ptr) = *apn_rate_ctrl_info;
      }
      else
      {
        local_err = DS_LOCAL_ERR_VAL_5;
      	 goto func_exit;  
      }
      
      break;
    }

    case DS_PDN_IOCTL_3GPP_GET_LTE_REESTAB_CALLTYPE:
    {
      inst_p = pdn_mgr_p->inst_ptr[inst_type];
      if(pdn_mgr_p->lte_params == NULL ||
         inst_type > DS_PDN_INSTANCE_IPV6 ||
         inst_p == NULL ||
         inst_p->state != DS_INSTANCE_STATE_UP)
      {
        local_err = DS_LOCAL_ERR_VAL_6;
      	 goto func_exit;
      }

      get_reestab_type = (ds_pdn_ioctl_3gpp_get_lte_reestab_calltype_type*)
                                                                     argval_ptr;
      get_reestab_type->calltype = 
                       pdn_mgr_p->lte_params->rab_reestab_call_type[inst_type];
      DS_MSG3(MSG_LEGACY_HIGH, "RAB reestab type %d for PDN %d inst %d",
              get_reestab_type->calltype, pdn_mgr_p->index, inst_type);
      break;
    }

    case DS_PDN_IOCTL_3GPP_SET_LTE_REESTAB_CALLTYPE:
    {
      inst_p = pdn_mgr_p->inst_ptr[inst_type];
      if(pdn_mgr_p->lte_params == NULL ||
         inst_type > DS_PDN_INSTANCE_IPV6 ||
         inst_p == NULL ||
         inst_p->state != DS_INSTANCE_STATE_UP)
      {
        local_err = DS_LOCAL_ERR_VAL_7;
      	 goto func_exit;
      }

      set_reestab_type = (ds_pdn_ioctl_3gpp_set_lte_reestab_calltype_type*)
                                                                     argval_ptr;
      if(set_reestab_type->calltype != DS_PDN_3GPP_LTE_REESTAB_CALLTYPE_NORMAL &&
         set_reestab_type->calltype != DS_PDN_3GPP_LTE_REESTAB_CALLTYPE_VOLTE)
      {
        local_err = DS_LOCAL_ERR_VAL_8;
      	 goto func_exit;
      }
      
      pdn_mgr_p->lte_params->rab_reestab_call_type[inst_type] = 
                                                    set_reestab_type->calltype;
      DS_MSG3(MSG_LEGACY_HIGH, "RAB reestab type set to %d for PDN %d inst %d",
              set_reestab_type->calltype, pdn_mgr_p->index, inst_type);
      break;
    }
    
    default:
    	 local_err = DS_LOCAL_ERR_VAL_10;
    	 goto func_exit;
  }

func_exit :
	 if(local_err != DS_LOCAL_ERR_VAL_NONE)
	 {
	   DS_ERROR_LOG_1("ds_pdn_mgr_pdn_ioctl_cback failed with err =d",
	   	              local_err);
	   if(ps_errno != NULL)
	   {
	     *ps_errno = DS_EINVAL;
	   }
    
	   return -1;
	 }
	 
  return 0;
}

/*===========================================================================
  FUNCTION DS_PDN_MGR_CHNG_BYTE_ORDER
  
  DESCRIPTION
    Changes the byte order
    
  PARAMETERS  
    uint32 containing the original byte order
      
  DEPENDENCIES 
    None.
    
  RETURN VALUE 
    unit32 containing the changed (reverse) byte order
     
  SIDE EFFECTS 
    None.
    
===========================================================================*/

uint32 ds_pdn_mgr_change_byte_order 
(
  uint32 orig_byte_order
)
{
  return (((orig_byte_order >> 24) & 0xff) | ((orig_byte_order >> 8) & 0xff00)
         |((orig_byte_order << 8) & 0xff0000) | 
          ((orig_byte_order << 24) & 0xff000000));
} /* ds_pdn_mgr_change_byte_order */

/*===========================================================================

FUNCTION DS_FWK_GET_SIP_SERV_ADDR

DESCRIPTION
  This function returns the list of sip server addresses stored in the iface.

PARAMETERS
  fwk_inst:        ptr to fwk control block on which to operate on.
  sip_serv_addr_ptr_array: pointer to the SIP server addresses array.
  count : as input value it contains the count of the SIP server
          addresses asked for and as a return value it contains the
          actual number of addresses returned.   
  pdn_instance		Instance of the PDN V4 or V6

RETURN VALUE
  0 Success
 -1 Failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean ds_pdn_mgr_set_sip_serv_addr
(
  ds_fwk_s_type                 *fwk_inst,
  ip_addr_type                  *sip_serv_addr_ptr_array,
  uint32                         count,
  ds_pdn_mgr_instance_type_e     pdn_instance
)
{
  int loop = 0;
  ds_fwk_sip_serv_addr_type *sip_addr = NULL;

  if(fwk_inst == NULL ||sip_serv_addr_ptr_array == NULL)
  {
    DS_ERROR_LOG_0("fwk_int or sip_serv_addr_ptr_array is NULL");
    return FALSE;
  }

  /*-------------------------------------------------------------------------
   If one or more pcscf addresses were returned by network, store them in
   the ds instance structure.
  -------------------------------------------------------------------------*/
  if(pdn_instance == DS_PDN_INSTANCE_IPV4 &&  fwk_inst->v4_pdn_ptr != NULL)
  {	 
    DS_MSG1(MSG_LEGACY_HIGH, "recd %d v4_pcscf address", 
                count);
    if(count > 0)
	{ 
    /*-----------------------------------------------------------------------
     ds3gpp_decode_v6_pcscf_pkt() decodes both v4 and v6 pcscf addresses.
     We need to update decode_mask with DS_3GPP_PCSCF_V4_PKT_DECODED here
     since it may have been missed before
	-----------------------------------------------------------------------*/
      for(loop=0; loop<count; loop++)
	  {
        DS_SYSTEM_HEAP_MEM_ALLOC(sip_addr, sizeof(ds_fwk_sip_serv_addr_type),
 									ds_fwk_sip_serv_addr_type*);
        if(sip_addr == NULL)
        {
          DS_ASSERT(0);
          return FALSE;
        }

        memset(sip_addr, 0, sizeof(ds_fwk_sip_serv_addr_type));
  
	    memscpy(&sip_addr->sip_addr,
	               sizeof(ps_ip_addr_type),
                   &sip_serv_addr_ptr_array[loop],
                   sizeof(ps_ip_addr_type));		 
                   list_push_back(&fwk_inst->v4_net_info.sip_serv_addr, &sip_addr->link);
       }	   
     } 		
  return TRUE;
  }
  
  if(pdn_instance == DS_PDN_INSTANCE_IPV6 &&  fwk_inst->v6_pdn_ptr != NULL)
  {	 
    DS_MSG1(MSG_LEGACY_HIGH, "recd %d v6_pcscf address", 
              count);
  
    if(count > 0)
	{	   
	  for(loop=0; loop<count; loop++)
	  {
        DS_SYSTEM_HEAP_MEM_ALLOC(sip_addr, sizeof(ds_fwk_sip_serv_addr_type),
								ds_fwk_sip_serv_addr_type*);
        if(sip_addr == NULL)
        {
          DS_ASSERT(0);
          return FALSE;
        }
		memset(sip_addr, 0, sizeof(ds_fwk_sip_serv_addr_type));
  
		memscpy(&sip_addr->sip_addr,
			   sizeof(ps_ip_addr_type),
			   &sip_serv_addr_ptr_array[loop],
			   sizeof(ps_ip_addr_type));
		list_push_back(&fwk_inst->v6_net_info.sip_serv_addr, &sip_addr->link);
	  }
    }	
  return TRUE;
  }
 return FALSE;
}


/*===========================================================================
FUNCTION      DS3GI_PROCESS_CM_CALL_RESTORE_BEARER_CTXT_RSP_CMD

DESCRIPTION   This function processes the DS_CM_CALL_RESTORE_BEARER_CTXT_RSP_CMD
              from Call Manager. This command is notification of bearer setup
              by network.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_pdn_process_restore_bearer_ctxt_rsp_cmd
(
  ds_command_type  *cmd_ptr
)
{
  emm_restore_bearer_ctxt_rsp_s_type  *restore_bearer_ctxt_ptr = NULL;
  mm_restore_pdp_ctxt_rsp_s_type      *restore_pdp_ctxt_ptr = NULL;
  emm_act_default_bearer_context_request_ind_s_type
                                      *activate_default_beaer_req= NULL;
  mm_pdp_activate_cnf_T               *restore_bearer_res_ptr = NULL;
  uint8                                next_psm_index = 0;
  uint8                                psm_index = 0;
  ds_pdn_psm_info_s                   *psm_info_p = NULL;
  sys_sys_mode_e_type                  call_mode = SYS_SYS_MODE_NONE;
  boolean                              ret_val = TRUE;
#ifdef FEATURE_NBIOT_NTN
  boolean                              is_ntn_ind= FALSE;
#endif /* FEATURE_NBIOT_NTN */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
   {
     ASSERT(0);
     return;
   }

   if (ds_pdn_psm_get_ulog_handle() != NULL)
   {
     ULOG_RT_PRINTF_0(ds_pdn_psm_get_ulog_handle(),
            "ds_pdn_process_cm_call_restore_bearer_ctxt_rsp_cmd processing ");
   }

   restore_bearer_ctxt_ptr = 
                  (emm_restore_bearer_ctxt_rsp_s_type*)cmd_ptr->cmd_payload_ptr;
   #ifdef FEATURE_NBIOT_NTN
   is_ntn_ind = restore_bearer_ctxt_ptr->ntn_ctxt_restore;
   #endif /* FEATURE_NBIOT_NTN */
   psm_index = ds_pdn_psm_info_find_current_index();
   if (psm_index == DS_3GPP_MAX_BEARER_CONTEXT) 
   {
     ret_val = FALSE;
     goto func_exit;
   }

   psm_info_p = ds_pdn_psm_get_info_ptr(psm_index);
   if(!ds_pdn_psm_info_validate(psm_info_p))
   {
     ret_val = FALSE;
     goto func_exit;
   }

   call_mode = psm_info_p->call_mode;
    
   /*-------------------------------------------------------------------------
     At this point, the activate bearer ind has been processed by the mode-
     specific handler.  Do next action based on results of mode-specific
     handler return value.
   -------------------------------------------------------------------------*/
   if (call_mode == SYS_SYS_MODE_LTE)
   {
      restore_bearer_ctxt_ptr = 
                  (emm_restore_bearer_ctxt_rsp_s_type*)cmd_ptr->cmd_payload_ptr;

     DS_SYSTEM_HEAP_MEM_ALLOC(activate_default_beaer_req,
                    sizeof(emm_act_default_bearer_context_request_ind_s_type),
                    emm_act_default_bearer_context_request_ind_s_type *);
  
     if (activate_default_beaer_req == NULL)
     {
       DS_MSG0(MSG_LEGACY_ERROR,"Memory Allocation Failed.");
       ret_val = FALSE;
       goto func_exit;
     }

     memset(activate_default_beaer_req, 0,
                     sizeof(emm_act_default_bearer_context_request_ind_s_type));
     activate_default_beaer_req->eps_bearer_id =
                      restore_bearer_ctxt_ptr->eps_bearer_id;
   
     ds_pdn_psm_fill_bearer_response_info_lte(activate_default_beaer_req);
 
     ret_val = ds_eps_pdn_mgr_default_bearer_activate_ind
                                             (activate_default_beaer_req);
   }
   else if(call_mode == SYS_SYS_MODE_GSM  
           #ifdef FEATURE_NBIOT_NTN 
                && (is_ntn_ind == FALSE)
           #endif /* FEATURE_NBIOT_NTN */
           )
   {
     restore_pdp_ctxt_ptr =
                 (mm_restore_pdp_ctxt_rsp_s_type*)cmd_ptr->cmd_payload_ptr;
     DS_SYSTEM_HEAP_MEM_ALLOC(restore_bearer_res_ptr,
                    sizeof(mm_pdp_activate_cnf_T),
                    mm_pdp_activate_cnf_T *);
  
     if (restore_bearer_res_ptr == NULL)
     {
       DS_MSG0(MSG_LEGACY_ERROR,"Memory Allocation Failed.");
       ret_val = FALSE;
       goto func_exit;
     }

     memset(restore_bearer_res_ptr, 0, sizeof(mm_pdp_activate_cnf_T));

     restore_bearer_res_ptr->nsapi = restore_pdp_ctxt_ptr->nsapi;
     restore_bearer_res_ptr->connection_id = restore_pdp_ctxt_ptr->connection_id;

     ds_pdn_psm_fill_bearer_response_info_gsm(restore_bearer_res_ptr);
 
     ret_val = ds_gprs_mgr_pdp_activate_cnf_hdlr (restore_bearer_res_ptr);
     
   }

   if(ret_val == TRUE)
   {
     ds_pdn_psm_set_psm_index_state(psm_index,
                                       DS_PDN_PSM_INFO_DONE);
     next_psm_index = ds_pdn_psm_info_find_next_index();
  
     if(next_psm_index == DS_3GPP_MAX_BEARER_CONTEXT) 
     {
       ds_pdn_psm_set_restoration_state(TRUE);
       
       if (ds_pdn_psm_get_ulog_handle() != NULL)
       {
         ULOG_RT_PRINTF_0(ds_pdn_psm_get_ulog_handle(),
                 "ds_pdn_process_cm_call_restore_bearer_ctxt_rsp_cmd sending"
                 "response success ");
       }
       ds_pdn_process_restore_context_response(TRUE, call_mode
                                         #ifdef FEATURE_NBIOT_NTN
                                             ,is_ntn_ind
                                         #endif /* FEATURE_NBIOT_NTN */
                                              );
     }
     else
     {
       if(ds_pdn_psm_restore_bearer_context_request(next_psm_index) == FALSE)
       {
         ret_val = FALSE;
       }
     }
   }

func_exit:
   if (ret_val != TRUE) 
   {
     if(psm_index < DS_3GPP_MAX_BEARER_CONTEXT)
     {
       ds_pdn_psm_set_psm_index_state(psm_index,
                                      DS_PDN_PSM_INFO_DONE);
     }
     if (ds_pdn_psm_get_ulog_handle() != NULL)
     {
       ULOG_RT_PRINTF_0(ds_pdn_psm_get_ulog_handle(),
                        "ds_pdn_process_cm_call_restore_bearer_ctxt_rsp_cmd "
                        "sending response failure ");
     }
      
     ds_pdn_process_restore_context_response(FALSE, call_mode
                                         #ifdef FEATURE_NBIOT_NTN
                                             ,is_ntn_ind
                                         #endif /* FEATURE_NBIOT_NTN */
                                              );
   }

   if (activate_default_beaer_req)
   {
     DS_SYSTEM_HEAP_MEM_FREE(activate_default_beaer_req);
   }

   if (restore_bearer_res_ptr)
   {
     DS_SYSTEM_HEAP_MEM_FREE(restore_bearer_res_ptr);
   }

   return;
}

#ifdef FEATURE_NBIOT_NTN

/*===========================================================================
FUNCTION      DS_PDN_MGR_SET_NTN_RESTORATION_IN_PROGRESS

DESCRIPTION   This function will set if NTN restoration is in progress.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_pdn_mgr_set_ntn_restoration_in_progress
(
  boolean  flag
)
{
  is_ntn_restoration_in_progress = flag;
  return;
}
/*===========================================================================
FUNCTION      DS_PDN_MGR_IS_NTN_RESTORATION_IN_PROGRESS

DESCRIPTION   This function will return if NTN is in progress or not

DEPENDENCIES  None

RETURN VALUE  TRUE/FALSE

SIDE EFFECTS  None
===========================================================================*/
boolean  ds_pdn_mgr_is_ntn_restoration_in_progress( void )
{
  return is_ntn_restoration_in_progress;
}
#endif /* FEATURE_NBIOT_NTN */

/*==============================================================================
FUNCTION DS_PDN_MGR_GET_RESOLVEDAPN_FOR_DEFAULTPDN

DESCRIPTION
  This function is used to return the resolved apn name for default 
  pdn context

PARAMETERS   :
  char** - Resolved APN name as returned by the network

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
==============================================================================*/
void ds_pdn_mgr_get_resolvedapn_for_defaultpdn(char ** resolved_apn)
{
  ds_pdn_mgr_s *pdn_mgr_p = NULL;

  if(resolved_apn == NULL)
  {
    DS_MSG0(MSG_LEGACY_HIGH,"Resolved apn address is NULL");
    return;
  }
  pdn_mgr_p = ds_pdn_mgr_get_def_pdn();
  if(pdn_mgr_p != NULL && strlen(pdn_mgr_p->pdp_profile->context.apn) == 0)
  {
    ds_apn_table_get_resolved_apn_name(pdn_mgr_p,resolved_apn);
  }
  return;
}

/*===========================================================================
FUNCTION      DS_PDN_MGR_SET_CM_EV_MASK

DESCRIPTION   This function sets event mask for various event types like SS,
              PH & CALL events.

PARAMETERS    CM event Type.
              bit256 mask pointer to be set.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds_pdn_mgr_set_cm_ev_mask
(
  dssysmgr_cm_event_type event,
  bit_mask_256_t *mask_ptr
)
{
  ds_local_err_val_e_type local_err = DS_LOCAL_ERR_VAL_NONE;
  
  if (mask_ptr == NULL)
  {
    local_err = DS_LOCAL_ERR_VAL_0;
    goto func_exit;
  }

  if (event < DSSYSMGR_EVT_TYPE_MIN || event >= DSSYSMGR_EVT_TYPE_MAX)
  {
    local_err = DS_LOCAL_ERR_VAL_1;
    goto func_exit;
  }

  switch(event)
  {
    case DSSYSMGR_EVT_TYPE_SS:
    {
      bit_mask_256_set_bit(mask_ptr,CM_SS_EVENT_SRV_CHANGED);
      bit_mask_256_set_bit(mask_ptr,CM_SS_EVENT_INFO);
    }
    break;
    case DSSYSMGR_EVT_TYPE_PH:
    {
      bit_mask_256_set_bit(mask_ptr, CM_PH_EVENT_OPRT_MODE);
      bit_mask_256_set_bit(mask_ptr, CM_PH_EVENT_SYS_SEL_PREF);
      bit_mask_256_set_bit(mask_ptr, CM_PH_EVENT_INFO);
      bit_mask_256_set_bit(mask_ptr, CM_PH_EVENT_DUAL_STANDBY_PREF);
      bit_mask_256_set_bit(mask_ptr, CM_PH_EVENT_DATA_PRIORITY);
      bit_mask_256_set_bit(mask_ptr, CM_PH_EVENT_SUBSCRIPTION_AVAILABLE);
      bit_mask_256_set_bit(mask_ptr, CM_PH_EVENT_SUBSCRIPTION_NOT_AVAILABLE);	  
    }
    break;
    default:
      break;
  }

func_exit:
  if (local_err != DS_LOCAL_ERR_VAL_NONE)
  {
    DS_MSG1(MSG_LEGACY_HIGH," ds_pdnmgr_set_cm_ev_mask : err %d", local_err);
  }
  return;
}/* ds_pdnmgr_set_cm_ev_mask_mh */

/*===========================================================================
FUNCTION      DS_PDN_MGR_NOTIFY_CB

DESCRIPTION   Call back function from Sysmgr for CM events

PARAMETERS    CM event Type.
              CM event common info.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds_pdn_mgr_notify_cb
(
  dssysmgr_cm_event_type ev_type,
  dssysmgr_cm_common_event_info cm_info
)
{
  switch(ev_type)
  {
    case DSSYSMGR_EVT_TYPE_PH:
      ds_pdn_mgr_handle_ph_ev_cb(cm_info.ph_ev_info.ph_event,
                                cm_info.ph_ev_info.ph_info_ptr);
      break;
    case DSSYSMGR_EVT_TYPE_SS:
    case DSSYSMGR_EVT_TYPE_CALL:
      break;
    default:
      break;
  }
  return;
}

/*===========================================================================
FUNCTION      DS_PDN_MGR_HANDLE_PH_EV_CB

DESCRIPTION   Call back function from Sysmgr for CM PH events


PARAMETERS    PH event Type.
              PH info.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds_pdn_mgr_handle_ph_ev_cb
(
  cm_ph_event_e_type         event,
  const cm_ph_info_s_type   *event_ptr
)
{
  ds_command_type *cmd_ptr = NULL;
  dssysmgr_ph_event_info_type *ph_ev_info_ptr = NULL;

  cmd_ptr = ds_allocate_cmd_buf(sizeof(dssysmgr_ph_event_info_type));
  if((cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    DS_MSG0(MSG_LEGACY_ERROR," Unable to allocate cmd ptr ");
    return;
  }
  
  cmd_ptr->hdr.cmd_id = DS_CMD_PDNMGR_CM_PH_EV;
  
  ph_ev_info_ptr = (dssysmgr_ph_event_info_type*)cmd_ptr->cmd_payload_ptr;
  
  ph_ev_info_ptr->ph_event           = event;
  ph_ev_info_ptr->srv_domain_pref    = event_ptr->srv_domain_pref;
  ph_ev_info_ptr->oprt_mode          = event_ptr->oprt_mode;
  ph_ev_info_ptr->mode_pref          = event_ptr->mode_pref;
  ph_ev_info_ptr->ph_state           = event_ptr->ph_state;
  ph_ev_info_ptr->mode_capability    = event_ptr->mode_capability;
  ph_ev_info_ptr->band_capability    = event_ptr->band_capability;
  ph_ev_info_ptr->lte_band_capability= event_ptr->lte_band_capability;
  
  ds_put_cmd( cmd_ptr );
  return;
}

/*===========================================================================
FUNCTION      DS_PDN_MGR_PH_EV_CMD_HDLR

DESCRIPTION   

PARAMETERS    CM event Type.
              CM event common info.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds_pdn_mgr_ph_ev_cmd_hdlr(ds_command_type  * cmd_ptr)
{
  dssysmgr_ph_event_info_type*  ph_info_ptr = NULL;
  ds_local_err_val_e_type       local_err = DS_LOCAL_ERR_VAL_NONE;
  
  if((cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL))
  {
    local_err = DS_LOCAL_ERR_VAL_0;
    goto func_exit;
  }

  ph_info_ptr = (dssysmgr_ph_event_info_type*)cmd_ptr->cmd_payload_ptr;
  switch(ph_info_ptr->ph_event)
  {
    case CM_PH_EVENT_OPRT_MODE:
    case CM_PH_EVENT_SYS_SEL_PREF:
    case CM_PH_EVENT_INFO:
    case CM_PH_EVENT_DUAL_STANDBY_PREF:
    case CM_PH_EVENT_DATA_PRIORITY:
    case CM_PH_EVENT_SUBSCRIPTION_AVAILABLE:
    case CM_PH_EVENT_SUBSCRIPTION_NOT_AVAILABLE:
      if((dssysmgr_get_oprt_mode() == SYS_OPRT_MODE_LPM) ||
       (dssysmgr_get_oprt_mode() == SYS_OPRT_MODE_OFFLINE))
      {
        ds_pdn_mgr_handle_lpm_mode_change(ph_info_ptr->oprt_mode);
      }
      else if((dssysmgr_get_oprt_mode() == SYS_OPRT_MODE_ONLINE))
      {
        if(ds_pdn_nv_manager_is_iot_rpm_enabled())
        {
          ds_3gpp_pdn_throt_advertise_blocked_apns_current_plmn();
          ds_3gpp_pdn_cntxt_disable_lte_if_attach_profile_unavailable();
        }
      }
      break;
    default:
      break;
  }
func_exit:
  if (local_err != DS_LOCAL_ERR_VAL_NONE)
  {
    DS_MSG1(MSG_LEGACY_HIGH," ds_pdn_mgr_ph_ev_cmd_hdlr : err %d", local_err);
  }
  return;
}

/*===========================================================================
FUNCTION DS_PDN_MGR_APN_PARAM_CHANGE_CMD_HDLR

DESCRIPTION
  This function detects if any of following profile params has changed or not
  1.  APN disabled flag
  2.  APN Name
  3.  Maximum PDN connections per block
  4.  Maximum PDN connection wait timer
  5.  PDN request wait timer
  6.  APN bearer
  7.  APN class
  8.  PDP Type
 
  If any of those above params have changed then it calls another which handles
  PDN disconnect processing if any PDN is up with the changed profile
 
PARAMETERS 
  prof_num:                 Profile whose contents have changed
  subs_id                   Profile Subscription ID
  prf_data:                 Profile specific data (mask, new values ....)
 
DEPENDENCIES
  None.

RETURN VALUE
  void 
 
SIDE EFFECTS
  None.
===========================================================================*/
void ds_pdn_mgr_apn_param_change_cmd_hdlr
(
  uint16                           prof_num,
  ds_profile_type_s                prof
)
{
  sys_sys_mode_e_type                  current_sys_mode = SYS_SYS_MODE_MAX;
  ds_profile_pdp_type_e                new_pdp_type = DS_PROFILE_PDP_IPV4;
  ds_profile_pdp_type_e                old_pdp_type = DS_PROFILE_PDP_IPV4;
  char                                 *new_apn = NULL;  
  char                                 *old_apn = NULL;   
  ds_pdn_mgr_s                         *pdn_context_p = NULL;
  uint8                                pdn_index = 0;      
  boolean                              apn_change = FALSE;
  boolean                              pdp_change = FALSE;
  boolean                              pdn_state_valid = FALSE;
  boolean                              delay_teardown = FALSE;
/*---------------------------------------------------------------------------*/

  /*------------------------------------------------------------------------- 
   For non LTE modes do not process the request
  -------------------------------------------------------------------------*/

  if((current_sys_mode = ds_pdn_hdlr_get_current_network_mode()) != 
       SYS_SYS_MODE_LTE)
  {
    DS_MSG1(MSG_LEGACY_HIGH,"Current SYS mode is %d, APN param change feature"
                             " disabled", current_sys_mode);
    return;
  }


  /*-------------------------------------------------------------------------- 
    Check if the changed apn parameters are one of
    1. apn_name
    2. pdp_type.
    If they are then delay proceesing. Send an apn_param_chg ind to the
    framework and allow the client to tear down the call. Also start a 5s timer
    If the timer expires then continue with the normal silent redial.
  -------------------------------------------------------------------------*/

  /*------------------------------------------------------------------------ 
    Get the profile data from the cache
  ------------------------------------------------------------------------*/

  new_pdp_type = prof.context.pdp_type;
  new_apn =  (char *)prof.context.apn;

  for(pdn_index = 0; pdn_index < DS_3GPP_MAX_PDN_CONTEXT; pdn_index++)
  {
    pdn_context_p = ds_pdn_mgr_get_pdn_ptr_from_index(pdn_index);
    pdn_state_valid = FALSE;
    delay_teardown = FALSE;

    /*---------------------------------------------------------------- 
      PDN state is valid only if it is UP or COMING_UP state
    ----------------------------------------------------------------*/
    if(ds_pdn_mgr_validate_pdn_ptr(pdn_context_p) &&
       ((pdn_context_p->state == DS_PDN_MGR_STATE_UP)  ||
         (pdn_context_p->state == DS_PDN_MGR_STATE_COMING_UP)))
    { 
      pdn_state_valid = TRUE;
      DS_MSG0(MSG_LEGACY_HIGH,"PDN state is valid");
    }

    /*------------------------------------------------------------------ 
      Ensure that the pdn context's profile num match occurs
      and the state is up.
      Also if apn_param_chg_teardown is set to TRUE ignore because MH
      is already processing a silent redial.
      Also if the pdn_context is in an Emergency call simply continue
      Also if the timer is already running, no need to do anything 
    -----------------------------------------------------------------*/
    if(!pdn_state_valid ||
       (pdn_context_p->pdp_profile_num != prof_num) ||
       (pdn_context_p->lte_params->apn_param_chg_teardown))
    {
      continue;
    }

    if(ps_timer_is_running(pdn_context_p->lte_params->apn_param_chg_timer))
    {
      DS_ERROR_LOG_0("apn block timer already running not starting again");
      continue;
    }
   
    /*------------------------------------------------------------------- 
      Extract pdp_type from user_data because pdn_context's pdp type
      can be over written
    -------------------------------------------------------------------*/
    old_pdp_type = pdn_context_p->pdp_profile->context.pdp_type;
    old_apn = (char*)pdn_context_p->pdp_profile->context.apn;

    if((strncasecmp((char*)old_apn,(char*)new_apn,
                        DS_PROFILE_APN_STRING_LEN+1)!= 0))
    {
      apn_change = TRUE;
    }

    if(old_pdp_type != new_pdp_type)
    {
      pdp_change = TRUE;
    }

    /*------------------------------------------------------------------------ 
      The 5 second timer will be started in the below cases
      1. Pdp change occurs
      2. APN chnage occurs
      3. PDN context state is COMING_UP --> Delay tearing down and bringing
      up the pdn context until the timer expires(5s). This will hopefully give
      enough time for the PDN context to change to UP or DOWN.
    ------------------------------------------------------------------------*/
    if((pdp_change) || (apn_change) || (pdn_context_p->state
                                         == DS_PDN_MGR_STATE_COMING_UP))
    {
      DS_MSG3(MSG_LEGACY_HIGH,"Delay APN param change teardown by 5 secs"
                        "pdp_change:%d,apn_change:%d,"
                        "pdn_state:%d",pdp_change,apn_change,
                        pdn_context_p->state);
      delay_teardown = TRUE;
    }
    

    /*------------------------------------------------------------------------ 
      If this stage is reached it means that there is no apn_name or pdp_type
      change
      Now check for all the other apn params to see if really an apn parameter
      changed
    -----------------------------------------------------------------------*/
    if(((pdp_change == FALSE) && (apn_change == FALSE)) &&
        (pdn_context_p->pdp_profile->apn_disable_flag ==
         prof.apn_disable_flag) &&
        (pdn_context_p->pdp_profile->apn_class ==
         prof.apn_class) &&
         (pdn_context_p->pdp_profile->apn_bearer == 
         prof.apn_bearer) &&
         (pdn_context_p->pdp_profile->pdn_req_wait_time == 
          prof.pdn_req_wait_time) &&
         (pdn_context_p->pdp_profile->max_pdn_conn_per_blk ==
          prof.max_pdn_conn_per_blk) &&
         (pdn_context_p->pdp_profile->max_pdn_conn_time ==
          prof.max_pdn_conn_time))
    {
      DS_MSG0(MSG_LEGACY_HIGH,"No Real APN Param chg, Return");
      return;
    }

    /*------------------------------------------------------------------------ 
      Start the timer if delay_teardown flag is set. If we reach here, it 
      means that any of the APN params have changed.
    ------------------------------------------------------------------------*/
    if(delay_teardown)
    {
      DS_MSG0(MSG_LEGACY_HIGH,"Starting delay teardown timer as apn param has changed");
      
      /*------------------------------------------------------------------------
        Start the timer interval 
      ------------------------------------------------------------------------*/
      (void)ps_timer_start(pdn_context_p->lte_params->apn_param_chg_timer,
                            DS_PDN_APN_PARAM_CHG_INTERVAL);
      return;
    }
 }

  DS_MSG0(MSG_LEGACY_HIGH,"APN param chg cmd processed");
#ifdef FEATURE_DATA_LTE
  ds_eps_apn_param_change_cmd_hdlr(prof_num);
#endif /* FEATURE_DATA_LTE */
}

/*===========================================================================
FUNCTION DS_PDN_MGR_PROCESS_APN_PARAM_CHG_TMR_EXP_CMD

DESCRIPTION
The function is called when the apn param chg timer expires, This would imply that 
the client will not tear down the call. IT will be MH's responsibility 
to clean up the call. 

PARAMETERS
  pdn_index - Pointer to the PDN context to be torn down

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void ds_pdn_mgr_process_apn_param_chg_tmr_exp_cmd(uint8  pdn_index)
{
  ds_pdn_mgr_s *pdn_mgr_ptr = NULL;
/*..................................................................*/
  DS_MSG0(MSG_LEGACY_HIGH,"In apn_param_chg_tmr_exp_cmd");
  pdn_mgr_ptr = ds_pdn_mgr_get_pdn_ptr_from_index(pdn_index);
 
  if(ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_ptr))
  {
    ds_eps_apn_param_change_cmd_hdlr(pdn_mgr_ptr->pdp_profile_num);
  }
  else
  {
    DS_ERROR_LOG_0("pdn-ptr_is_not valid");
  }
  return;
}

/*==============================================================================
FUNCTION DS_PDN_MGR_GET_ALL_PDN_CONTEXT_PROFILES

DESCRIPTION
  This function returns the list of all primary Profile number that are 
  currently active.

PARAMETERS   :
  profile_list_ptr  - list of profile numbers which are active.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
==============================================================================*/
void ds_pdn_mgr_get_all_pdn_context_profiles
(
  ds_prim_profile_list_type * profile_list_ptr
)
{
  uint8 loop_index = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(profile_list_ptr != NULL);
  memset(profile_list_ptr,0x0,sizeof(struct ds_rmsm_ati_prim_profile_list_type));
  /*-------------------------------------------------------------------------
    Iterate through the pdn context table and update profile_list
    with all PDN profile number which are in active state..
  -------------------------------------------------------------------------*/
  for(loop_index = 0; loop_index < DS_3GPP_MAX_PDN_CONTEXT; loop_index++)
  {
    if (ds_pdn_mgr_validate_pdn_ptr(ds_pdn_manager[loop_index]) &&
       (DS_PDN_MGR_STATE_DOWN != ds_pdn_manager[loop_index]->state )&&
       (DS_PDN_MGR_STATE_MAX != ds_pdn_manager[loop_index]->state ))
    {
      profile_list_ptr->data_ptr[profile_list_ptr->data_len++] = 
                          ds_pdn_manager[loop_index]->pdp_profile_num;
    }
  }
}

/*===========================================================================
FUNCTION ds_pdn_mgr_profile_change_cb

DESCRIPTION
  The function is called when the profile param changes. CB is recieved
  from profile module, so post a command to DS task.

PARAMETERS
  Profile number
  Mak for changed parameters

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_mgr_profile_change_cb
(
  uint8                         profile_num,
  uint64                        mask,
  boolean                       apn_param_changed
)
{
  ds_command_type              *cmd_ptr = NULL;
  ds_pdn_mgr_profile_change_s  *data_ptr = NULL;
/*-------------------------------------------*/

  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_pdn_mgr_profile_change_s));
  if(cmd_ptr == NULL || cmd_ptr->cmd_payload_ptr == NULL)
  {
    return;
  }

  cmd_ptr->hdr.cmd_id = DS_CMD_PDN_PROFILE_PARAM_CHG_CMD;
  data_ptr = (ds_pdn_mgr_profile_change_s*)cmd_ptr->cmd_payload_ptr;

  data_ptr->profile_num = profile_num;
  data_ptr->mask = mask;
  data_ptr->apn_param_changed = apn_param_changed;

  ds_put_cmd(cmd_ptr);
}

/*===========================================================================
FUNCTION DS_PDN_MGR_PROCESS_APN_DISABLED_CMD
 
DESCRIPTION
   This function is used to handle the event when the admin APN is disabled or
  enabled in the profile.
            LTE is disabled even if one APN in the min APN list is disabled
  Similarly LTE enable is called all the APN in min APN list is enabled
 

PARAMETERS 
  profile_num

DEPENDENCIES 
  
RETURN VALUE 
  void
 
SIDE EFFECTS 
  
===========================================================================*/
void ds_pdn_mgr_process_apn_disabled_cmd
(
  uint8              profile_num,
  ds_profile_type_s  prof_info
)
{
  uint16                              profiles_list
                                        [DS_PDN_CFG_PROFILES_MAX + 1];
  int                                 num_profiles=0;
  uint8                               profile=0;
  uint8                               index=0;
  int                                 disabled_profile_cnt=0;
  boolean                             profile_match=FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    If APN Disabled flag is set to TRUE, we will throttle the APN.
    We will also tear down any PDN Connection which is active using that
    profile.
   
    If APN Disabled is set to FALSE, we will unthrottle the APN. 
  -------------------------------------------------------------------------*/
  if (prof_info.apn_disable_flag)
  {
    ds_3gpp_pdn_throt_set_global_throt_mask
      ((byte*)prof_info.context.apn, DS_3GPP_PDN_THROT_APN_DISABLED_MASK);

    ds_3gpp_pdn_throt_advertise_blocked_apns_current_plmn();

    ds_3gpp_pdn_cntxt_disable_lte_if_attach_profile_unavailable();

    ds_pdn_mgr_teardown_pdn_per_profile
        (profile_num, DS_NET_DOWN_REASON_INTERNAL_APN_DISABLED);
  }
  else
  {
    ds_3gpp_pdn_throt_unblock_apn_disabled_throttling((byte*)prof_info.context.apn);
  }

  /*-------------------------------------------------------------------------
    Read the Min APN list to check if the disabled APN is present in it.
  -------------------------------------------------------------------------*/
  if(ds_pdn_nv_manager_read_efs_nv(
                      "/nv/item_files/modem/data/3gpp/lteps/profile_list",
                      (unsigned char *) &profiles_list[0],
                      (sizeof(uint16) * DS_PDN_CFG_PROFILES_MAX)) 
                                                            != NV_DONE_S)                           
  {
    return;
  }
  
  num_profiles = profiles_list[0];
  if((num_profiles == 0) || (num_profiles >= DS_PDN_CFG_PROFILES_MAX))
  {
    return;
  }
  
  for(index = 1; index <= num_profiles; index++)
  {
    profile = profiles_list[index];

    if(profile == profile_num)
    {
      profile_match = TRUE; // used in case of apn_disable_flag
    }
    
    if(prof_info.apn_disable_flag == TRUE)
    {
      disabled_profile_cnt++; //disable_profile_cnt ;
    }
  } //loop through the min apn list

  DS_MSG2(MSG_LEGACY_HIGH, "profile %d, disabled_profile_cnt %d",
          profile_num, disabled_profile_cnt);
  
  if(profile_match)
  {
    if(prof_info.apn_disable_flag)
    {
      ds3g_msh_disable_lte_selection_ex();
    }
    else if( prof_info.apn_disable_flag == FALSE && disabled_profile_cnt == 0)
    {
      ds3g_msh_enable_lte_selection_ex();
    }
  }
  
  return;
} /*ds_3gpp_process_apn_disabled_cmd*/

/*===========================================================================
FUNCTION DS_PDN_MGR_APN_NAME_CHG_HDLR
 
DESCRIPTION
  This function is invoked when the APN name is changed.

PARAMETERS 
  Profile number
  
DEPENDENCIES 
  None. 
  
RETURN VALUE 
  void
 
SIDE EFFECTS 
  
===========================================================================*/
void ds_pdn_mgr_apn_name_chg_hdlr
(
  uint8                     profile_num,
  ds_profile_type_s         prof_info 
)
{
  ds_pdn_apm_apn_change_cmd_process(profile_num);
   
  ds_pdn_roaming_process_disallowed_flag_chg_cmd(profile_num, 
                                                 prof_info.roaming_disallowed);

  ds_pdn_mgr_process_apn_disabled_cmd(profile_num, prof_info);
  
  if (ds_pdn_apm_is_apn_in_attach_pdn_list((char*)prof_info.context.apn)) 
  {
    ds_3gpp_pdn_cntxt_enable_lte_each_plmnid();
  }
  
  return;
}

/*===========================================================================
FUNCTION ds_pdn_mgr_profile_change_cmd_hdlr

DESCRIPTION
  The function is called when the profile param changes as 
  cmd processing in DS task.

PARAMETERS
  Changed profile param staructure

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_mgr_profile_change_cmd_hdlr
(
  ds_pdn_mgr_profile_change_s  *change_ptr
)
{
  ds_profile_type_s   prof_info = {{0}};
  reattempt_ind_T     reattempt_indicator = {0};
  uint8               index = 0;
/*-------------------------------------------*/

  if(change_ptr == NULL)
  {
    return;
  }

  if(ds_profile_setting_from_profile_id(change_ptr->profile_num, &prof_info) != 
                                          DS_PROFILE_RESULT_SUCCESS)
  {
    return;
  }

  // Handle all paramater specific change handling
  
  if(change_ptr->mask & DS_PROFILE_APN_NAME)
  {
    ds_pdn_mgr_apn_name_chg_hdlr(change_ptr->profile_num, prof_info);
  }

  if(change_ptr->mask & DS_PROFILE_TYPE)
  {
    (void)ds_3gpp_network_override_hdlr_set_overridden_ip
                        ((byte *)prof_info.context.apn,
                         DS_3GPP_PDN_THROT_PDP_UNDEFINED,
                         DS_3GPP_PDN_THROT_NTW_O_RIDE_CAUSE_MAX,
                         DS_3GPP_THROT_RAT_MASK_LTE|DS_3GPP_THROT_RAT_MASK_UMTS,
                         reattempt_indicator);
  }

  if(change_ptr->mask & DS_PROFILE_APN_DISABLE_FLAG)
  {
    ds_pdn_mgr_process_apn_disabled_cmd(change_ptr->profile_num, prof_info);
    ds_pdn_apm_update_apn_disable_flag(change_ptr->profile_num,
                                       prof_info.apn_disable_flag);
  }
  
  if(change_ptr->mask & DS_PROFILE_MAX_PDN_CONN_PER_BLOCK)
  {
    if((ds_3gpp_pdn_limit_is_enabled()) &&
       ((index = ds_3gpp_pdn_limit_tbl_find_entry(
               (byte*)prof_info.context.apn)) != 
                    DS_3GPP_PDN_LIMIT_TBL_INVALID_ENTRY))
    {
      ds_3gpp_pdn_limit_update_max_pdn_conn_per_blk(index,
                                               prof_info.max_pdn_conn_per_blk);
      ds_3gpp_pdn_limit_reset_pdn_conn_cntr(index);
      ds_3gpp_pdn_limit_start_pdn_conn_tmr(index);
    }
  }
  
  if(change_ptr->mask & DS_PROFILE_MAX_PDN_CONN_TIMER)
  {
    if((ds_3gpp_pdn_limit_is_enabled()) &&
       ((index = ds_3gpp_pdn_limit_tbl_find_entry(
               (byte*)prof_info.context.apn)) != 
                    DS_3GPP_PDN_LIMIT_TBL_INVALID_ENTRY))
    {
      ds_3gpp_pdn_limit_update_max_pdn_conn_time(index,
                                               prof_info.max_pdn_conn_time);
      ds_3gpp_pdn_limit_reset_pdn_conn_cntr(index);
      ds_3gpp_pdn_limit_start_pdn_conn_tmr(index);
    }
  }
  
  if(change_ptr->mask & DS_PROFILE_PDN_REQ_WAIT_TIMER)
  {
    if((ds_3gpp_pdn_limit_is_enabled()) &&
       ((index = ds_3gpp_pdn_limit_tbl_find_entry(
               (byte*)prof_info.context.apn)) != 
                    DS_3GPP_PDN_LIMIT_TBL_INVALID_ENTRY))
    {
      ds_3gpp_pdn_limit_update_pdn_req_wait_time(index, 
                                                 prof_info.pdn_req_wait_time);

      /* Check whether PDN Wait Timer is running. 
         We restart the timer with the new value, only if it is 
         already running */
      if(ds_3gpp_pdn_limit_is_pdn_req_wait_timer_in_progress(index))
      {
        ds_3gpp_pdn_limit_start_pdn_wait_tmr(index);
      }
    }
  }

  if(change_ptr->mask & DS_PROFILE_ROAMING_DISALLOWED)
  {
    ds_pdn_roaming_process_disallowed_flag_chg_cmd(change_ptr->profile_num, 
                                                 prof_info.roaming_disallowed);
  }


  // PDN specific APN param change handling
  if (ds_pdn_nv_manager_get_apn_param_chg_cfg() &&
      change_ptr->apn_param_changed)
  {
    ds_pdn_mgr_apn_param_change_cmd_hdlr(change_ptr->profile_num, prof_info);    
  }

  return;
}

/*===========================================================================
FUNCTION      DS_3GPP_PDN_CNTX_GET_THR0TTLE_INFO

DESCRIPTION
  Handler for PS_SYS_IOCTL_PDN_THROTTLE_INFO. Retrieves throttle info
  for all apns for the current plmn, saved in the Per PLMN throttle
  table.

PARAMETERS 
  arg_val_ptr: Argument to be populated
  subs_id:     Subscription id
  ps_errno:    Err number if IOCTL cant be handled
   
  
 
DEPENDENCIES
  None.

RETURN VALUE
  0  - Success
  -1 - Failure
 
SIDE EFFECTS
  None.
===========================================================================*/ 
int16 ds_3gpp_pdn_cntx_get_throttle_info
(
  void                    *arg_val_ptr,
  int16                   *ps_errno
)
{
  sys_plmn_id_s_type                   current_plmn_id;
  ds_sys_pdn_throttle_info_type        *throttle_info;
  uint32                               rat_mask = DS_3GPP_THROT_RAT_MASK_GLOBAL;
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(arg_val_ptr == NULL)
  {
    *ps_errno = DS_EFAULT;
    return -1;
  }

  /*-----------------------------------------------------------------------  
    Reset output buffer
    num_apn = 0 implicit
   -----------------------------------------------------------------------*/
  memset(arg_val_ptr, 0, sizeof(ds_sys_pdn_throttle_info_type));
  throttle_info = (ds_sys_pdn_throttle_info_type *)arg_val_ptr;

  ds_pdn_hdlr_get_current_plmn(&current_plmn_id);

    rat_mask |= ds_3gpp_pdn_throt_sm_sys_mode_to_rat_mask
                  ((ds_pdn_hdlr_get_current_network_mode()));

  /*------------------------------------------------------------------------
    For current plmn, get throttle info for all apns 
   -----------------------------------------------------------------------*/
  ds_3gpp_pdn_throt_sm_get_throttle_info_for_plmn
    (&current_plmn_id, throttle_info, rat_mask, FALSE); 

  *ps_errno = 0; /* No error */
  return 0; 
} /*ds_3gpp_pdn_cntx_get_throttle_info() */


/*==============================================================================
FUNCTION DS_PDN_MGR_SET_t3448_STAUS_IND

DESCRIPTION
  This function is used to update the status of t3448 from NAS

PARAMETERS   :
     t3448_timer status from NAS

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
==============================================================================*/

void ds_pdn_mgr_set_T3448_staus_ind
(
  emm_timer_t3448_status_ind_type* t3448_status
)
{
  ds_bearer_mgr_s 	       *bearer_mgr_p = NULL;
  uint8                        index = 0;
/*.................................................................*/

  t3448_timer_ind.status = t3448_status->status;
  t3448_timer_ind.timer_t3448_value = t3448_status->timer_t3448_value;
   	
  for(index = 0; index < DS_3GPP_MAX_BEARER_CONTEXT; index++)
  {
    bearer_mgr_p = ds_bearer_mgr_get_instance_ptr(index);
 
    if(!ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p))
    {
      continue;
    }
    if(t3448_timer_ind.status == EMM_TIMER_T3448_STATUS_START &&
        !ds_pdn_hdlr_is_ue_in_ac11to15())
    {
      ds_bearer_mgr_disable_tx_flow(bearer_mgr_p, DS_BEARER_CP_DATA_BACKOFF_MASK); 
    }
    else
    {
      ds_bearer_mgr_enable_tx_flow(bearer_mgr_p, DS_BEARER_CP_DATA_BACKOFF_MASK); 
    }
  }  
  return; 	
}

/*==============================================================================
FUNCTION DS_PDN_MGR_SET_BARRING_STAUS_IND

DESCRIPTION
  This function is used to update the status of t302 from NAS

PARAMETERS   :
     t302_timer status frrom NAS

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
==============================================================================*/

void ds_pdn_mgr_set_barring_staus_ind
(
  emm_timer_barring_status_ind_type* barring_status
)
{
  ds_bearer_mgr_s                   *bearer_mgr_p = NULL;
  emm_timer_barring_status_ind_type barring_status_ind = {{0}};
  uint8                             index = 0;
/*.................................................................*/
#ifdef FEATURE_MODE_TLB
  ds_eps_tlb_per_subs_info_type     *hndl = NULL;
  boolean                           tlb_hndl_is_valid = FALSE;
/*.................................................................*/

  hndl = ds_eps_tlb_get_info_hndl();

  if (ds_eps_tlb_validate_info_hndl(hndl))
  {
    tlb_hndl_is_valid = TRUE;
  }

  if ( tlb_hndl_is_valid &&
       ( hndl->tlb_state.mode == LTE_TLB_MODE_A || 
         hndl->tlb_state.mode == LTE_TLB_MODE_B || 
         hndl->tlb_state.mode == LTE_TLB_MODE_G ||
         hndl->tlb_state.mode == LTE_TLB_MODE_I ||
         hndl->tlb_state.mode == LTE_TLB_MODE_H )
     )
  {
    DS_MSG0(MSG_LEGACY_HIGH,"TLB Mode is active ignore barring");
    return;
  }
#endif /*FEATURE_MODE_TLB*/

  DS_MSG1(MSG_LEGACY_HIGH, "ds_pdn_mgr_set_barring_staus_ind current %d",
                              active_barring_mask);
  barring_status_ind.status = barring_status->status;
  barring_status_ind.barring_category = barring_status->barring_category;

  for(index = 0; index < DS_3GPP_MAX_BEARER_CONTEXT; index++)
  {
    bearer_mgr_p = ds_bearer_mgr_get_instance_ptr(index);
    if(!ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p))
    {
      continue;
    }
    if(barring_status_ind.status == EMM_TIMER_BARRING_STATUS_START)
    {
      if(barring_status_ind.barring_category == EMM_TIMER_BARRING_MO_DATA)
      {
        ds_bearer_mgr_disable_tx_flow(bearer_mgr_p, DS_BEARER_MO_DATA_BARRING_MASK); 
        active_barring_mask |= (1<<EMM_TIMER_BARRING_MO_DATA);
      }
      if(barring_status_ind.barring_category == EMM_TIMER_BARRING_MO_EXP_DATA)
      {
        ds_bearer_mgr_disable_tx_flow(bearer_mgr_p, DS_BEARER_MO_EX_DATA_BARRING_MASK); 
        active_barring_mask |= (1<<EMM_TIMER_BARRING_MO_EXP_DATA);
      }
    }
    else if(barring_status_ind.status == EMM_TIMER_BARRING_STATUS_STOP)
    {
      if(barring_status_ind.barring_category == EMM_TIMER_BARRING_MO_DATA)
      {
        ds_bearer_mgr_enable_tx_flow(bearer_mgr_p, DS_BEARER_MO_DATA_BARRING_MASK); 
        active_barring_mask &= ~(1<<EMM_TIMER_BARRING_MO_DATA);
      }
      if(barring_status_ind.barring_category == EMM_TIMER_BARRING_MO_EXP_DATA)
      {
        ds_bearer_mgr_enable_tx_flow(bearer_mgr_p, DS_BEARER_MO_EX_DATA_BARRING_MASK); 
        active_barring_mask &= ~(1<<EMM_TIMER_BARRING_MO_EXP_DATA);
         
      }
      if(ds_bearer_mgr_is_data_pending())
      {
        ds_pdn_cp_data_process_lte_data_transport_cnf_internal(FALSE,
               ds_pdn_cp_data_get_barring_cnf_index());    
        ds_bearer_mgr_proc_reorig_dorm_cmd(bearer_mgr_p);
        ds_pdn_cp_data_set_barring_cnf_index(DS_PDN_CP_DATA_MAX_SENDING_WINDOW_SIZE);
      }
   
    }
  }
    DS_MSG1(MSG_LEGACY_HIGH, "ds_pdn_mgr_set_barring_staus_ind after new ind %d",
                              active_barring_mask);
  return;
}

/*==============================================================================
FUNCTION DS_PDN_MGR_IS_t3448_RUNNING

DESCRIPTION
  This function is used to check whether t3448 timer is running or not

PARAMETERS   :
   

DEPENDENCIES
  None

RETURN VALUE
  TRUE if t3448 running 
  FASE if t3448 not running

SIDE EFFECTS
  None
==============================================================================*/

boolean ds_pdn_mgr_is_T3448_running()
{
  if(t3448_timer_ind.status == EMM_TIMER_T3448_STATUS_START)
  {
    return TRUE;
  }
  return FALSE;
}

/*==============================================================================
FUNCTION DS_PDN_MGR_IS_BARRING_TIMER_RUNNING

DESCRIPTION
  This function is used to check whether t3448 timer is running or not

PARAMETERS   :
   

DEPENDENCIES
  None

RETURN VALUE
  TRUE if its running 
  FASE if its not running

SIDE EFFECTS
  None
==============================================================================*/
boolean ds_pdn_mgr_is_barring_timer_running()
{
  if((active_barring_mask & (1<<EMM_TIMER_BARRING_MO_DATA)) ||
     (active_barring_mask & (1<<EMM_TIMER_BARRING_MO_EXP_DATA)))
  {
    return TRUE;
  }
  return FALSE;
}

/*==============================================================================
FUNCTION DS_PDN_MGR_GET_ACTIVE_BARRING_BMASK

DESCRIPTION
  This function is used to get the actvie barring bmask

PARAMETERS   :
   

DEPENDENCIES
  None

RETURN VALUE
bmask

SIDE EFFECTS
  None
==============================================================================*/
uint32 ds_pdn_mgr_get_active_barring_bmask()
{
  return active_barring_mask;
}

/*==============================================================================
FUNCTION DS_PDN_MGR_GET_t3448_TIMER_VALUE

DESCRIPTION
  This function is used to get the t3448 timer value

PARAMETERS   :
   

DEPENDENCIES
  None

RETURN VALUE
 Timer value

SIDE EFFECTS
  None
==============================================================================*/
dword ds_pdn_mgr_get_t3448_timer_value()
{
  return t3448_timer_ind.timer_t3448_value;
}
/*===========================================================================
FUNCTION      DS_PDN_CCMI_CFCM_EVENT_CB

DESCRIPTION   Callback registered with CFCM for CPU and Thermal monitor related events.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_pdn_ccmi_cfcm_event_cb 
(
  cfcm_cmd_type_s* fc_info
)
{

  if (fc_info != NULL && fc_info->client_id == CFCM_CLIENT_DS )
  {
    if((fc_info->monitors_active & CFCM_MONITOR_MASK_THERMAL_PA_EM)||
       ((fc_info->monitors_active & CFCM_MONITOR_MASK_THERMAL_PA_EX)|| 
        (fc_info->monitors_active & CFCM_MONITOR_MASK_THERMAL_RUNAWAY_EX)))
    {
       DS_MSG2(MSG_LEGACY_HIGH,
  	       "ds_pdn_ccmi_cfcm_event_cb cmd %d ue_is_in_thermal_level3_mode %d",
                fc_info->cmd,ue_is_in_thermal_level3_mode );

      if(fc_info->cmd == CFCM_CMD_SHUT_DOWN) //CFCM_CMD_SHUT_DOWN == 31
      {
         ue_is_in_thermal_level3_mode = TRUE;
         return;
      }
      else
      { 
        /* UE not in Level 3 thermal emergency mode */
        ue_is_in_thermal_level3_mode = FALSE;
        return;
      }
    }
  }
  DS_MSG0(MSG_LEGACY_ERROR, 
           "Invalid client_id or fc ptr is passed to ds_pdn_ccmi_cfcm_event_cb");
  return;
}

/*===========================================================================
FUNCTION      DS_PDN_CCMI_REGISTER_WITH_CFCM_EVENTS

DESCRIPTION   Registers CCM with CFCM to get CFCM events for Thermal monitors.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_pdn_ccmi_register_with_cfcm_events (void)
{
  cfcm_reg_req_type_s ds_pdn_ccm_client_reg_req;
    
  ds_pdn_ccm_client_reg_req.client_id = CFCM_CLIENT_DS;
  ds_pdn_ccm_client_reg_req.req_umid = 0;
  ds_pdn_ccm_client_reg_req.req_cb = ds_pdn_ccmi_cfcm_event_cb; 
  ds_pdn_ccm_client_reg_req.monitor_mask = 0;

  ds_pdn_ccm_client_reg_req.monitor_mask |= CFCM_MONITOR_MASK_THERMAL_PA_EM|
                                            CFCM_MONITOR_MASK_THERMAL_PA_EX|
                                            CFCM_MONITOR_MASK_THERMAL_RUNAWAY_EX;

  if (	ds_pdn_ccm_client_reg_req.monitor_mask != 0 )
  {
    cfcm_client_register(&ds_pdn_ccm_client_reg_req);

  }

}
#ifdef FEATURE_GPS_WWAN_PRIORITY_SEARCH
/*===========================================================================
FUNCTION ds_pdn_evaluate_gps_high_priority_search_flow_for_all_bearers

DESCRIPTION
  This function cycles through the bearer context table, finds all
  bearers evaluates the flow as applicable when GPS high prioirt seach
  started/stoped.
  
PARAMETERS
  enable_flow 
  True enable the flow
  False Disable the flow 

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_evaluate_gps_high_priority_search_flow_for_all_bearers
(
  boolean  enable_flow
)
{
  ds_bearer_mgr_s               *bearer_mgr_p = NULL;
  uint8                          index = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_MSG0(MSG_LEGACY_HIGH, 
  	       "GPS high priority search is on going/stoped disable/enable flow_for_all_bearers");
  /*-------------------------------------------------------------------------
    Loop through all the bearers. call disable/enable
  -------------------------------------------------------------------------*/
  for(index = 0; index < DS_3GPP_MAX_BEARER_CONTEXT; index++)
  {
    bearer_mgr_p = ds_bearer_mgr_get_instance_ptr(index);

    if(!ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p))
    {
      continue;
    }
    if(enable_flow)
    {
      ds_bearer_mgr_enable_tx_flow(bearer_mgr_p, DS_BEARER_GPS_SEARCH_IN_PROGRESS_MASK);
    }
	else
   {
     ds_bearer_mgr_disable_tx_flow(bearer_mgr_p, DS_BEARER_GPS_SEARCH_IN_PROGRESS_MASK);
   }
  }
}
#endif /* FEATURE_GPS_WWAN_PRIORITY_SEARCH */


/*===========================================================================
FUNCTION      ds_pdn_mem_ctrl_ev_cb

DESCRIPTION   This function will invoke client functions when a DSM Mem 
              Control Event occurs.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_pdn_mem_ctrl_ev_cb
(
  dsm_mempool_id_enum_type  pool_id,
  dsm_mem_level_enum_type   event,
  dsm_mem_op_enum_type      op
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_MSG0(MSG_LEGACY_HIGH,"ds_pdn_mem_ctrl_ev_cb");
  ds3g_siolib_mem_ctrl_ev_cb(pool_id, event, op);
  ds_bearer_mgr_mem_ctrl_ev_cb(pool_id, event, op);

  return;
} /* ds_pdn_mem_ctrl_ev_cb() */

/*===========================================================================
FUNCTION      DS3G_REGISTER_MEM_EVENT_CALLBACKS

DESCRIPTION   This function will register call functions with DSM memory events

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_pdn_mgr_register_mem_event_callbacks( void )
{
  /*-----------------------------------------------------------------------
    Register all DSM_DS_SMALL_ITEM_POOL related events.
  -----------------------------------------------------------------------*/

  #define DS3G_DSM_DS_SMALL_ITEM_SIO_MANY_MARK 175
  #define DS3G_DSM_DS_SMALL_ITEM_SIO_FEW_MARK  25

  dsm_reg_mem_event_level( DSM_DS_SMALL_ITEM_POOL,
                           DSM_MEM_LEVEL_SIO_FEW,
                           DS3G_DSM_DS_SMALL_ITEM_SIO_FEW_MARK );

  dsm_reg_mem_event_level( DSM_DS_SMALL_ITEM_POOL,
                           DSM_MEM_LEVEL_SIO_MANY,
                           DS3G_DSM_DS_SMALL_ITEM_SIO_MANY_MARK );
  
  dsm_reg_mem_event_cb ( DSM_DS_SMALL_ITEM_POOL,
                         DSM_MEM_LEVEL_SIO_MANY,
                         DSM_MEM_OP_FREE,
                         ds_pdn_mem_ctrl_ev_cb );

  dsm_reg_mem_event_cb ( DSM_DS_SMALL_ITEM_POOL,
                         DSM_MEM_LEVEL_SIO_FEW,
                         DSM_MEM_OP_NEW,
                         ds_pdn_mem_ctrl_ev_cb );
  

  #define DS3G_DSM_DS_LARGE_ITEM_SIO_MANY_MARK 100 
  #define DS3G_DSM_DS_LARGE_ITEM_SIO_FEW_MARK  50

  dsm_reg_mem_event_level( DSM_DS_LARGE_ITEM_POOL,
                           DSM_MEM_LEVEL_SIO_FEW,
                           DS3G_DSM_DS_LARGE_ITEM_SIO_FEW_MARK );

  dsm_reg_mem_event_level( DSM_DS_LARGE_ITEM_POOL,
                           DSM_MEM_LEVEL_SIO_MANY,
                           DS3G_DSM_DS_LARGE_ITEM_SIO_MANY_MARK );
  
  /*-----------------------------------------------------------------------
    Register all DSM_DS_LARGE_ITEM_POOL related events.
  -----------------------------------------------------------------------*/
  dsm_reg_mem_event_cb ( DSM_DS_LARGE_ITEM_POOL,
                         DSM_MEM_LEVEL_SIO_MANY,
                         DSM_MEM_OP_FREE,
                         ds_pdn_mem_ctrl_ev_cb );

 
  dsm_reg_mem_event_cb ( DSM_DS_LARGE_ITEM_POOL,
                         DSM_MEM_LEVEL_SIO_FEW,
                         DSM_MEM_OP_NEW,
                         ds_pdn_mem_ctrl_ev_cb);
  
  /*------------------------------------------------------------------ 
    Overwriting levels set in DSM init
    Number of data in flight is RTT * BW. The levels have to be inline
    with this.
  ------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
  Register all DSM_DS_DUP_ITEM_POOL related events.
-----------------------------------------------------------------------*/
#define DS3G_DSM_DS_DUP_ITEM_SIO_MANY_MARK 600  
#define DS3G_DSM_DS_DUP_ITEM_SIO_FEW_MARK  400

/* Register level and its count 			   */
  dsm_reg_mem_event_level(DSM_DUP_ITEM_POOL,
                          DSM_MEM_LEVEL_SIO_FEW,
                          DS3G_DSM_DS_DUP_ITEM_SIO_FEW_MARK );

/* Register Opposite level and its count	   */
  dsm_reg_mem_event_level(DSM_DUP_ITEM_POOL,
                          DSM_MEM_LEVEL_SIO_MANY,
                          DS3G_DSM_DS_DUP_ITEM_SIO_MANY_MARK );

/* Register both levels for optimized callback */
  dsm_reg_mem_event_optimized_level(DSM_DUP_ITEM_POOL,
                                    DSM_MEM_LEVEL_SIO_FEW,
                                    DSM_MEM_LEVEL_SIO_MANY);

  dsm_reg_mem_event_cb ( DSM_DUP_ITEM_POOL,
                         DSM_MEM_LEVEL_SIO_MANY,
                         DSM_MEM_OP_FREE,
                         ds_pdn_mem_ctrl_ev_cb );

  dsm_reg_mem_event_cb ( DSM_DUP_ITEM_POOL,
                         DSM_MEM_LEVEL_SIO_FEW,
                         DSM_MEM_OP_NEW,
                         ds_pdn_mem_ctrl_ev_cb );


/* ds3g_siolib_register_mem_event_callbacks() */
}
#ifdef FEATURE_NBIOT_NTN
/*===========================================================================
FUNCTION      ds_pdn_mgr_set_ntn_gnss_status

DESCRIPTION   This function will used to set the ntn gnss status

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_pdn_mgr_set_ntn_gnss_status(boolean status)
{
  ds_pdn_ntn_gnss_failure_status = status;
} 

/*===========================================================================
FUNCTION      ds_pdn_mgr_get_ntn_gnss_status

DESCRIPTION   This function will used to get the ntn gnss status

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
boolean ds_pdn_mgr_get_ntn_gnss_status()
{
  return ds_pdn_ntn_gnss_failure_status;
}

#endif /* FEATURE_NBIOT_NTN */


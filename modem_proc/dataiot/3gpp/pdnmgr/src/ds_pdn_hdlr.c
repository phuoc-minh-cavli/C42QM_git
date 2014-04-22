/*===========================================================================
                      DS_PDN_HDLR.C
DESCRIPTION
This file define PDN handler APIs.

Copyright (c) 2018 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/3gpp/pdnmgr/src/ds_pdn_hdlr.c#3 $
  $DateTime: 2022/05/13 03:58:57 $$Author: pwbldsvc $

when           who    what, where, why
--------    ---    ----------------------------------------------------------
08/09/17    sd      First checkin
===========================================================================*/

#include "ds_pdn_defs.h"
#include "ds_pdn_hdlr.h"
#include "ds_pdn_manager.h"
#include "ds3gmmgsdiif.h"
#include "ds_pdn_nv_manager.h"
#include "dssysmgr.h"
#include "ds_pdn_psm_hdlr.h"
#include "ds_gprs_bearer_manager.h"
#include "ds_3gpp_throttle_sm.h"
#include "ds_pdn_cp_data_hdlr.h"
#include "ds_3gpp_pdn_throttle_sm.h"
#include "ds_3gpp_dam.h"
#include "ds_3gpp_srv_req_throt_sm.h"
#include "ds_pdn_roaming_hdlr.h"
#include "ds_3gpp_rpm.h"

/*---------------------------------------------------------------------------
  Global to store system specific parameters
---------------------------------------------------------------------------*/
static ds_pdn_hdlr_info_type ds_pdn_hdlr_info = {0};

/*===========================================================================
FUNCTION ds_pdn_hdlr_clean_up_params
 
DESCRIPTION
  This function cleans up all the parameters owned by PDN HDLR
 
PARAMETERS 
  None

DEPENDENCIES 
  None

RETURN VALUE 
  None
 
SIDE EFFECTS 
  None
===========================================================================*/
void ds_pdn_hdlr_cleanup_params
(
  void
)
{
  ds_3gpp_throt_rat_e_type  rat_index = DS_3GPP_THROT_RAT_MIN;
  list_type                 *list_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_pdn_hdlr_info.attach_is_complete = FALSE;
  ds_pdn_hdlr_info.attach_is_initiated = FALSE;

#ifdef FEATURE_DATA_LTE
  ds_pdn_hdlr_info.internal_sys_mode = SYS_SYS_MODE_LTE;
  ds_pdn_hdlr_info.last_full_srv_mode = SYS_SYS_MODE_LTE;
#elif defined FEATURE_GSM_GPRS
  ds_pdn_hdlr_info.internal_sys_mode = SYS_SYS_MODE_GSM;
  ds_pdn_hdlr_info.last_full_srv_mode = SYS_SYS_MODE_GSM;
#endif  

  ds_pdn_hdlr_info.data_suspend_flag = FALSE;
  ds_pdn_hdlr_info.sent_ps_data_available = FALSE;

  ds_pdn_hdlr_info.t3346_status.active_rat = SYS_SYS_MODE_NONE;
  ds_pdn_hdlr_info.t3346_status.t3346_status = SYS_NAS_T3346_STATUS_UNKNOWN;

  ds_pdn_hdlr_info.current_ds_lte_flag = DS_PDN_LTE_NOT_SET;  

  memset(&ds_pdn_hdlr_info.ciot_cap, 0, sizeof(sys_neg_ciot_cap));

    for (rat_index = DS_3GPP_THROT_RAT_GLOBAL;
       rat_index < DS_3GPP_THROT_RAT_MAX;
       rat_index++)
  {
    ds_pdn_hdlr_info.max_simul_active_pdp_info[rat_index].
                 max_simul_active_pdp_context = DS_3GPP_MAX_BEARER_CONTEXT;
  
    memset(&(ds_pdn_hdlr_info.max_simul_active_pdp_info[rat_index].plmn_id),
         0,
         sizeof(sys_plmn_id_s_type));
  }

  list_ptr = &ds_pdn_hdlr_info.blocked_plmn_list;
  if(list_ptr != NULL)
  { 
    ds_3gpp_pdn_cntxt_flush_blocked_plmn_list();
  }
  else
  {
    list_init(&(ds_pdn_hdlr_info.blocked_plmn_list));
  }
  
  memset(&(ds_pdn_hdlr_info.current_plmn),
         0,
         sizeof(sys_plmn_id_s_type));
}

/*===========================================================================
FUNCTION ds_pdn_hdlr_init
 
DESCRIPTION
  This function initializes all system specific parmeters for 3GPP MH
 
PARAMETERS 
  None

DEPENDENCIES 
  None

RETURN VALUE 
  None
 
SIDE EFFECTS 
  None
===========================================================================*/
void ds_pdn_hdlr_init
(
  void
)
{
  uint8  event_type_mask = (1 << DSSYSMGR_EVT_TYPE_PH);
  
  ds_register_cmd_hdlr(DS_MODULE_PDN_MGR, ds_pdn_mgr_process_cmds);
  dssysmgr_client_reg_event(DSSYSMGR_CLIENT_3GPP,event_type_mask,
                            ds_pdn_mgr_notify_cb,NULL);
  dssysmgr_cm_init();

  ds_pdn_hdlr_cleanup_params();
  
  return;
}

/*===========================================================================
FUNCTION ds_pdn_hdlr_get_current_network_mode

DESCRIPTION
  This function returns the current mode within the 3GPP RATs. This mode
  is maintained by the 3GPP mode handler and is used to dispatch mode
  specific functions

PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE
  sys_sys_mode_e_type - current mode

SIDE EFFECTS
  None.
===========================================================================*/
sys_sys_mode_e_type ds_pdn_hdlr_get_current_network_mode
( 
  void
)
{
  return ds_pdn_hdlr_info.internal_sys_mode;
} /* ds_pdn_hdlr_get_current_network_mode */

/*===========================================================================
FUNCTION ds_pdn_hdlr_set_network_mode

DESCRIPTION
  This function updates the network mode maintained inside the 3GPP mode
  handler.

PARAMETERS
  network_mode - the mode to which we have to update.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_pdn_hdlr_set_network_mode
( 
  sys_sys_mode_e_type     network_mode
)
{
  ds_pdn_hdlr_info.internal_sys_mode = network_mode;
  return;
} /* ds_pdn_hdlr_set_network_mode */

/*===========================================================================
FUNCTION ds_pdn_hdlr_get_ps_data_suspend_flag

DESCRIPTION
  This function returns the flag indicating if PS data is suspended.

PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE
  boolean : TRUE if set, FALSE otehrwise

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_pdn_hdlr_get_ps_data_suspend_flag
( 
  void
)
{
  return ds_pdn_hdlr_info.data_suspend_flag;
} /* ds_pdn_hdlr_get_ps_data_suspend_flag */

/*===========================================================================
FUNCTION ds_pdn_hdlr_get_attach_initiated_flag

DESCRIPTION
  This function returns the flag indicating if attach is initiated

PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE
  boolean : TRUE if set, FALSE otehrwise

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_pdn_hdlr_get_attach_initiated_flag
( 
  void
)
{
  return ds_pdn_hdlr_info.attach_is_initiated;
} /* ds_pdn_hdlr_get_attach_initiated_flag */

/*===========================================================================
FUNCTION ds_pdn_hdlr_set_attach_initiated_flag

DESCRIPTION
  This function sets the flag indicating if attach is initiated

PARAMETERS
  boolean : TRUE if set, FALSE otherwise

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_pdn_hdlr_set_attach_initiated_flag
( 
  boolean flag
)
{
  ds_pdn_hdlr_info.attach_is_initiated = flag;
} /* ds_pdn_hdlr_set_attach_initiated_flag */

/*===========================================================================
FUNCTION ds_pdn_hdlr_get_attach_complete_flag

DESCRIPTION
  This function returns the flag indicating if attach is complete

PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE
  boolean : TRUE if set, FALSE otehrwise

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_pdn_hdlr_get_attach_complete_flag
( 
  void
)
{
  return ds_pdn_hdlr_info.attach_is_complete;
} /* ds_pdn_hdlr_get_attach_complete_flag */

/*===========================================================================
FUNCTION ds_pdn_hdlr_set_attach_complete_flag

DESCRIPTION
  This function sets the flag indicating if attach is complete

PARAMETERS
  boolean : TRUE if set, FALSE otherwise

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_pdn_hdlr_set_attach_complete_flag
( 
  boolean flag
)
{
  ds_pdn_hdlr_info.attach_is_complete = flag;
} /* ds_pdn_hdlr_set_attach_complete_flag */

/*===========================================================================
FUNCTION ds_pdn_hdlr_get_sent_ps_data_avail_flag

DESCRIPTION
  This function returns the flag indicating if PS data available 
  indication was sent.

PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE
  boolean : TRUE if set, FALSE otehrwise

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_pdn_hdlr_get_sent_ps_data_avail_flag
( 
  void
)
{
  return ds_pdn_hdlr_info.sent_ps_data_available;
} /* ds_pdn_hdlr_get_sent_ps_data_avail_flag */

/*===========================================================================
FUNCTION ds_pdn_hdlr_set_sent_ps_data_avail_flag

DESCRIPTION
  This function sets the flag indicating if PS data available 
  indication was sent.

PARAMETERS
  boolean : TRUE if set, FALSE otehrwise

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void ds_pdn_hdlr_set_sent_ps_data_avail_flag
( 
  boolean   sent_ps_data_available
)
{
  ds_pdn_hdlr_info.sent_ps_data_available = sent_ps_data_available;
} /* ds_pdn_hdlr_set_sent_ps_data_avail_flag */

/*===========================================================================
FUNCTION ds_pdn_hdlr_get_t3346_status

DESCRIPTION
  This function returns the T3346 status from NAS

PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE
  ds_pdn_t3346_status_type

SIDE EFFECTS
  None.
===========================================================================*/
ds_pdn_t3346_status_type ds_pdn_hdlr_get_t3346_status
( 
  void
)
{
  return ds_pdn_hdlr_info.t3346_status;
} /* ds_pdn_hdlr_get_t3346_status */

/*===========================================================================
FUNCTION  ds_pdn_hdlr_is_ps_data_avail_ind_allowed

DESCRIPTION
  This function queries srv_status maintained by ds3gmgr to check if
  we need to flow control the bearer and not send PS DATA AVAILABLE ind
  to CM.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  Boolean

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_pdn_hdlr_is_ps_data_avail_ind_allowed
(
  void
)
{
  sys_srv_status_e_type curr_srv_status;
  boolean ret_val = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  curr_srv_status = dssysmgr_get_srv_status();
  if((curr_srv_status == SYS_SRV_STATUS_SRV) || /* Full Service */
     ((ds_pdn_nv_manager_get_send_ps_data_avail_on_pwr_save()) &&
      (curr_srv_status == SYS_SRV_STATUS_PWR_SAVE))) /* Power Save */
  {
    ret_val = TRUE;
  }

  DS_MSG1(MSG_LEGACY_HIGH, "ds3gpp_is_ps_data_avail_ind_allowed: %d", ret_val);
  return ret_val;
}/*ds3gpp_is_ps_data_avail_ind_allowed*/

/*===========================================================================
FUNCTION ds_pdn_hdlr_get_blocked_plmn_list

DESCRIPTION
  This function returns the blocked PLMN list.

PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE
  list_type 

SIDE EFFECTS
  None.
===========================================================================*/
list_type* ds_pdn_hdlr_get_blocked_plmn_list
( 
  void
)
{
  return &ds_pdn_hdlr_info.blocked_plmn_list;
} /* ds_pdn_hdlr_get_blocked_plmn_list */

/*===========================================================================
FUNCTION ds_pdn_hdlr_get_current_plmn

DESCRIPTION
  This function fetches the current PLMN ID

PARAMETERS
  plmn_id_ptr - pointer to PLMN id to be returned

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_pdn_hdlr_get_current_plmn
(
  sys_plmn_id_s_type*     plmn_ptr
)
{
  if(plmn_ptr == NULL)
  {
    return;
  }

  memscpy((void*)plmn_ptr,
          sizeof(sys_plmn_id_s_type),
          &(ds_pdn_hdlr_info.current_plmn),
          sizeof(sys_plmn_id_s_type));
  return;
} /* ds_pdn_hdlr_get_current_plmn */

/*===========================================================================
FUNCTION ds_pdn_hdlr_set_current_plmn

DESCRIPTION
  This function sets the current PLMN ID

PARAMETERS
  plmn_id_ptr - pointer to PLMN id to be returned

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_pdn_hdlr_set_current_plmn
(
  emm_plmn_change_ind_type *payload
)
{
  
  if(payload == NULL)
  {
    return;
  }

  memscpy(&(ds_pdn_hdlr_info.current_plmn),
          sizeof(sys_plmn_id_s_type),
          &(payload->plmn),
          sizeof(sys_plmn_id_s_type));
  return;
} /* ds_pdn_hdlr_set_current_plmn */

/*===========================================================================
FUNCTION ds_pdn_plmn_change_hdlr

DESCRIPTION
  This Handler is triggered while PLMN  changes
  
PARAMETERS  
  plmn_id - PLMN ID to be checked
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  TRUE  - If PLMN is non-NULL
  FALSE - otherwise

SIDE EFFECTS 
  None.  
===========================================================================*/
void ds_pdn_hdlr_plmn_change_hdlr
(
  emm_plmn_change_ind_type *payload
)
{
  uint32                     plmn_id_changed = 0;
  ds_pdn_apm_ind_pay_load    ds_pdn_apm_payload = {0};

  if(payload == NULL)
  {
    return;
  }

  plmn_id_changed = memcmp(&ds_pdn_hdlr_info.current_plmn,
                             &payload->plmn,
                             sizeof(sys_plmn_id_s_type));
  if(plmn_id_changed != 0)
  {
    
    if (TRUE == ds_3gpp_throt_sm_is_plmn_non_null(payload->plmn))
    {
#ifdef FEATURE_DATA_RPM
      if (ds_3gpp_is_rpm_enabled())
      {
        ds_3gpp_rpm_plmn_id_chg_hdlr(payload->plmn);
      }
#endif /* FEATURE_DATA_RPM */
      
      ds_3gpp_pdn_process_new_plmn( payload->plmn,
                              ds_pdn_hdlr_get_current_network_mode());
      ds_3gpp_pdn_cntxt_disable_lte_if_attach_profile_unavailable();
    }
    
    ds_pdn_hdlr_set_current_plmn(payload);
    
    ds_pdn_apm_payload.ind_field_mask = DS_APM_IND_PLMN_FIELD_MASK;
    memscpy(&(ds_pdn_apm_payload.plmn_id),
	           sizeof( sys_plmn_id_s_type ),
            &(payload->plmn),
            sizeof(sys_plmn_id_s_type));
    ds_pdn_apm_ind(DS_PDN_APM_PLMN_CHANGED_IND, &(ds_pdn_apm_payload));

    /*-------------------------------------------------------------------
       Clear Serving PLMN Rate Control Information if PLMN has changed
       for all bearers
      ------------------------------------------------------------------*/
    ds_pdn_cp_data_process_plmn_changed(payload->plmn);
  }
  else
  {
    ds_3gpp_pdn_throt_clr_on_plmn_change(ds_pdn_hdlr_get_current_network_mode(),
                                           payload->plmn);
  }
  return;
}

/*===========================================================================
FUNCTION ds_pdn_hdlr_is_plmn_non_null

DESCRIPTION
  This function checks if the PLMN passed into this function is non-NULL
  
PARAMETERS  
  plmn_id - PLMN ID to be checked
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  TRUE  - If PLMN is non-NULL
  FALSE - otherwise

SIDE EFFECTS 
  None.  
===========================================================================*/
boolean ds_pdn_hdlr_is_plmn_non_null
(
  sys_plmn_id_s_type plmn_id
)
{
  sys_plmn_id_s_type zero_plmn_id = {0};
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(memcmp((void*)&(plmn_id),
            (void*)&(zero_plmn_id), 
            sizeof(sys_plmn_id_s_type)) != 0)
  {
    return TRUE;
  }
  else
  {
    DS_ERROR_LOG_0("PLMN being inspected is zero");
    return FALSE;
  }
} /* ds_pdn_hdlr_is_plmn_non_null */

/*===========================================================================
FUNCTION      ds_pdn_hdlr_get_hplmn_id

DESCRIPTION   This function returns the HPLMN ID.
              Note: memory has to be allocated by user.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_pdn_hdlr_get_hplmn_id
(
  sys_plmn_id_s_type      *hplmn_id_p
)
{
  sys_plmn_id_s_type  hplmn;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Get HPLMN id from NAS
  -------------------------------------------------------------------------*/
  hplmn = reg_sim_per_subs_hplmn_get( SYS_MODEM_AS_ID_1 );
  memscpy(hplmn_id_p,
          sizeof(sys_plmn_id_s_type),
          &hplmn,
          sizeof(sys_plmn_id_s_type));
  return;
} /* ds_pdn_hdlr_get_hplmn_id() */

/*===========================================================================
FUNCTION      ds_pdn_hdlr_get_ehplmn_list

DESCRIPTION   This function returns the EHPLMN List
              Note: memory has to be allocated by user.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_pdn_hdlr_get_ehplmn_list
(
  reg_sim_plmn_list_s_type  *ehplmn_list_p
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Get EHPLMN list from NAS
  -------------------------------------------------------------------------*/
  reg_sim_per_subs_ehplmn_list_get_ptr(ehplmn_list_p, SYS_MODEM_AS_ID_1 );
  return;
} /* ds_pdn_hdlr_get_ehplmn_list() */

/*===========================================================================
FUNCTION      ds_pdn_hdlr_is_plmn_in_ehplmn_list

DESCRIPTION   This function checks whether the given PLMN is in EHPLMN List

DEPENDENCIES  None

RETURN VALUE  TRUE, if PLMN is in EHPLMN List
                       FALSE, otherwise

SIDE EFFECTS  None
===========================================================================*/
boolean ds_pdn_hdlr_is_plmn_in_ehplmn_list
(
  sys_plmn_id_s_type      plmn_id
)
{
  reg_sim_plmn_list_s_type   ehplmn_list;
  uint32                     index = 0;
  boolean                    ret_val = FALSE;
  /*------------------------------------------------------------------------*/

  memset(&ehplmn_list, 0, sizeof(reg_sim_plmn_list_s_type));

  ds_pdn_hdlr_get_ehplmn_list(&ehplmn_list);

  for (index = 0; index < ehplmn_list.length; index++)
  {
    if (sys_plmn_match(plmn_id, ehplmn_list.plmn[index]) == TRUE)
    {
      ret_val = TRUE;
      break;
    }
  }

  return ret_val;
} /* ds_pdn_hdlr_is_plmn_in_ehplmn_list */

/*===========================================================================
  FUNCTION DS_3GPP_EXTRACT_MCC_FROM_PLMN_ID
  
  DESCRIPTION
    This function extracts MCC from PLMN ID
    
  PARAMETERS  
   sys_plmn_id_s_type          *current_plmn_id
   uint16                      *ret_mcc (MCC ptr)
   uint16                      *ret_mnc (MNC ptr)
      
  DEPENDENCIES 
    None.
    
  RETURN VALUE 
    void
     
  SIDE EFFECTS 
    None.
    
===========================================================================*/
void ds_pdn_hdlr_extract_mcc_mnc_from_plmn_id
(
   sys_plmn_id_s_type          *current_plmn_id,
   uint16                      *ret_mcc,
   uint16                      *ret_mnc
)
{
  uint8                       mcc_digit[3] = {0};
  uint16                      mcc = 0;
  uint8                       mnc_digit[3] = {0};
  uint16                      mnc = 0;
  /*- - - - - -- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if((current_plmn_id == NULL) || (ret_mcc == NULL) || 
     (ret_mnc == NULL))
  {
    return;
  }

  mcc_digit[0] = (current_plmn_id->identity[0] & 0x0F);
  mcc_digit[1] = (current_plmn_id->identity[0] / 0x10);
  mcc_digit[2] = (current_plmn_id->identity[1] & 0x0F);

  mcc = (mcc_digit[0] * 100) + (mcc_digit[1] * 10) + (mcc_digit[2]);

  *ret_mcc = mcc;

  DS_MSG1(MSG_LEGACY_HIGH, "MCC returned is %d",mcc);

  mnc_digit[0] = (current_plmn_id->identity[2] & 0x0F);
  mnc_digit[1] = (current_plmn_id->identity[2] / 0x10);
  mnc_digit[2] = (current_plmn_id->identity[1] / 0x10);

  if (mnc_digit[2] == 0xF)
  {
    mnc = 10 * mnc_digit[0] + mnc_digit[1];
  }
  else
  {
    mnc = 100 * mnc_digit[0] + 10 * mnc_digit[1] + mnc_digit[2];
  }

  *ret_mnc = mnc;

  DS_MSG1(MSG_LEGACY_HIGH, "MNC returned is %d",mnc);
  return;
}

/*===========================================================================
  FUNCTION ds_pdn_hdlr_ps_data_suspend_resume_hdlr
  
  DESCRIPTION
    This function handles suspend/resume indication from CM
    
  PARAMETERS  
   boolean - PS data suspend/resume flag
                  TRUE - If PS data is suspended,
                  FALSE - otherwise
      
  DEPENDENCIES 
    None.
    
  RETURN VALUE 
    None
     
  SIDE EFFECTS 
    None.    
===========================================================================*/
void ds_pdn_hdlr_ps_data_suspend_resume_hdlr
(
  boolean  suspend_flag
)
{
  uint8              index = 0;
  ds_bearer_mgr_s   *bearer_mgr_p = NULL;
  boolean            send_ps_data_avail = FALSE;
  
  if(suspend_flag) /* Suspend handling */
  {
    /* "Not Suspended State" */
    if(ds_pdn_hdlr_info.data_suspend_flag == FALSE)
    {
      /*-----------------------------------------------------------------
          Suspend data transmission
          -----------------------------------------------------------------*/
      ds_pdn_hdlr_info.data_suspend_flag = TRUE;
      ds_pdn_hdlr_info.sent_ps_data_available = FALSE;
  
      ds_bearer_mgr_suspend_ind();
    }
    /* "Suspended State/PS Data Avail Not Sent" */
    else if (ds_pdn_hdlr_info.sent_ps_data_available == FALSE)
    {  
      /*-----------------------------------------------------------------
            Enable flow for all bearer if send_ps_data_avail == TRUE
            otherwise disable flow
          -----------------------------------------------------------------*/  
      send_ps_data_avail = ds_pdn_hdlr_is_ps_data_avail_ind_allowed();
      
      for( index=0; index<DS_3GPP_MAX_BEARER_CONTEXT; index++ )
      {
        bearer_mgr_p = ds_bearer_mgr_get_instance_ptr(index);
        if(!ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p))
        {
          continue;
        }
      
        if((bearer_mgr_p->state == DS_BEARER_MGR_STATE_UP_DORMANT_REESTAB) ||
           (bearer_mgr_p->state == DS_BEARER_MGR_STATE_UP_DORMANT) ||
           (bearer_mgr_p->state == DS_BEARER_MGR_STATE_UP))
        {             
          if(send_ps_data_avail)
          {
            ds_bearer_mgr_enable_tx_flow(bearer_mgr_p, DS_BEARER_CONTROL_MASK);
          }
          else
          { 
            ds_bearer_mgr_disable_tx_flow(bearer_mgr_p, DS_BEARER_CONTROL_MASK);
          }             
        }// valid for bearer state         
      }// end for all bearer_contexts    
    }
  }
  else /* Resume handling */
  {
    if(ds_pdn_hdlr_info.data_suspend_flag == TRUE)
    {
      ds_pdn_hdlr_info.data_suspend_flag = FALSE;
      ds_pdn_hdlr_info.sent_ps_data_available = FALSE;

      if(dssysmgr_is_srv_domain_ps_supported(dssysmgr_get_srv_domain()))
      {      
        ds_bearer_mgr_resume_ind();
      }      
    }
  }
}

/*===========================================================================
FUNCTION ds_pdn_hdlr_is_ue_in_ac11to15_on_plmnid

DESCRIPTION
  This function checks whether the ue is in ac 11 to 15 in
  the plmn id specified. This function returns true if 
  one of the  below scenarios is True 
 
  a) This function checks if the ue is in access class 11 or 15
     and the plmn specified matches with the home plmn or ehlpm list.

  b) This function checks if the ue is in access class 12 to 15  and 
     the plmn's  mcc part matches with the home plmn 

PARAMETERS
  None
  
DEPENDENCIES
  None.

RETURN VALUE  
  boolean - TRUE/FALSE
 
SIDE EFFECTS 
  None. 
===========================================================================*/
boolean ds_pdn_hdlr_is_ue_in_ac11to15_on_plmnid
(
  sys_plmn_id_s_type       current_plmn_id
)
{   
  boolean                ue_ac_11to15     = FALSE;
  uint32                 efacc_class      = 0;
  sys_plmn_id_s_type     hplmn_id;
  uint16                 mcc_hplmn        = 0;
  uint16                 mnc_hplmn        = 0;
  uint16                 mcc_current_plmn = 0;
  uint16                 mnc_current_plmn = 0;
  sys_plmn_id_s_type     zero_plmn;
  ds_local_err_val_e_type local_val = DS_LOCAL_ERR_VAL_NONE;


  memset(&zero_plmn, 0, sizeof(sys_plmn_id_s_type));

  if ( memcmp(&current_plmn_id, &zero_plmn, sizeof(sys_plmn_id_s_type)) == 0)
  {
    DS_ERROR_LOG_0("plmn_id passed is zero");
    return ue_ac_11to15;
  }

  do
  {
     /*------------------------------------------------------------------- 
      Get the Access class
      If the access class is 12-13-14
          Check the MCC of the Home PLMN and current Serving PLMN
      If the access class is 11 or 15
          Check the current PLMN with EHPLMN(if configured),
          If that does not match, check with current HPLMN.
    -------------------------------------------------------------------*/

    if(ds3g_mmgsdi_get_efacc_class(&efacc_class))
    {

      ds_pdn_hdlr_get_hplmn_id(&hplmn_id);

      if ((efacc_class & DS_PDN_AC_CLASS_12_to_14) != 0)
      {
        /*-------------------------------------------------------------- 
          Get the MCC of the Home PLMN country and compare it with
          current PLMN's MCC
          -------------------------------------------------------------*/
        ds_pdn_hdlr_extract_mcc_mnc_from_plmn_id(&current_plmn_id,
                                             &mcc_current_plmn,
                                             &mnc_current_plmn);

        ds_pdn_hdlr_extract_mcc_mnc_from_plmn_id(&hplmn_id,
                                             &mcc_hplmn,
                                             &mnc_hplmn);

        if((mcc_hplmn == mcc_current_plmn) && (mcc_hplmn != 0))
        {
          local_val = DS_LOCAL_ERR_VAL_0;
          ue_ac_11to15 = TRUE;
          break;
        }

      }
      if((efacc_class & DS_PDN_AC_CLASS_11_or_15)!= 0)
      {
       
        /*--------------------------------------------------------- 
          See if current PLMN is a part of EHPLMN list
          ---------------------------------------------------------*/
        if(ds_pdn_hdlr_is_plmn_in_ehplmn_list(current_plmn_id))
        {
          local_val = DS_LOCAL_ERR_VAL_1;
          ue_ac_11to15 = TRUE;
          break;
        }
        /*---------------------------------------------------------- 
          Now check if the PLMN is a part of Home PLMN
          ----------------------------------------------------------*/
        if(sys_plmn_match(current_plmn_id,hplmn_id))
        {
          local_val = DS_LOCAL_ERR_VAL_2;
          ue_ac_11to15 = TRUE;
          break;
        }
      }
    }
  }while(0);

  DS_MSG2(MSG_LEGACY_HIGH, 
  	       "ds_pdn_hdlr_is_ue_in_ac11to15_on_plmnid returned %d , reason %d",
  	       ue_ac_11to15, local_val);
  return ue_ac_11to15;
}

/*===========================================================================
FUNCTION ds_pdn_hdlr_is_ue_in_ac11to15

DESCRIPTION
  This function checks whether the ue is in ac 11 to 15 on the current 
  PLMN. This function returns true if 
  one of the  below scenarios is True 
 
  a)This function checks if the ue is in access class 11 or 15
  and the ue's current plmn matches with the home plmn or ehlpm list.

  b)This function checks if the ue is in access class 12 to 15  and 
   the ue's current plmn's mcc part matches with the home plmn 

PARAMETERS
  None
  
DEPENDENCIES
  None.

RETURN VALUE  
  boolean - TRUE/FALSE
 
SIDE EFFECTS 
  None. 

===========================================================================*/
boolean ds_pdn_hdlr_is_ue_in_ac11to15
(
  void
)
{
   sys_plmn_id_s_type     current_plmn_id;
   
   ds_pdn_hdlr_get_current_plmn(&current_plmn_id);
   return ds_pdn_hdlr_is_ue_in_ac11to15_on_plmnid(current_plmn_id);
}

/*===========================================================================
FUNCTION      ds_pdn_hdlr_set_cp_ciot_cap

DESCRIPTION   This function sets the CIOT Capability Information.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_pdn_hdlr_set_cp_ciot_cap
(
  sys_neg_ciot_cap       neg_ciot_cap
)
{
  ds_command_type                    *cmd_buf = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_MSG2(MSG_LEGACY_HIGH, 
          "ds_pdn_hdlr_set_cp_ciot_cap: cp_ciot_support old: %d new: %d",
          ds_pdn_hdlr_info.ciot_cap.cp_ciot_supported, 
          neg_ciot_cap.cp_ciot_supported);

  if (memcmp(&ds_pdn_hdlr_info.ciot_cap, &neg_ciot_cap, sizeof(sys_neg_ciot_cap)) == 0)
  {
    return;
  }

  ds_pdn_hdlr_info.ciot_cap = neg_ciot_cap;

  /*-----------------------------------------------------------------------
    Get a command buffer from the DS Task.
  -----------------------------------------------------------------------*/
  cmd_buf = ds_allocate_cmd_buf(sizeof(sys_neg_ciot_cap));
  if((NULL == cmd_buf) || (NULL == cmd_buf->cmd_payload_ptr))
  {
    DS_ASSERT(0);
    return;
  }
  else
  {
    cmd_buf->hdr.cmd_id = DS_CMD_PDN_NW_CIOT_CAP_CHG;

    memscpy(cmd_buf->cmd_payload_ptr, sizeof(sys_neg_ciot_cap),
            &neg_ciot_cap, sizeof(sys_neg_ciot_cap));
    ds_put_cmd(cmd_buf);
  }
} /* ds_pdn_hdlr_set_cp_ciot_cap */

/*===========================================================================
FUNCTION  DS_PDN_HDLR_SYS_CHANGE_HANDLER

DESCRIPTION

  Called by 3G DSMGR when the serving system changes.


PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_hdlr_sys_change_handler
(
  sys_sys_mode_e_type    old_mode,
  sys_sys_mode_e_type    new_mode,
  sys_srv_domain_e_type  old_srv_domain,
  sys_srv_domain_e_type  new_srv_domain
)
{
  DS_MSG0(MSG_LEGACY_HIGH,"Sys change handler");
  if(ds_pdn_psm_get_restoration_state ())
  {
    ds_pdn_psm_set_restoration_state(FALSE);
    ds_pdn_psm_print_bearer_info();
    if(ds_pdn_hdlr_get_current_network_mode() == SYS_SYS_MODE_LTE)
    {
      ds_pdn_psm_send_attach_complete_indication();
    }
    else if(ds_pdn_hdlr_get_current_network_mode() == SYS_SYS_MODE_GSM)
    {
      ds_gprs_bearer_cntxt_activate_on_full_srv();
    }
  }


  /*---------------------------------------------------------------------
    Advertise throttling information to MPPM and APM
  ---------------------------------------------------------------------*/

  if (ds_pdn_nv_manager_get_enable_pdn_throt_per_rat())
  {     
    if (dssysmgr_get_srv_status() == SYS_SRV_STATUS_SRV || 
        ds_pdn_hdlr_get_current_network_mode() == SYS_SYS_MODE_LTE)
    {
      ds_3gpp_pdn_throt_sm_advertise_blocked_apns_to_external_clients();
    }
  }

  if (new_mode == SYS_SYS_MODE_LTE || new_mode == SYS_SYS_MODE_GSM)
  {
    if(new_srv_domain == SYS_SRV_DOMAIN_PS_ONLY ||
       new_srv_domain == SYS_SRV_DOMAIN_CS_PS)
    {
     /*------------------------------------------------------------------------- 
        If old_srv_domain is SYS_SRV_DOMAIN_CS_ONLY or SYS_SRV_DOMAIN_NO_SRV or
        SYS_SRV_DOMAIN_CAMPED and new srv_domain is PS srv domain
        reset pdn blocked in apn reject sm
       ------------------------------------------------------------------------*/
      if((old_srv_domain  == SYS_SRV_DOMAIN_CS_ONLY) || 
         (old_srv_domain  == SYS_SRV_DOMAIN_NO_SRV)  ||
         (old_srv_domain  == SYS_SRV_DOMAIN_CAMPED))
      {
        /* Reset PDN blocked in APN reject SM for new network attach */
        ds_3gpp_dam_set_pdn_blocked(FALSE);
      }

      /* Reset Service request throttling on RAT change only for a specific config */
      if (ds_3gpp_srv_req_throt_t3402_is_enabled())
      {
        if ( (old_mode == SYS_SYS_MODE_LTE ||
              old_mode == SYS_SYS_MODE_GSM) &&
             (old_mode != new_mode) )
        {

          DS_MSG0(MSG_LEGACY_HIGH, 
                  "Sys mode changed, reset Service Request throttling SM");
          ds_bearer_mgr_reset_srv_req_throt(new_mode);
        }
      } 
    }
  }   

  return;
}

/*===========================================================================
FUNCTION      ds_pdn_at_init_service_mode

DESCRIPTION   This function sets service mode for AT.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_pdn_at_init_service_mode()
{
  sys_sys_mode_e_type         sys_mode = SYS_SYS_MODE_NO_SRV;
  cm_mode_pref_e_type         cm_mode_pref;
  /*-----------------------------------------------------------*/

  ds_pdn_nv_manager_read_mode_pref_from_nv();
  
  cm_mode_pref = dssysmgr_get_mode_pref();
  
  sys_mode = dssysmgr_mode_pref_to_sys_mode(cm_mode_pref);
  #ifdef FEATURE_DATA_ATCOP
  dsat_init_service_mode(sys_mode);
  #endif /* FEATURE_DATA_ATCOP */
  return;
}
/*===========================================================================
FUNCTION DS_3GPP_PDN_GET_MAX_SIMUL_ACTIVE_PDP_CONTEXT

DESCRIPTION
  This function gets the maximum number of sumultanous active pdp contexts for
  the current RAT and PLMN.

PARAMETERS 
  rat_type 
  subs_id          : Subscription Id

DEPENDENCIES
  None.

RETURN VALUE  
  Number of simultanous active pdp contexts.

SIDE EFFECTS
  None.
===========================================================================*/
uint8 ds_3gpp_pdn_get_max_simul_active_pdp_context
(
  ds_3gpp_throt_rat_e_type           rat_type
)
{
  uint8                                  ret_val = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if((rat_type != DS_3GPP_THROT_RAT_LTE) &&
       (rat_type != DS_3GPP_THROT_RAT_UMTS))
  {
    return ret_val;
  }

    ret_val = ds_pdn_hdlr_info.max_simul_active_pdp_info[rat_type].max_simul_active_pdp_context;  
  return ret_val;
}/* ds_3gpp_pdn_get_max_simul_active_pdp_context */

/*===========================================================================
FUNCTION DS_3GPP_PDN_SET_MAX_SIMUL_ACTIVE_PDP_CONTEXT

DESCRIPTION
  This function sets the maximum number of sumiltanous active pdp contexts 
  for the current PLMN and RAT.
  pdn connection.

PARAMETERS 
  rat_type 
  max_active_pdp_context
  subs_id          : Subscription Id 
  
DEPENDENCIES
  None.

RETURN VALUE  
  void

SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_pdn_set_max_simul_active_pdp_context
(
  ds_3gpp_throt_rat_e_type           rat_type,
  uint8                              max_active_pdp_context
)
{
  sys_plmn_id_s_type                     plmn_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    if((rat_type != DS_3GPP_THROT_RAT_LTE) &&
       (rat_type != DS_3GPP_THROT_RAT_UMTS))
    {
      return;
    }

    ds_pdn_hdlr_get_current_plmn(&plmn_id);

    ds3g_print_plmn_id((void*)&plmn_id);

    ds_pdn_hdlr_info.max_simul_active_pdp_info[rat_type].max_simul_active_pdp_context = 
      max_active_pdp_context;
    memscpy((void*)&(ds_pdn_hdlr_info.max_simul_active_pdp_info[rat_type].plmn_id),
             sizeof(sys_plmn_id_s_type),
             (void*)&plmn_id, 
             sizeof(sys_plmn_id_s_type));
    return;
 
} /* ds_3gpp_pdn_set_max_simul_active_pdp_context */

/*===========================================================================
FUNCTION DS_3GPP_PDN_RESET_MAX_SIMUL_ACTIVE_PDP_CONTEXT

DESCRIPTION
  This function sets the maximum number of sumiltanous active pdp contexts
  for the current PLMN and RAT.
  pdn connection.

PARAMETERS
  max_active_pdp_context
  subs_id          : Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_pdn_reset_max_simul_active_pdp_context
(
  void  
)
{
  ds_3gpp_throt_rat_e_type               rat_type = DS_3GPP_THROT_RAT_MIN;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for (rat_type = DS_3GPP_THROT_RAT_LTE;rat_type < DS_3GPP_THROT_RAT_MAX;
         rat_type++)
  {
    ds_pdn_hdlr_info.max_simul_active_pdp_info[rat_type].max_simul_active_pdp_context = 
     DS_3GPP_MAX_BEARER_CONTEXT;
   
    memset(&(ds_pdn_hdlr_info.max_simul_active_pdp_info[rat_type].plmn_id),
        0,
        sizeof(sys_plmn_id_s_type));
  }
return;
} /* ds_3gpp_pdn_set_max_simul_active_pdp_context */

/*===========================================================================
FUNCTION DS_3GPP_PDN_RESET_MAX_SIMUL_ACTIVE_PDP_CONTEXT_ON_PLMN_CHANGE

DESCRIPTION
  This function sets the maximum number of sumiltanous active pdp
  contexts for the current pdn connection.

PARAMETERS
  max_active_pdp_context
  subs_id
  
DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_pdn_reset_max_simul_active_pdp_context_on_plmn_change
(
  ds_3gpp_throt_rat_e_type               rat_type,
  sys_plmn_id_s_type                     plmn_id
)
{
  /*----------------------------------------------------------------------*/

  if((rat_type != DS_3GPP_THROT_RAT_LTE) &&
     (rat_type != DS_3GPP_THROT_RAT_UMTS))
  {
    return;
  }
  
  if (memcmp(&ds_pdn_hdlr_info.max_simul_active_pdp_info[rat_type].plmn_id, 
             &plmn_id, sizeof(sys_plmn_id_s_type)) != 0)
  {
    ds_pdn_hdlr_info.max_simul_active_pdp_info[rat_type].max_simul_active_pdp_context 
       = DS_3GPP_MAX_BEARER_CONTEXT;

    memset(&(ds_pdn_hdlr_info.max_simul_active_pdp_info[rat_type].plmn_id),
         0,
         sizeof(sys_plmn_id_s_type));
  }
  return;
} /* ds_3gpp_pdn_reset_max_simul_active_pdp_context_on_plmn_change */

/*===========================================================================
FUNCTION DS_3GPP_PDN_PROCESS_NEW_PLMN

DESCRIPTION
  This wrapper is to invoke a set of functions to advertise/clear blocked
  APN when PLMN changes.

PARAMETERS
  subs_id       -- Subscription ID where mode_pref_change occurred.
  plmn_id       -- new PLMN 
  network_mode  -- current RAT

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/

void ds_3gpp_pdn_process_new_plmn
(
  sys_plmn_id_s_type      plmn_id,
  sys_sys_mode_e_type     network_mode
)
{
  ds_3gpp_pdn_throt_clr_on_plmn_change(network_mode, plmn_id);
  ds_pdn_roaming_inform_new_plmn_info( plmn_id);
  ds_3gpp_pdn_throt_advertise_blocked_apns_with_plmn ( plmn_id);
  
  return;
} /*ds_3gpp_pdn_process_new_plmn */

/*===========================================================================
FUNCTION DS3GPP_SET_DS_LTE_FLAG

DESCRIPTION
  This function sets the lte flag based on whether we have disabled or 
  enabled LTE from ds module.   This is based on the boolean variable 

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE


SIDE EFFECTS
  None.

===========================================================================*/
void ds3gpp_set_ds_lte_flag
(
  boolean                 lte_flag
)
{
  if(lte_flag)
  { 
    ds_pdn_hdlr_info.current_ds_lte_flag = DS_PDN_LTE_ENABLED; 
  }
  else
  {
    ds_pdn_hdlr_info.current_ds_lte_flag = DS_PDN_LTE_DISABLED; 
  } 
  
  DS_MSG1(MSG_LEGACY_HIGH,"current_ds_lte_flag is %d",
                      ds_pdn_hdlr_info.current_ds_lte_flag);
} /* ds3gpp_set_ds_lte_flag */

/*===========================================================================
FUNCTION DS3GPP_IS_LTE_FLAG_DISABLED

DESCRIPTION
  This function returns TRUE if LTE Flag is disabled

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  boolean

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds3gpp_is_lte_flag_disabled
(
  void
)
{
  return (ds_pdn_hdlr_info.current_ds_lte_flag == DS_PDN_LTE_DISABLED);
} /* ds3gpp_is_lte_flag_disabled */


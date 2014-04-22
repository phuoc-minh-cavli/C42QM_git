#ifndef DS_EPS_PDN_MANAGER_H
#define DS_EPS_PDN_MANAGER_H

/*===========================================================================
                      DS_EPS_PDN_MANAGER.H
DESCRIPTION
This file exposes LTE specific PDN manager through APIs and data structures.

Copyright (c) 2018 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/3gpp/pdnmgr/inc/ds_eps_pdn_manager.h#1 $
  $DateTime: 2020/03/17 08:03:16 $$Author: pwbldsvc $

when           who    what, where, why
--------    ---    ----------------------------------------------------------
08/09/17    sd      First checkin
===========================================================================*/

#include "ds_pdn_defs.h"
#include "ds_pdn_manager.h"


/*--------------------------------------------------------------------------- 
  DS_EPS_PROCESSING_GET_PDN_CONN_IND is the state of attach SM when we are
  servicing a GET_PDN_CONN_REQ from NAS.
  The attach SM state is changed to DS_EPS_PROCESS_GET_PDN_CONN_IND when we have a
  cached sequence number and there is a  need to re originate a PDN_CONN_REQ
  with the cached sequence number.
  ---------------------------------------------------------------------------*/
typedef enum 
{
   DS_EPS_ATTACH_STATE_INVALID = -1,
   DS_EPS_ATTACH_STATE_NULL,
   DS_EPS_CACHED_GET_PDN_CONN_IND,
   DS_EPS_REDIAL_GET_PDN_CONN_IND,
   DS_EPS_PROCESSING_GET_PDN_CONN_IND, 
   DS_EPS_ATTACHED,
   DS_EPS_MAX_STATES
} ds_eps_attach_state_e;

/*--------------------------------------------------------------------------- 
  Structure to store the failure reason (if the current attach fails)
  The recorded values are to be used during the next attempt attempt 
  ---------------------------------------------------------------------------*/
typedef struct
{
  boolean                          is_attach_fail;
   /** Current attach fail status*/
  ds_net_down_reason_type          down_reason;
  /** Attach failure reason */
}ds_eps_prev_attach_status_type;

typedef struct
{
  boolean                          ota_attach_performed;
  ds_eps_attach_state_e            attach_state;  
  uint8                            seq_num;
  uint8                            cached_seq_num; 
  boolean                          is_gw_originated;
  ds_eps_prev_attach_status_type   prev_attach_status;
  boolean                          responding_to_page;
  boolean                          cached_paging_response;
  boolean                          attach_wo_pdn_supported;
  boolean                          cached_attach_wo_pdn_supported;
} ds_eps_attach_sm_s;

/*-------------------------------------------------------------------
   Attach fall back IP type. Used during LTE dual ip attach fall back
  -------------------------------------------------------------------*/
typedef enum
{
  DS_EPS_ATTACH_FB_IP_TYPE_MIN =-1,
    /** Min value. Used only for Min value check after NV read*/
  DS_EPS_ATTACH_FB_IP_TYPE_IPV4,
    /** Attach Fall back IP type IPV4 */
  DS_EPS_ATTACH_FB_IP_TYPE_IPV6,
    /** Attach Fall back type IPV6 */
  DS_EPS_ATTACH_FB_IP_TYPE_MAX
    /** Max Value: Used only for Max value check after NV read */
}ds_eps_attach_fb_type;

typedef struct
{
  boolean                    fallback_enabled;
  ds_eps_attach_fb_type      fallback_ip_type;
}ds_eps_attach_fb_info_type;

typedef enum 
{
  DS_EPS_BRINGUP_ATTACH_PDN_FAIL = 0,
  DS_EPS_BRINGUP_ATTACH_PDN_PEND_REDIAL,
  DS_EPS_BRINGUP_ATTACH_PDN_SUCCESS   
} ds_eps_bringup_attach_pdn_result_e;

/*-------------------------------------------------------------------------- 
  Local typedef to pass LTE attach SM parameters
  It currently contains
  1. Seq_num - Attach req or not
  2. responding to page - Indicates if the attach request is sent out
                          because of a page from the network
  3. Attach w/o PDN support                          
--------------------------------------------------------------------------*/
typedef struct
{
  boolean responding_to_page;
  uint8   seq_num;
  boolean attach_wo_pdn_supported;
}ds_eps_attach_sm_info_type;

/*-------------------------------------------------------------------------- 
  Bearer deactivation params used for both L and G
  1. Bearer deactivation indication.
  2. PDN reject.
  3. PDN connectivity failure.
--------------------------------------------------------------------------*/
struct ds_bearer_deact_params_type
{
  // L specific
  eps_bearer_id_T         eps_bearer_id; 
  esm_cause_T             esm_cause; 
  lte_nas_local_cause_T   esm_local_cause ;
  emm_failure_type        emm_failure_cause;
  
  pdn_reject_ind_params_s_type pdn_reject_ind;
  protocol_cfg_options_T  protocol_config_options; 
  ext_protocol_cfg_options_T  ext_protocol_conf_opt;

  // Common for L and G  
  timer_3gpp_type3        backoff_timer;
  reattempt_ind_T         reattempt_indicator; 

  // G specific  
  byte                          connection_id;
  pdp_reject_ind_params_s_type  pdp_reject_ind;
  cause_enum_type_T             cause_type;
  pdp_cause_T                   cause;  
};


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
);

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
);

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
);

#define DS_EPS_ATTACH_SM_IS_UE_ATTACHED  \
        (ds_eps_pdn_attach_sm_get_state() ==  DS_EPS_ATTACHED)

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
);

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
uint32 ds_eps_pdn_cntxt_get_t3402_timer_val();

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

/*===========================================================================
FUNCTION  DS_EPS_PDN_MGR_DEFAULT_BEARER_ACTIVATE_IND

DESCRIPTION
  This function accepts or rejects the the EPS default bearer activate request.

PARAMETERS
  info_ptr_i:  Default bearer activate information

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_eps_pdn_mgr_default_bearer_activate_ind
(
  emm_act_default_bearer_context_request_ind_s_type   *info_ptr
);

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
);

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
);

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
);

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
);

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
);

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
);

/*===========================================================================
FUNCTION      DS_EPS_PDN_MGR_GET_LTE_ATTACH_PARAMS

DESCRIPTION
  Updates the Attach profile APN info when an IOCTL query is received

PARAMETERS
  ioctl_name[in]  -      Operation name
  arg_val_ptr[in] -      Pointer to operation specific argument
  ps_errno [out]  -       Specifies type of error in case of error.
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
);

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
);

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
  ds_pdn_apm_ind_pay_load     *ind_payload
);


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
);

#endif /* DS_EPS_PDN_MANAGER_H */

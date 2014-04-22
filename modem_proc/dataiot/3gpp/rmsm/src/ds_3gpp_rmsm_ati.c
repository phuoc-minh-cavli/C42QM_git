/*===========================================================================
                        D S _3 G P P _ R M S M _ A T I . C 

DESCRIPTION

  This files contains the internal function for ATCoP 
  mode handler interactions. 
  

EXTERNALIZED FUNCTIONS

Copyright (c) 2009 - 2020 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/dataiot.mpss/2.0/3gpp/rmsm/src/ds_3gpp_rmsm_ati.c#9 $ 
  $DateTime: 2021/11/11 20:34:11 $ $Author: pwbldsvc $

when(mm/dd/yy) who    what, where, why
--------       ---   -----------------------------------------------------------
12/10/18       aks    3GPP MH First checkin for RMSM AT interface to CT.
===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "ds_headers.h"
#include <stringl/stringl.h>
#include "ds_pdn_manager.h"
#include "ds_3gpp_rmsm_ati.h"
#include "ds_crit_sect.h"
#include "ds_bearer_manager.h"
#include "ds_profile.h"
#include "ds_rmsmi.h"
#include "ps_metai_info.h"
#include "ds_pdn_cp_data_hdlr.h"
#include "ds_fwki.h"
#include "ps_ip6_hdr.h"
#include "ds_pdn_nv_manager.h"


/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                            INTERNAL DEFINITIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/
#define DS_3GPP_RMSM_ATI_MIN_DATA_LEN   21
#define DS_3GPP_RMSM_ATI_MAX_FRAME_SIZE 1500

typedef struct
{
  ds_3gpp_rmsm_at_instance_type  ds_3gpp_rmsm_at_instance; /* RMSM_AT instance */
  ds_fwk_clnt_handle_type        client_handle;            /* FWK client handle*/
  ds_pdn_event_enum_type         event;                    /* PDN event   */
  ds_pdn_event_info_u_type       event_info;               /* Event info       */
  boolean                        result;                   /* result of reg    */
} ds_3gpp_rmsm_at_type;
   
/*===========================================================================
                            Forward Declarations
===========================================================================*/
  ds_3gpp_ati_pdp_state_type ds_3gpp_ati_pdp_state[DS_3GPP_RMSM_AT_UM_MAX_INSTANCES] = {{NULL}};
  rex_crit_sect_type         ds_3gpp_rmsm_crit_sect;
  ds_3gpp_ati_rx_data_info_type  ds_3gpp_ati_rx_data_info = {NULL};

/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                          FUNCTION DEFINTIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/

void ds_3gpp_rmsm_ati_um_inst_down_handler
(
  ds_3gpp_rmsm_at_type*  instance
);
void ds_3gpp_rmsm_ati_um_inst_up_handler
(
  ds_3gpp_rmsm_at_type*  instance
);

void ds_rmsm_ati_um_event_handler_cb
(
  ds_fwk_clnt_handle_type    client_handle,
  ds_pdn_event_enum_type     event,
  ds_pdn_event_info_u_type   event_info,
  void                       *user_data_ptr
);

ds_3gpp_rmsm_at_instance_type ds_3gpp_rmsm_ati_get_inst_num_from_um_inst
(
  ds_at_pdn_instance* um_inst_ptr
);

boolean  ds_3gpp_rmsm_ati_allocate_instance
(
  uint32                          profile_number,
  ds_3gpp_rmsm_at_instance_type   *instance
);

boolean ds_3gpp_rmsm_ati_generate_ip_frame
(
  dsm_item_type **ip_frame,
  uint16        total_data_len
);
boolean ds_3gpp_rmsm_ati_generate_ipv6_frame
(
  dsm_item_type **ip_frame,
  uint16        total_data_len
);
boolean ds_3gpp_rmsm_ati_send_data
(
  ds_fwk_s_type                    *ds_fwk_ptr,
  uint32                           *data_len
);

void ds_3gpp_rmsm_at_data_cmd_hdlr
(
  ds_command_type *ds_cmd_ptr
);

boolean ds_3gpp_rmsm_ip_tx_um_data_cb
(
  ps_sig_enum_type sig,
  void             *pkt_instance
);

boolean ds_3gpp_rmsm_ati_pdp_state_type_validate
(
  ds_3gpp_ati_pdp_state_type     *pdp_ptr
);


/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                        INTERNAL FUNCTION DEFINTIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/

/*===========================================================================
  MACRO DS_3GPP_RMSM_SET_FLOW_STATE
  
  DESCRIPTION   
    Given the call instance, sets the flow enabled flag
  
  DEPENDENCIES  
    None.
  
  RETURN VALUE  
    See Descritpion.
    
  SIDE EFFECTS  
    None.
  ===========================================================================*/
#define DS_3GPP_RMSM_SET_FLOW_STATE(i,flag)\
    ds_3gpp_ati_pdp_state[i].ds_3gpp_ati_pdp_state_type_dyn_p->um_flow_enabled = flag
  
  /*===========================================================================
  MACRO DS_3GPP_RMSM_GET_FLOW_STATE
  
  DESCRIPTION   
    Given the call instance, gets the flow enabled flag
  
  DEPENDENCIES  
    None.
  
  RETURN VALUE  
    See Descritpion.
    
  SIDE EFFECTS  
    None.
  ===========================================================================*/
#define DS_3GPP_RMSM_GET_FLOW_STATE(i)\
    ds_3gpp_ati_pdp_state[i].ds_3gpp_ati_pdp_state_type_dyn_p->um_flow_enabled
  
  /*===========================================================================
  MACRO DS_3GPP_RMSM_GET_PENDING_DATA
  
  DESCRIPTION   
    Given the call instance, gets the bytes of data pending
  
  DEPENDENCIES  
    None.
  
  RETURN VALUE  
    See Descritpion.
    
  SIDE EFFECTS  
    None.
  ===========================================================================*/
#define DS_3GPP_RMSM_GET_PENDING_DATA(i)\
    ds_3gpp_ati_pdp_state[i].ds_3gpp_ati_pdp_state_type_dyn_p->pending_data


/*===========================================================================
  FUNCTION   DS_3GPP_RMSM_GET_INFO_PTR_FROM_INSTANCE
  
  DESCRIPTION
    Returns the pointer to 3GPP RMSM_AT instance for the specified instance 
    
  NOTE: THIS IS A RESTRICTED FUNCTION AND IS INTENDED TO BE USED BY 
  DS_3GPP_RMSM_AT MODULES ONLY
  
  RETURN VALUE
    Pointer to the RMSM AT info structure if a valid instance is passed, else
    NULL
  
  
  DEPENDENCIES
    None
  
  SIDE EFFECTS
    None
  ===========================================================================*/
ds_3gpp_ati_pdp_state_type *  ds_3gpp_rmsm_get_info_ptr_from_instance
(
  ds_3gpp_rmsm_at_instance_type instance
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( (instance >=0) && (instance < DS_3GPP_RMSM_AT_UM_MAX_INSTANCES ) )
  {
    if(ds_3gpp_ati_pdp_state[instance].in_use == TRUE)
    {
      return &ds_3gpp_ati_pdp_state[instance];
    }
  }
  DS_MSG0(MSG_LEGACY_HIGH,"No valid instance, return NULL ");
  return NULL;

} /* ds_3gpp_rmsm_get_info_ptr_from_instance */

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_ATI_ACTIVATE_INST

  DESCRIPTION
    This function allocates and bringup V6 instance
  
  RETURN VALUE
    FALSE  : if failure
    TRUE : if Success
  
  DEPENDENCIES
    None
    
  SIDE EFFECTS
    None
===========================================================================*/
dsat_result_enum_type  ds_3gpp_rmsm_ati_activate_inst
( 
  ds_3gpp_ati_pdp_state_type     *pdp_ptr,
  int8                           profile_number,
  ds_rmsm_ati_inst_type_e        inst_type
)
{
  int                            result      = -1;
  ds_policy_info_type            policy_info = {0};
  ds_fwk_response_info_s_type    resp_info   = {0};
  ds_fwk_clnt_handle_type        clt_hdl     = DS_FWK_INVALID_CLNT_HANDLE;
  ds_profile_type_s              profile_info = {{0}};  
  ds_profile_status_etype        res = DS_PROFILE_RESULT_SUCCESS;
  ds_mo_ex_user_info_s           *mo_ex_data = NULL;
  ds_fwk_clnt_info_s_type        clt_info = {0};
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  if(!ds_3gpp_rmsm_ati_pdp_state_type_validate(pdp_ptr))
  {
    DS_MSG0(MSG_LEGACY_ERROR,"Not a valid instance ds_3gpp_rmsm_ati_pdp_state");
    return DSAT_ERROR;
  } 
  DS_MSG1(MSG_LEGACY_HIGH,"Activate inst of PDP type %d",inst_type);
  res = ds_profile_setting_from_profile_id((int8)profile_number,&profile_info);
  if(res != DS_PROFILE_RESULT_SUCCESS)
  {
    DS_MSG0(MSG_LEGACY_ERROR,"Unable to read profile params from cache");
    return DSAT_ERROR;
  }
  
  if(pdp_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->um_inst_ptr[inst_type] == NULL)
  {
    DS_SYSTEM_HEAP_MEM_ALLOC(pdp_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->um_inst_ptr[inst_type],
                  sizeof(ds_at_pdn_instance), ds_at_pdn_instance* );
    if(pdp_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->um_inst_ptr[inst_type] == NULL)
    {
      DS_MSG0(MSG_LEGACY_ERROR,"Memory allocation failed");
      return DSAT_ERROR;
    }
    memset(pdp_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->um_inst_ptr[inst_type],
                  0, sizeof(ds_at_pdn_instance));
  }
  
  policy_info.length = strlen(profile_info.context.apn);
  memscpy(policy_info.name, policy_info.length ,
  	      profile_info.context.apn,policy_info.length);
  
  /*Check for MO exception flag set by user*/
  if(pdp_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->user_info_ptr)
  {
    mo_ex_data = (ds_mo_ex_user_info_s*)
                   pdp_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->user_info_ptr;
    policy_info.mo_excp_data_cap = mo_ex_data->is_mox_data_pending;
  }

  switch(inst_type)
  {
    case DS_RMSM_ATI_V4_INST:
      policy_info.family = IPV4_ADDR;
      clt_hdl = pdp_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->v4_clt_hdl;
      break;
    case DS_RMSM_ATI_V6_INST:
      policy_info.family = IPV6_ADDR;
      clt_hdl = pdp_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->v6_clt_hdl;
      break;
    case DS_RMSM_ATI_NONIP_INST:
      policy_info.family = NON_IP_ADDR;
      clt_hdl = pdp_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->nonip_clt_hdl;
      break;
    default:
      DS_MSG1(MSG_LEGACY_ERROR,"Invalid instance PDP type [%d]",inst_type);
      return DSAT_ERROR;
  }

  /*---------------------------------------------------------------------
    Bring up the Um instance. If there is an error, 
    delete the allocated instance and return FALSE.
  ---------------------------------------------------------------------*/
  if ((result = ds_fwk_bringup_pdn_by_policy(DS_FWK_CLIENT_ATCOP,policy_info,
                                    clt_hdl,&resp_info)) == 0 )
  {
    /* we need to make sure before that the context is already up or not */
    /* if already up , return OK */
    DS_MSG1(MSG_LEGACY_HIGH,"PDP Instance %d is UP",inst_type);
    pdp_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->um_inst_ptr[inst_type]->in_use = TRUE;
    pdp_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->um_inst_ptr[inst_type]->state = DS_INSTANCE_STATE_UP;
    ds_fwk_get_clnt_info_from_handle(DS_FWK_CLIENT_ATCOP,clt_hdl,&clt_info);
    pdp_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->um_inst_ptr[inst_type]->fw_inst = clt_info.fw_inst;             
  }
  else if ( result == DS_EWOULDBLOCK )
  {
    DS_MSG1(MSG_LEGACY_HIGH,"PDP Instance %d is being brought UP", inst_type);
    return DSAT_ASYNC_CMD;
  }
  else
  {
    DS_MSG1(MSG_LEGACY_HIGH,"Failed to bringup PDP instance %d, return FALSE", inst_type);
    DS_SYSTEM_HEAP_MEM_FREE(pdp_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->um_inst_ptr[inst_type]);
    pdp_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->um_inst_ptr[inst_type] = NULL;
    return DSAT_ERROR; 
  }
  
  return DSAT_OK;
}/* ds_3gpp_rmsm_ati_activate_inst*/

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_ATI_ACTIVATE_V4_INST

  DESCRIPTION
    This function allocates and brings up V4 instance.
  
  RETURN VALUE
    FALSE  : if failure
    TRUE : if Success
  
  DEPENDENCIES
    None
    
  SIDE EFFECTS
    None
===========================================================================*/
dsat_result_enum_type  ds_3gpp_rmsm_ati_activate_v4_inst
( 
  ds_3gpp_ati_pdp_state_type     *pdp_ptr,
  int8                           profile_number
)
{  
  return ds_3gpp_rmsm_ati_activate_inst(pdp_ptr,profile_number,DS_RMSM_ATI_V4_INST);
}/* ds_3gpp_rmsm_ati_activate_v4_inst */

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_ATI_ACTIVATE_V6_INST

  DESCRIPTION
    This function allocates and bringup V6 instance
  
  RETURN VALUE
    FALSE  : if failure
    TRUE : if Success
  
  DEPENDENCIES
    None
    
  SIDE EFFECTS
    None
===========================================================================*/
dsat_result_enum_type  ds_3gpp_rmsm_ati_activate_v6_inst
( 
  ds_3gpp_ati_pdp_state_type     *pdp_ptr,
  int8                           profile_number
)
{
  return ds_3gpp_rmsm_ati_activate_inst(pdp_ptr,profile_number,DS_RMSM_ATI_V6_INST);
}/* ds_3gpp_rmsm_ati_activate_v6_inst*/

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_ATI_ACTIVATE_NONIP_INST

  DESCRIPTION
    This function allocates and bringup V6 instance
  
  RETURN VALUE
    FALSE  : if failure
    TRUE : if Success
  
  DEPENDENCIES
    None
    
  SIDE EFFECTS
    None
===========================================================================*/
dsat_result_enum_type  ds_3gpp_rmsm_ati_activate_nonip_inst
( 
  ds_3gpp_ati_pdp_state_type     *pdp_ptr,
  int8                           profile_number
)
{
  return ds_3gpp_rmsm_ati_activate_inst(pdp_ptr,profile_number,DS_RMSM_ATI_NONIP_INST);
}/* ds_3gpp_rmsm_ati_activate_v6_inst*/

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_ATI_TEAR_DOWN_INST

  DESCRIPTION
    This function tear down instance corresponding to passed inst type
  RETURN VALUE
    FALSE  : if failure
    TRUE : if Success
  
  DEPENDENCIES
    None
    
  SIDE EFFECTS
    None
===========================================================================*/
dsat_result_enum_type ds_3gpp_rmsm_ati_tear_down_inst
(
  ds_3gpp_ati_pdp_state_type     *pdp_ptr,
  int8                           profile_num,
  ds_rmsm_ati_inst_type_e        inst_type
)
{
  ds_3gpp_rmsm_at_instance_type  instance = 0;
  int16                          ret_val  = DS_EWOULDBLOCK;
  dsat_result_enum_type          result   = DSAT_ASYNC_CMD;
  ds_at_pdn_instance*            um_inst_ptr = NULL;
  ds_fwk_clnt_handle_type        clt_hdl = DS_FWK_INVALID_CLNT_HANDLE;
  ds_fwk_response_info_s_type    resp_info = {0};
  ds_pdn_mgr_s*                  pdn_mgr_p = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   if(!ds_3gpp_rmsm_ati_pdp_state_type_validate(pdp_ptr))
  {
    DS_MSG0(MSG_LEGACY_ERROR,"Not a valid AT instance ds_3gpp_rmsm_ati_pdp_state");
    return DSAT_ERROR;
  } 
  
  if(pdp_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->um_inst_ptr[inst_type] == NULL)
  {
    return DSAT_ERROR;
  }
  
  um_inst_ptr = pdp_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->um_inst_ptr[inst_type];

  instance =  ds_3gpp_rmsm_ati_get_inst_num_from_um_inst (um_inst_ptr);

  DS_MSG1(MSG_LEGACY_HIGH,"instance [%d] is being brought down ",inst_type);
  switch(inst_type)
  {
    case DS_RMSM_ATI_V4_INST:
      clt_hdl = pdp_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->v4_clt_hdl;
      break;
    case DS_RMSM_ATI_V6_INST:
      clt_hdl = pdp_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->v6_clt_hdl;
      break;
    case DS_RMSM_ATI_NONIP_INST:
      clt_hdl = pdp_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->nonip_clt_hdl;
      break;
    default:
      DS_MSG1(MSG_LEGACY_ERROR,"Invalid PDP Instance type [%d]",inst_type);
      return DSAT_ERROR;
  }
  
  /*---------------------------------------------------------------------
    Tear down the v4 UM instance first.
    Checking for the return code as the instance could already be down.
  ---------------------------------------------------------------------*/
  ret_val = ds_fwk_tear_down_pdn(DS_FWK_CLIENT_ATCOP,clt_hdl,&resp_info);
  if ( ( ret_val == 0 ) || (ret_val != DS_EWOULDBLOCK))
  {
    /*-------------------------------------------------------------------
       If the UM instance is already down, we should de-allocate
       the instance if not done yet and return OK to ATCoP immediately.
    -------------------------------------------------------------------*/
    DS_MSG1(MSG_LEGACY_HIGH,"UM instance is down for clt hdl %d ",clt_hdl);

    pdn_mgr_p = ds_pdn_mgr_get_pdn_from_profile_id(profile_num);
    if((pdn_mgr_p != NULL) && (pdn_mgr_p->is_default == TRUE))
    {
      DS_MSG0(MSG_LEGACY_HIGH,"Default profile torn down req.Return OK");
      result = DSAT_OK;
    }
      
    DS_SYSTEM_HEAP_MEM_FREE(pdp_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->um_inst_ptr[inst_type]);
    pdp_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->um_inst_ptr[inst_type] = NULL;
    
    return result;

  }
  else
  {
    /*-------------------------------------------------------------------
       Wait for PDN_INST_DOWN event and then send final response 
    -------------------------------------------------------------------*/
    DS_MSG1(MSG_LEGACY_HIGH,"Waiting for PDP Inst %d down event ",inst_type);
    return DSAT_ASYNC_CMD;
  }

}


/*===========================================================================
  FUNCTION DS_3GPP_RMSM_ATI_TEAR_DOWN_V4_INST

  DESCRIPTION
    This function tear down a V4 instance
  RETURN VALUE
    FALSE  : if failure
    TRUE : if Success
  
  DEPENDENCIES
    None
    
  SIDE EFFECTS
    None
===========================================================================*/
dsat_result_enum_type ds_3gpp_rmsm_ati_tear_down_v4_inst
(
  ds_3gpp_ati_pdp_state_type     *pdp_ptr,
  int8                           profile_num
)
{
  return ds_3gpp_rmsm_ati_tear_down_inst(pdp_ptr,profile_num,DS_RMSM_ATI_V4_INST); 
}
/* ds_3gpp_rmsm_ati_tear_down_v4_inst */

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_ATI_TEAR_DOWN_V6_INST

  DESCRIPTION
    This function tear down a V6 instance
  
  RETURN VALUE
    FALSE  : if failure
    TRUE : if Success
  
  DEPENDENCIES
    None
    
  SIDE EFFECTS
    None
===========================================================================*/
dsat_result_enum_type ds_3gpp_rmsm_ati_tear_down_v6_inst
(
  ds_3gpp_ati_pdp_state_type     *pdp_ptr,
  int8                           profile_num
)
{
  return ds_3gpp_rmsm_ati_tear_down_inst(pdp_ptr,profile_num,DS_RMSM_ATI_V6_INST);
}
/* ds_3gpp_rmsm_ati_tear_down_v6_instance */
/*===========================================================================
  FUNCTION DS_3GPP_RMSM_ATI_TEAR_DOWN_V6_INST

  DESCRIPTION
    This function tear down a V6 instance
  
  RETURN VALUE
    FALSE  : if failure
    TRUE : if Success
  
  DEPENDENCIES
    None
    
  SIDE EFFECTS
    None
===========================================================================*/

dsat_result_enum_type ds_3gpp_rmsm_ati_tear_down_nonip_inst
(
  ds_3gpp_ati_pdp_state_type     *pdp_ptr,
  int8                           profile_num
)
{
  return ds_3gpp_rmsm_ati_tear_down_inst(pdp_ptr,profile_num,DS_RMSM_ATI_NONIP_INST);
}

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_ATI_CGACT_ACTIVATION

  DESCRIPTION
    This function activates the Profile, if not already activated .
    If the profile is already activated return SUCCESS immediately.

  RETURN VALUE
    DSAT_OK -- Profile is already activated
    DSAT_ERROR -- In case of any FAILURE
    DSAT_ASYNC_CMD -- Profile is being activated.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
dsat_result_enum_type  ds_3gpp_rmsm_ati_cgact_activation
(
  uint32                  profile_number,
  void *                  user_info_ptr
)
{
  ds_profile_status_etype        result = DS_PROFILE_RESULT_SUCCESS;
  ds_3gpp_rmsm_at_instance_type  instance = 0;
  ds_3gpp_ati_pdp_state_type     *at_pdp_state_ptr = NULL;
  dsat_result_enum_type          v4_act_result = DSAT_ERROR, v6_act_result = DSAT_ERROR,
                                 nonip_act_result = DSAT_ERROR;
  ds_local_err_val_e_type        err_val = DS_LOCAL_ERR_VAL_NONE;
  ds_profile_type_s              profile_info = {{0}};
  ds_profile_pdp_type_e          pdp_type = DS_PROFILE_PDP_MAX;
  boolean                        send_resp = FALSE; 
  dsat_rmsm_cmd_type             at_cmd_type = DSAT_RMSM_CGACT_CMD;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_MSG1(MSG_LEGACY_HIGH,"HANDLE CGACT for Pkt data call profile number (%d)", 
            profile_number);

  /*-------------------------------------------------------------------------
    Get the profile information & see what type of context is defined
  -------------------------------------------------------------------------*/
  result = ds_profile_setting_from_profile_id((int8)profile_number,&profile_info);

  if( result != DS_PROFILE_RESULT_SUCCESS ) 
  {
    err_val = DS_LOCAL_ERR_VAL_0;
    goto func_exit;
  }

  pdp_type = profile_info.context.pdp_type;
  
  /* Getting instance, if the same profile is already activated 
     return that instance */
  if( FALSE == ds_3gpp_rmsm_ati_allocate_instance( profile_number,
                                                   &instance))
  {
    err_val = DS_LOCAL_ERR_VAL_1;
    goto func_exit;
  }
                                                   
  at_pdp_state_ptr = ds_3gpp_rmsm_get_info_ptr_from_instance(instance);
  if(NULL == at_pdp_state_ptr)
  {
    err_val = DS_LOCAL_ERR_VAL_2;
    goto func_exit;
  }
  
  /* check if it is already activated , if so return from here */
  if(!ds_3gpp_rmsm_ati_pdp_state_type_validate(at_pdp_state_ptr))
  {
    err_val = DS_LOCAL_ERR_VAL_3;
    goto func_exit;
  } 
  if(at_pdp_state_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->profile_mask & (1<< (profile_number-1)))
  {
    DS_MSG0(MSG_LEGACY_HIGH,"Already activated return OK immdediately");
    return DSAT_OK;
  }

  /*-------------------------------------------------------------------------
    Set the profile number,user info and validity flag
  -------------------------------------------------------------------------*/
  at_pdp_state_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->profile_mask |= (1 << (profile_number-1));
  at_pdp_state_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->user_info_ptr = user_info_ptr;
  if((user_info_ptr) && (((ds_mo_ex_user_info_s*)user_info_ptr)->is_mox_data_pending == TRUE))
  {
    at_pdp_state_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->mo_ex_data_info = 
                                             (ds_mo_ex_user_info_s* )user_info_ptr;
    at_cmd_type = DSAT_RMSM_CSODCP_CMD;
    }
  at_pdp_state_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->connect_state = DS_3GPP_ATIF_PDP_ACTIVATING;

  /* Activate V4 profile, if context is v4v6 or v4 */
  if ( (pdp_type == DS_PROFILE_PDP_IPV4 ) ||
       (pdp_type == DS_PROFILE_PDP_IPV4V6 ) )
    
  {
    v4_act_result = ds_3gpp_rmsm_ati_activate_v4_inst( at_pdp_state_ptr,(int8)profile_number);
    if((pdp_type == DS_PROFILE_PDP_IPV4) && (v4_act_result == DSAT_OK))
    {
      send_resp = TRUE;
    }
  }

  /* Activate V6 profile, if context is v4v6 or v6 */
  if ( (pdp_type == DS_PROFILE_PDP_IPV6 ) ||
       (pdp_type == DS_PROFILE_PDP_IPV4V6 ) )
    
  {
    v6_act_result = ds_3gpp_rmsm_ati_activate_v6_inst( at_pdp_state_ptr,(int8)profile_number);
    if((pdp_type == DS_PROFILE_PDP_IPV6) && (v6_act_result == DSAT_OK))
    {
      send_resp = TRUE;
    }
  }

  /*Activate NON IP profile*/       
  if ( pdp_type == DS_PROFILE_PDP_NON_IP )
  {
    nonip_act_result = ds_3gpp_rmsm_ati_activate_nonip_inst(at_pdp_state_ptr,(int8)profile_number);
    if(nonip_act_result == DSAT_OK)
    {
      send_resp = TRUE;
    }
  }
  
  if ( ( DSAT_ERROR == v4_act_result ) && (DSAT_ERROR == v6_act_result) && (DSAT_ERROR == nonip_act_result))
  {
    at_pdp_state_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->connect_state = DS_3GPP_ATIF_PDP_DEACTIVATED;
    at_pdp_state_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->profile_mask &= ~(1 << (profile_number-1));
    err_val = DS_LOCAL_ERR_VAL_4;
    goto func_exit;
  }
  else if((pdp_type == DS_PROFILE_PDP_IPV4V6) && (v4_act_result == DSAT_OK) && (v6_act_result == DSAT_OK))
  {
    send_resp = TRUE;
  }

  if(send_resp == TRUE)
  {
  /* Send success of activation  if any of the instance is in UP state */
    at_pdp_state_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->connect_state = DS_3GPP_ATIF_PDP_ACTIVATED;
    if(at_cmd_type == DSAT_RMSM_CSODCP_CMD)
    {
      /* Reset the cache as it's no longer needed */
      at_pdp_state_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->mo_ex_data_info = NULL;
    }
      return DSAT_OK;
  }
  
  return DSAT_ASYNC_CMD;
  
func_exit:
  DS_MSG1(MSG_LEGACY_ERROR," ds_3gpp_rmsm_ati_cgact_activation err_val: %d",
                             err_val);
  return DSAT_ERROR;
} /* ds_3gpp_rmsm_ati_cgact_activation() */

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_ATI_CGACT_DEACTIVATION

  DESCRIPTION
    This function de-activates the Profile activated by this module,
    if not already de-activated .
    If the profile is already de-activated return SUCCESS immediately.

  RETURN VALUE
    DSAT_OK -- Profile is already deactivated
    DSAT_ERROR -- In case of any FAILURE
    DSAT_ASYNC_CMD -- Profile is being deactivated.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
dsat_result_enum_type  ds_3gpp_rmsm_ati_cgact_deactivation
(
  uint32                profile_number,
  void *                user_info_ptr
)
{
  ds_3gpp_rmsm_at_instance_type  instance =0;
  ds_3gpp_ati_pdp_state_type     *info_ptr = NULL;
  dsat_result_enum_type          result = DSAT_OK,result1 = DSAT_OK,
                                 result2 = DSAT_OK;
  ds_pdn_mgr_s                   *pdn_mgr_p = NULL;
  ds_local_err_val_e_type        err_val = DS_LOCAL_ERR_VAL_NONE;
  boolean                        ota_teardown = FALSE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_MSG1(MSG_LEGACY_HIGH,"HANDLE +CGACT deact for Pkt data call profilenumber (%d)", 
            profile_number);
  
  instance = ds_3gpp_rmsm_ati_get_inst_num_from_profile(profile_number);

  if ( DS_3GPP_RMSM_AT_UM_MAX_INSTANCES == instance)
  {
    /* profile was already de-activated or not a +CGACT activated context*/
    pdn_mgr_p = ds_pdn_mgr_get_pdn_from_profile_id(profile_number);

    /* Query MH to check whether the profile is active */
    if( ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p) && 
        (DS_PDN_MGR_STATE_DOWN != pdn_mgr_p->state))
    {
      /* Profile is activated by other application */
      if(pdn_mgr_p->nw_mode == SYS_SYS_MODE_LTE)
      {
        if(pdn_mgr_p->is_default)
        {
          DS_MSG2(MSG_LEGACY_HIGH,"Default PDN case: get_allow_default teardown %d any_other_PDN UP %d",
                                ds_pdn_nv_manager_get_allow_default_pdn_teardown(),
                                ds_pdn_mgr_is_any_other_pdn_up(pdn_mgr_p));
          
          if( ds_pdn_nv_manager_get_allow_default_pdn_teardown() &&
              ds_pdn_mgr_is_any_other_pdn_up(pdn_mgr_p) )
          {
            DS_MSG0(MSG_LEGACY_HIGH,"Set OTA tear down default PDN");
            ota_teardown = TRUE;
          }
        }
        else /* Non default PDN on LTE */
        {
          DS_MSG2(MSG_LEGACY_HIGH,"Non default PDN case: always connect NV %d any_other_PDN UP %d",
                        ds_pdn_nv_manager_get_always_connect_def_pdn_flag(),
                        ds_pdn_mgr_is_any_other_pdn_up(pdn_mgr_p));
          
          if( ds_pdn_nv_manager_get_always_connect_def_pdn_flag() ||
              ds_pdn_mgr_is_any_other_pdn_up(pdn_mgr_p) )
          {
            DS_MSG0(MSG_LEGACY_HIGH,"Set OTA tear down Non default PDN");
            ota_teardown = TRUE;
          }
        }
      }
      else   /*Mode other than LTE */
      {
        DS_MSG0(MSG_LEGACY_HIGH,"Set OTA tear down Non-LTE case");
        ota_teardown = TRUE;
      }
      
      DS_MSG1(MSG_LEGACY_HIGH,"OTA teardown in AT deactivation %d",ota_teardown);
      if(ota_teardown)
      {
        DS_MSG0(MSG_LEGACY_HIGH,"Tearing DOWN PDN not activated by AT");
      ds_pdn_mgr_teardown_pdn_per_profile(profile_number, 
                                       DS_NET_DOWN_REASON_INTERNAL_CALL_ENDED);
      }
    }
    else
    {
      /* Profile is already de-activated. So return DSAT_OK */
      DS_MSG0(MSG_LEGACY_HIGH,"Profile is already de-activated, return OK");
    }

    return DSAT_OK;
  }

  info_ptr = ds_3gpp_rmsm_get_info_ptr_from_instance(instance);
  if(NULL == info_ptr)
  {
    /* Instance is not in use, so return ERROR */
    err_val = DS_LOCAL_ERR_VAL_1;
    goto func_exit;
  }

  if(!ds_3gpp_rmsm_ati_pdp_state_type_validate(info_ptr))
  {
    err_val = DS_LOCAL_ERR_VAL_2;
    goto func_exit;
  } 

  /*Check if multiple profiles bound to same AT instance*/
  if((info_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->profile_mask & ~(1 << (profile_number-1))) != 0)
  {
    info_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->profile_mask &= ~(1 << (profile_number-1)); 
    DS_MSG1(MSG_LEGACY_HIGH,"Other profiles still active, return OK. Profile_mask %d",
                   info_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->profile_mask);
    return DSAT_OK;
  }
    
  if (info_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->um_inst_ptr[DS_RMSM_ATI_V4_INST] != NULL ) 
  {
    result = ds_3gpp_rmsm_ati_tear_down_v4_inst(info_ptr,profile_number);
  }
  if (info_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->um_inst_ptr[DS_RMSM_ATI_V6_INST] != NULL ) 
  {
    result1 = ds_3gpp_rmsm_ati_tear_down_v6_inst(info_ptr,profile_number);
  }
  if (info_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->um_inst_ptr[DS_RMSM_ATI_NONIP_INST] != NULL)
  {
    result2 = ds_3gpp_rmsm_ati_tear_down_nonip_inst(info_ptr,profile_number);
  }
  if( (result == DSAT_ASYNC_CMD) || (result1 == DSAT_ASYNC_CMD) || (result2 == DSAT_ASYNC_CMD))
  {
    info_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->connect_state = DS_3GPP_ATIF_PDP_DEACTIVATING;
    return DSAT_ASYNC_CMD;
  }
  else  
  {
    /* de allocate the instance only if all the um instances are cleaned up */
    if( ( info_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->um_inst_ptr[DS_RMSM_ATI_V4_INST] == NULL ) &&
    ( info_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->um_inst_ptr[DS_RMSM_ATI_V6_INST] == NULL ) &&
    ( info_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->um_inst_ptr[DS_RMSM_ATI_NONIP_INST] == NULL)) 
    {
      ds_3gpp_rmsm_ati_deallocate_instance (instance);
    }
    DS_MSG0(MSG_LEGACY_HIGH,"Profile is de-activated, return OK");
    return DSAT_OK;
  }

func_exit:
  if(err_val != DS_LOCAL_ERR_VAL_NONE)
  {
    DS_MSG1(MSG_LEGACY_ERROR,"ds_3gpp_rmsm_ati_cgact_deactivation err_val : %d",
                            err_val);
  }
  return DSAT_ERROR;

}/* ds_3gpp_rmsm_ati_cgact_deactivation */

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_ATI_PDP_STATE_TYPE_VALIDATE

  DESCRIPTION
  
  RETURN VALUE
    FALSE  : if failure
    TRUE : if Success
  
  DEPENDENCIES
    None
    
  SIDE EFFECTS
    None
===========================================================================*/
boolean ds_3gpp_rmsm_ati_pdp_state_type_validate
(
  ds_3gpp_ati_pdp_state_type     *pdp_ptr
)
{
  boolean ret_val = FALSE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(pdp_ptr == NULL)
  {
    DS_MSG0(MSG_LEGACY_ERROR,"Input parameter passed is INVALID");
    return ret_val;
  }
  if ((pdp_ptr->ds_3gpp_ati_pdp_state_type_dyn_p != NULL) && pdp_ptr->in_use == TRUE)
  {
    ret_val = TRUE;
  }
  return ret_val;
}

/*===========================================================================
FUNCTION   DS_3GPP_RMSM_ATI_ALLOCATE_INSTANCE

DESCRIPTION
  Returns the pointer to RMSM AT instance if present 
  
NOTE: THIS IS A RESTRICTED FUNCTION AND IS INTENDED TO BE USED BY 
DS_3GPP_AT MODULES ONLY


RETURN VALUE
  Pointer to the instance number:
              valid DS_3GPP_RMSM instance  if Success
              NULL if failure to allocate an instance

DEPENDENCIES
  ds_3gpp_ati_init_all_instances() should have been called first.

SIDE EFFECTS
  None
===========================================================================*/
boolean  ds_3gpp_rmsm_ati_allocate_instance
(
  uint32 profile_number,
  ds_3gpp_rmsm_at_instance_type*  instance
)
{
  ds_3gpp_rmsm_at_instance_type index = 0;
  ds_fwk_clnt_handle_type       v4_client_hdl    = DS_FWK_INVALID_CLNT_HANDLE;
  ds_fwk_clnt_handle_type       v6_client_hdl    = DS_FWK_INVALID_CLNT_HANDLE;
  ds_fwk_clnt_handle_type       nonip_client_hdl = DS_FWK_INVALID_CLNT_HANDLE;
  ds_profile_type_s             profile_info = {{0}};
  ds_policy_info_type           pol_info = {0};
  ds_profile_pdp_type_e         pdp_type = DS_PROFILE_PDP_MAX;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  if(NULL == instance)
  {
    DS_MSG0(MSG_LEGACY_ERROR,"Instance passed is Invalid");
    return FALSE;
  }
  
  /* Check if an instance is already allocated for this profile number */
  *instance = ds_3gpp_rmsm_ati_get_inst_num_from_profile(profile_number);
  if(DS_3GPP_RMSM_AT_UM_MAX_INSTANCES != *instance)
  {
    DS_MSG1(MSG_LEGACY_HIGH,"Already activated instance = %d",*instance);
    return TRUE;
  }
  
  ds_profile_setting_from_profile_id((int8)profile_number,&profile_info);
  pol_info.length = strlen(profile_info.context.apn);
  memscpy(pol_info.name, pol_info.length ,
  	      profile_info.context.apn, pol_info.length);
  pdp_type = profile_info.context.pdp_type;
  
  /*Check if call is already up with this policy info*/
  if((pdp_type == DS_PROFILE_PDP_IPV4) || (pdp_type == DS_PROFILE_PDP_IPV4V6))
  {
    pol_info.family = IPV4_ADDR;
    v4_client_hdl = ds_fwk_look_up_pdn_by_policy(DS_FWK_CLIENT_ATCOP,pol_info);
  }
  if((pdp_type == DS_PROFILE_PDP_IPV6) || (pdp_type == DS_PROFILE_PDP_IPV4V6))
  {
    pol_info.family = IPV6_ADDR;
    v6_client_hdl = ds_fwk_look_up_pdn_by_policy(DS_FWK_CLIENT_ATCOP,pol_info);
  }
  if(pdp_type == DS_PROFILE_PDP_NON_IP)
  {
    pol_info.family = NON_IP_ADDR;
    nonip_client_hdl = ds_fwk_look_up_pdn_by_policy(DS_FWK_CLIENT_ATCOP,pol_info);
  }
  
  for (index = 0; (index <DS_3GPP_RMSM_AT_UM_MAX_INSTANCES); index ++)
  {
    if((ds_3gpp_ati_pdp_state[index].in_use == TRUE) && 
      ((v4_client_hdl != DS_FWK_INVALID_CLNT_HANDLE) ||
       (v6_client_hdl != DS_FWK_INVALID_CLNT_HANDLE) || 
       (nonip_client_hdl != DS_FWK_INVALID_CLNT_HANDLE)))
    {
      if(ds_3gpp_ati_pdp_state[index].ds_3gpp_ati_pdp_state_type_dyn_p && 
          ((ds_3gpp_ati_pdp_state[index].ds_3gpp_ati_pdp_state_type_dyn_p->v4_clt_hdl == v4_client_hdl) ||
          (ds_3gpp_ati_pdp_state[index].ds_3gpp_ati_pdp_state_type_dyn_p->v6_clt_hdl == v6_client_hdl)  ||
          (ds_3gpp_ati_pdp_state[index].ds_3gpp_ati_pdp_state_type_dyn_p->nonip_clt_hdl == nonip_client_hdl)))
      {
        *instance = index;
        ds_3gpp_ati_pdp_state[index].ds_3gpp_ati_pdp_state_type_dyn_p->v4_clt_hdl = (index * 3) + DS_RMSM_ATI_V4_INST;
        ds_3gpp_ati_pdp_state[index].ds_3gpp_ati_pdp_state_type_dyn_p->v6_clt_hdl = (index * 3) + DS_RMSM_ATI_V6_INST;
        ds_3gpp_ati_pdp_state[index].ds_3gpp_ati_pdp_state_type_dyn_p->nonip_clt_hdl = (index * 3) + DS_RMSM_ATI_NONIP_INST;
        return TRUE;
      }
    }
    else if(ds_3gpp_ati_pdp_state[index].in_use == FALSE)
    {
      *instance = index;
      DS_MSG1(MSG_LEGACY_HIGH,"Allocated Instance = %d",index);
      ds_3gpp_ati_pdp_state[index].in_use = TRUE;
      DS_SYSTEM_HEAP_MEM_ALLOC(ds_3gpp_ati_pdp_state[index].ds_3gpp_ati_pdp_state_type_dyn_p,
        sizeof(ds_3gpp_ati_pdp_state_type_dyn_s),ds_3gpp_ati_pdp_state_type_dyn_s *);

      memset((void *)ds_3gpp_ati_pdp_state[index].ds_3gpp_ati_pdp_state_type_dyn_p,0x0,
                                              sizeof(ds_3gpp_ati_pdp_state_type_dyn_s));
      ds_3gpp_ati_pdp_state[index].ds_3gpp_ati_pdp_state_type_dyn_p->v4_clt_hdl = (index * 3) + DS_RMSM_ATI_V4_INST;
      ds_3gpp_ati_pdp_state[index].ds_3gpp_ati_pdp_state_type_dyn_p->v6_clt_hdl = (index * 3) + DS_RMSM_ATI_V6_INST;
      ds_3gpp_ati_pdp_state[index].ds_3gpp_ati_pdp_state_type_dyn_p->nonip_clt_hdl = (index * 3) + DS_RMSM_ATI_NONIP_INST;
      return TRUE;
    }
  }
  DS_MSG0(MSG_LEGACY_ERROR,"No available instance");
  *instance = DS_3GPP_RMSM_AT_UM_MAX_INSTANCES;
  return FALSE;
} /* ds_3gpp_rmsm_ati_allocate_instance */

/*===========================================================================
FUNCTION   DS_3GPP_RMSM_ATI_DEALLOCATE_INSTANCE

DESCRIPTION
  This function de-allocates RMSM AT instance if present 
  
NOTE: THIS IS A RESTRICTED FUNCTION AND IS INTENDED TO BE USED BY 
DS_3GPP_AT MODULES ONLY

RETURN VALUE
  None

DEPENDENCIES
  ds_3gpp_ati_init_all_instances() should have been called first.

SIDE EFFECTS
  None
===========================================================================*/
void ds_3gpp_rmsm_ati_deallocate_instance
(
  ds_3gpp_rmsm_at_instance_type  instance
)
{
  DS_MSG1(MSG_LEGACY_HIGH,"In ds_3gpp_rmsm_ati_deallocate_instance instance id %d",instance);
  if ( DS_3GPP_RMSM_AT_UM_MAX_INSTANCES <= instance)
  {
    DS_MSG1(MSG_LEGACY_ERROR,"Invalid instance : %d",instance);
    return;
  }
    
  if(ds_3gpp_ati_pdp_state[instance].in_use == TRUE)
  {
    DS_RMSM_ENTER_CRIT_SECTION(&ds_3gpp_rmsm_crit_sect);
    ds_3gpp_ati_pdp_state[instance].in_use = FALSE;
    DS_SYSTEM_HEAP_MEM_FREE(ds_3gpp_ati_pdp_state[instance].ds_3gpp_ati_pdp_state_type_dyn_p);
    ds_3gpp_ati_pdp_state[instance].ds_3gpp_ati_pdp_state_type_dyn_p = NULL;
    DS_RMSM_LEAVE_CRIT_SECTION(&ds_3gpp_rmsm_crit_sect);
  }

}/* ds_3gpp_rmsm_ati_deallocate_instance */

/*===========================================================================
FUNCTION   DS_3GPP_RMSM_ATI_INIT_ALL_INSTANCES

DESCRIPTION
  This function initializes all RMSM AT instances 
  
NOTE: THIS IS A RESTRICTED FUNCTION AND IS INTENDED TO BE USED BY 
DS_3GPP_AT MODULES ONLY


RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void  ds_3gpp_rmsm_ati_init_all_instances( void )
{
  int index = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Initialized the state information for each instance of RMSM AT
  -------------------------------------------------------------------------*/
  for (index = 0; index < DS_3GPP_RMSM_AT_UM_MAX_INSTANCES; index++)
  {
    memset(&ds_3gpp_ati_pdp_state[index],0x0,sizeof(ds_3gpp_ati_pdp_state_type));
    ds_3gpp_ati_pdp_state[index].in_use = FALSE;
  }
  
  DS_RMSM_INIT_CRIT_SECTION(&ds_3gpp_rmsm_crit_sect);
    
  /*Register with framework for PDN events*/
  ds_fwk_reg_events(DS_FWK_CLIENT_ATCOP,ds_rmsm_ati_um_event_handler_cb,NULL);
  
}/* ds_3gpp_rmsm_ati_init_all_instances */

/*===========================================================================
FUNCTION   DS_3GPP_RMSM_ATI_GET_INST_NUM_FROM_UM_INST

DESCRIPTION
  This function returns the RMSM AT instance from Um instance pointer if present 

RETURN VALUE
  The instance number:
     valid DS_3GPP_RMSM instance  if Success
     DS_3GPP_RMSM_AT_UM_MAX_INSTANCES if failure to allocate an instance

DEPENDENCIES
  ds_3gpp_ati_init_all_instances() should have been called first.

SIDE EFFECTS
  None
===========================================================================*/

ds_3gpp_rmsm_at_instance_type ds_3gpp_rmsm_ati_get_inst_num_from_um_inst
(
  ds_at_pdn_instance* um_inst_ptr
)
{
  int idx;
  ds_3gpp_rmsm_at_instance_type instance = DS_3GPP_RMSM_AT_UM_MAX_INSTANCES;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( NULL == um_inst_ptr)
  {
    DS_MSG0(MSG_LEGACY_ERROR,"Invalid input AT PDN instance");
    return instance;
  }

  for (idx=0; idx < DS_3GPP_RMSM_AT_UM_MAX_INSTANCES; idx++)
  {
    /* Return Instance if it matches either v4, v6 or nonip instance */
    if ((ds_3gpp_ati_pdp_state[idx].in_use == TRUE)  &&
        ((ds_3gpp_ati_pdp_state[idx].ds_3gpp_ati_pdp_state_type_dyn_p->um_inst_ptr[DS_RMSM_ATI_V4_INST] == um_inst_ptr) ||
        (ds_3gpp_ati_pdp_state[idx].ds_3gpp_ati_pdp_state_type_dyn_p->um_inst_ptr[DS_RMSM_ATI_V6_INST] == um_inst_ptr)  ||
        (ds_3gpp_ati_pdp_state[idx].ds_3gpp_ati_pdp_state_type_dyn_p->um_inst_ptr[DS_RMSM_ATI_NONIP_INST] == um_inst_ptr)))
    {
      instance = idx;
      break;
    }
  }
  return instance;
}/* ds_3gpp_rmsm_ati_get_inst_num_from_um_inst */

/*===========================================================================
FUNCTION   DS_3GPP_RMSM_ATI_GET_INST_NUM_FROM_CLIENT_HANDLE

DESCRIPTION
  This function returns the RMSM AT instance from Um instance pointer if present 

RETURN VALUE
  The instance number:
     valid DS_3GPP_RMSM instance  if Success
     DS_3GPP_RMSM_AT_UM_MAX_INSTANCES if failure to allocate an instance

DEPENDENCIES
  ds_3gpp_ati_init_all_instances() should have been called first.

SIDE EFFECTS
  None
===========================================================================*/

ds_3gpp_rmsm_at_instance_type ds_3gpp_rmsm_ati_get_inst_num_from_client_handle
(
  ds_fwk_clnt_handle_type clt_hdl
)
{
  return (ds_3gpp_rmsm_at_instance_type)(clt_hdl / 3);
}/* ds_3gpp_rmsm_ati_get_inst_num_from_client_handle */

/*===========================================================================
FUNCTION   DS_3GPP_RMSM_ATI_GET_INST_NUM_FROM_PROFILE

DESCRIPTION
  This function returns the RMSM AT instance from profile number if present 
  
NOTE: THIS IS A RESTRICTED FUNCTION AND IS INTENDED TO BE USED BY 
DS_AT MODULES ONLY

RETURN VALUE
  The instance number:
     valid DS_3GPP_RMSM instance  if Success
     DS_3GPP_RMSM_AT_UM_MAX_INSTANCES if failure to allocate an instance

DEPENDENCIES
  ds_3gpp_ati_init_all_instances() should have been called first.

SIDE EFFECTS
  None
====================================================================================*/
ds_3gpp_rmsm_at_instance_type ds_3gpp_rmsm_ati_get_inst_num_from_profile
(
  uint32 profile_number
)
{
  int idx;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_3gpp_rmsm_at_instance_type instance = DS_3GPP_RMSM_AT_UM_MAX_INSTANCES;

  for (idx=0; idx<DS_3GPP_RMSM_AT_UM_MAX_INSTANCES; idx++)
  {
    
    DS_MSG2(MSG_LEGACY_HIGH,"Fetching instance num from Profile Index, Index: %d  mask: %x",idx,
        (ds_3gpp_ati_pdp_state[idx].ds_3gpp_ati_pdp_state_type_dyn_p)?
        (ds_3gpp_ati_pdp_state[idx].ds_3gpp_ati_pdp_state_type_dyn_p->profile_mask):0x0);
    
    if ( (ds_3gpp_ati_pdp_state[idx].in_use == TRUE)  &&
         ds_3gpp_ati_pdp_state[idx].ds_3gpp_ati_pdp_state_type_dyn_p->profile_mask & (1 << (profile_number-1)))
    {
      instance = idx;
      break;
    }
  }
  return instance;
}/* ds_3gpp_rmsm_ati_get_inst_num_from_um_inst */

/*===========================================================================
FUNCTION   DS_3GPP_RMSM_ATI_GET_UM_INST_STATE

DESCRIPTION
  This function returns the RMSM AT instance from v6 Um instance pointer if present 
  
NOTE: THIS IS A RESTRICTED FUNCTION AND IS INTENDED TO BE USED BY 
DS_AT MODULES ONLY

RETURN VALUE
  PS instance STATE

DEPENDENCIES
  ds_3gpp_ati_init_all_instances() should have been called first.

SIDE EFFECTS
  None
===========================================================================*/
ds_pdn_mgr_instance_state_e ds_3gpp_rmsm_ati_get_um_inst_state
(
  ds_3gpp_rmsm_at_instance_type instance,
  ds_rmsm_ati_inst_type_e       inst_type
)
{
  if(!ds_3gpp_rmsm_ati_pdp_state_type_validate(&ds_3gpp_ati_pdp_state[instance]))
  {
    return DS_INSTANCE_STATE_DOWN;
  }

  if(ds_3gpp_ati_pdp_state[instance].ds_3gpp_ati_pdp_state_type_dyn_p->um_inst_ptr[inst_type])
    return (ds_3gpp_ati_pdp_state[instance].ds_3gpp_ati_pdp_state_type_dyn_p->um_inst_ptr[inst_type]->state);
  else 
    return DS_INSTANCE_STATE_DOWN;
  
}/* ds_3gpp_rmsm_ati_get_um_inst_state */

/*===========================================================================
FUNCTION   DS_3GPP_RMSM_ATI_GET_V4_UM_INST_STATE

DESCRIPTION
  This function returns the RMSM AT instance state for ipv4 instance
  
NOTE: THIS IS A RESTRICTED FUNCTION AND IS INTENDED TO BE USED BY 
DS_AT MODULES ONLY

RETURN VALUE
  PS instance STATE

DEPENDENCIES
  ds_3gpp_ati_init_all_instances() should have been called first.

SIDE EFFECTS
  None
===========================================================================*/
ds_pdn_mgr_instance_state_e ds_3gpp_rmsm_ati_get_v4_um_inst_state
(
  ds_3gpp_rmsm_at_instance_type instance
)
{
  return ds_3gpp_rmsm_ati_get_um_inst_state(instance,DS_RMSM_ATI_V4_INST);
}/* ds_3gpp_rmsm_ati_get_v4_um_inst_state */

/*===========================================================================
FUNCTION   DS_3GPP_RMSM_ATI_GET_V6_UM_INST_STATE

DESCRIPTION
  This function returns the RMSM AT instance state for ipV6 instance
  
NOTE: THIS IS A RESTRICTED FUNCTION AND IS INTENDED TO BE USED BY 
DS_AT MODULES ONLY

RETURN VALUE
  PS instance STATE

DEPENDENCIES
  ds_3gpp_ati_init_all_instances() should have been called first.

SIDE EFFECTS
  None
===========================================================================*/
ds_pdn_mgr_instance_state_e ds_3gpp_rmsm_ati_get_v6_um_inst_state
(
  ds_3gpp_rmsm_at_instance_type instance
)
{
  return ds_3gpp_rmsm_ati_get_um_inst_state(instance,DS_RMSM_ATI_V6_INST);
}/* ds_3gpp_rmsm_ati_get_v6_um_inst_state */

/*===========================================================================
FUNCTION   DS_3GPP_RMSM_ATI_GET_NONIP_UM_INST_STATE

DESCRIPTION
  This function returns the RMSM AT instance state for non ip instance 
  
NOTE: 

RETURN VALUE
  PDN MGR instance STATE

DEPENDENCIES
  ds_3gpp_ati_init_all_instances() should have been called first.

SIDE EFFECTS
  None
===========================================================================*/
ds_pdn_mgr_instance_state_e ds_3gpp_rmsm_ati_get_nonip_um_inst_state
(
  ds_3gpp_rmsm_at_instance_type instance
)
{
  return ds_3gpp_rmsm_ati_get_um_inst_state(instance,DS_RMSM_ATI_NONIP_INST);
}/* ds_3gpp_rmsm_ati_get_nonip_um_inst_state */

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_GET_INST_PTR
  
  DESCRIPTION   
    Given the call instance, returns the ps instance_ptr.
  
  DEPENDENCIES  
    None.
  
  RETURN VALUE  
    See Description.
    
  SIDE EFFECTS  
    None.
  ===========================================================================*/
ds_at_pdn_instance* ds_3gpp_rmsm_get_inst_ptr
(
  ds_3gpp_rmsm_at_instance_type instance,
  ds_rmsm_ati_inst_type_e       inst_type
)
{
  ds_local_err_val_e_type err_val = DS_LOCAL_ERR_VAL_NONE;
  
  if( instance >= DS_3GPP_RMSM_AT_UM_MAX_INSTANCES  )
  {
    err_val = DS_LOCAL_ERR_VAL_0;
    goto func_exit;
  }
  
  if(!ds_3gpp_rmsm_ati_pdp_state_type_validate(&ds_3gpp_ati_pdp_state[instance]))
  {
    err_val = DS_LOCAL_ERR_VAL_1;
    goto func_exit;
  }
  
  return (ds_3gpp_ati_pdp_state[instance].ds_3gpp_ati_pdp_state_type_dyn_p->um_inst_ptr[inst_type]);
      
func_exit:
   DS_MSG1(MSG_LEGACY_ERROR,"ds_3gpp_rmsm_get_v4_inst_ptr err_val : %d",err_val);
   return NULL;

} /* ds_3gpp_rmsm_get_v4_inst_ptr */

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_GET_V6_instance_PTR
  
  DESCRIPTION   
    Given the call instance, returns the v6 ps instance_ptr.
  
  DEPENDENCIES  
    None.
  
  RETURN VALUE  
    
  SIDE EFFECTS  
    None.
  ===========================================================================*/
ds_at_pdn_instance* ds_3gpp_rmsm_get_v4_inst_ptr
(
  ds_3gpp_rmsm_at_instance_type instance
)
{
  return ds_3gpp_rmsm_get_inst_ptr(instance,DS_RMSM_ATI_V4_INST);
} /* ds_3gpp_rmsm_get_v4_inst_ptr */

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_GET_V6_instance_PTR
  
  DESCRIPTION   
    Given the call instance, returns the v6 ps instance_ptr.
  
  DEPENDENCIES  
    None.
  
  RETURN VALUE  
    
  SIDE EFFECTS  
    None.
  ===========================================================================*/
ds_at_pdn_instance* ds_3gpp_rmsm_get_v6_inst_ptr
(
  ds_3gpp_rmsm_at_instance_type instance
)
{
  return  ds_3gpp_rmsm_get_inst_ptr(instance,DS_RMSM_ATI_V6_INST);
} /* ds_3gpp_rmsm_get_v6_inst_ptr */

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_GET_NONIP_INSTANCE_PTR
  
  DESCRIPTION   
    Given the call instance, returns the v6 ps instance_ptr.
  
  DEPENDENCIES  
    None.
  
  RETURN VALUE  
    
  SIDE EFFECTS  
    None.
  ===========================================================================*/
ds_at_pdn_instance* ds_3gpp_rmsm_get_nonip_inst_ptr
(
  ds_3gpp_rmsm_at_instance_type instance
)
{
  return  ds_3gpp_rmsm_get_inst_ptr(instance,DS_RMSM_ATI_NONIP_INST);
} /* ds_3gpp_rmsm_get_nonip_inst_ptr */

/*===========================================================================
FUNCTION   DS_3GPP_RMSM_ATI_UM_EVENT_HANDLER_CB

DESCRIPTION
  This callback function is called when one of the registered ps_instance events
  occur on the UM_PKT_instance.  Registraion is done for various events based
  upon the call type during call setup. As part of event handling
  appropriate event is posted to DS_3GPP_RMSM_AT.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
/*ARGSUSED*/
void ds_rmsm_ati_um_event_handler_cb
(
  ds_fwk_clnt_handle_type    client_handle,
  ds_pdn_event_enum_type     event,
  ds_pdn_event_info_u_type   event_info,
  void                      *user_data_ptr

)
{
  ds_3gpp_rmsm_at_instance_type   instance = DS_3GPP_RMSM_AT_UM_MAX_INSTANCES;
  ds_command_type                 *cmd_ptr = NULL; /* DS Task cmd buf        */
  boolean                         post_event = FALSE;
  ds_3gpp_rmsm_at_type            *data_ptr = NULL;
  ds_local_err_val_e_type         err_val = DS_LOCAL_ERR_VAL_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_MSG2(MSG_LEGACY_HIGH, "Recd event %d for client hdl %d", event, client_handle);

  instance = ds_3gpp_rmsm_ati_get_inst_num_from_client_handle(client_handle);
  if ( DS_3GPP_RMSM_AT_UM_MAX_INSTANCES <= instance)
  {
    err_val = DS_LOCAL_ERR_VAL_0;
    goto func_exit;
  }

  /*-------------------------------------------------------------------------
    Process flow control events first since they may get posted during data
    transfer
  -------------------------------------------------------------------------*/
  switch( event )
  {
    case PDN_UP_EV:
      DS_MSG0(MSG_LEGACY_HIGH, "PDN Up event in ATI");
      post_event= TRUE;
      break;

    case PDN_DOWN_EV:
      DS_MSG0(MSG_LEGACY_HIGH, "PDN Down event in ATI");
      post_event = TRUE;
      break;

    case BEARER_FLOW_ENABLED_EV:
      /*---------------------------------------------------------------------
        Set signal to PS if data is pending to resume data transfer.
      ---------------------------------------------------------------------*/
      DS_MSG0(MSG_LEGACY_HIGH, "Bearer flow enabled");
      DS_3GPP_RMSM_SET_FLOW_STATE(instance,TRUE);
      if (DS_3GPP_RMSM_GET_PENDING_DATA(instance) > 0)
      {
        /*---------------------------------------------------------------------
         Send signal to PS to send data to UM instance
        ---------------------------------------------------------------------*/
        DS_MSG0(MSG_LEGACY_HIGH,"setting signal ");
        ps_set_signal( PS_RMSM_AT_UM_RX_Q_SIGNAL );
      }
      break;

    case BEARER_FLOW_DISABLED_EV:
      /*---------------------------------------------------------------------
        We don't need to do anything when UM flow is disabled, transfer 
        function will stop sending data based on this flag.
      ---------------------------------------------------------------------*/
      DS_MSG0(MSG_LEGACY_HIGH, "Bearer flow DISABLED");
      if ( DS_3GPP_RMSM_GET_PENDING_DATA(instance) > 0)
      {
        /* clear the signal if already Set */
        ps_clr_signal(PS_RMSM_AT_UM_RX_Q_SIGNAL);
      }
      DS_3GPP_RMSM_SET_FLOW_STATE(instance,FALSE);
      break;
    
    default:
      DS_MSG1(MSG_LEGACY_ERROR, "Unregistered event %d recvd, ignoring", event);
      break;
  } /* switch( event ) */ 
  if(post_event == FALSE)
  {
    err_val = DS_LOCAL_ERR_VAL_1;
    goto func_exit;
  }

  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_3gpp_rmsm_at_type));
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    err_val = DS_LOCAL_ERR_VAL_2;
    goto func_exit;
  }

  cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_RMSM_AT_PDN_EV;
  data_ptr = (ds_3gpp_rmsm_at_type*)cmd_ptr->cmd_payload_ptr;

  data_ptr->client_handle = client_handle;
  data_ptr->event = event;
  data_ptr->ds_3gpp_rmsm_at_instance = instance;
  memscpy( &(data_ptr->event_info),
           sizeof(ds_pdn_event_info_u_type),
           &event_info,
           sizeof(ds_pdn_event_info_u_type));
  ds_put_cmd( cmd_ptr);

func_exit:
  if(err_val != DS_LOCAL_ERR_VAL_NONE)
    DS_MSG1(MSG_LEGACY_ERROR, "ds_3gpp_rmsm_ati_um_event_handler_cb err_val : %d", err_val);
  return;
}/* ds_3gpp_rmsm_ati_um_event_handler_cb */

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_AT_UM_INST_EVT_HDLR()

  DESCRIPTION
    This function process instance_UP, instance_DOWN
    events

  RETURN VALUE
    ds_cmd_ptr: Cmd buffer passed with the cmd

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void ds_3gpp_rmsm_at_um_inst_evt_hdlr
(
  ds_command_type *ds_cmd_ptr
)
{
  ds_3gpp_rmsm_at_type  *rmsm_at_ptr = NULL;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if ((ds_cmd_ptr == NULL) || (ds_cmd_ptr->cmd_payload_ptr == NULL))
  {  
    DS_MSG0(MSG_LEGACY_ERROR, "Invalid command ptr");
    return;
  }

  rmsm_at_ptr = (ds_3gpp_rmsm_at_type*)ds_cmd_ptr->cmd_payload_ptr;
  DS_MSG2(MSG_LEGACY_HIGH, "Recd event %d for AT instance %d",
                      rmsm_at_ptr->event,rmsm_at_ptr->ds_3gpp_rmsm_at_instance);

  switch(rmsm_at_ptr->event)
  {
    case  PDN_DOWN_EV:
      ds_3gpp_rmsm_ati_um_inst_down_handler(rmsm_at_ptr);
      break;

    case  PDN_UP_EV:
      ds_3gpp_rmsm_ati_um_inst_up_handler(rmsm_at_ptr);
      break;

    default:
      DS_MSG1(MSG_LEGACY_ERROR,"Invalid Event type :%d", rmsm_at_ptr->event);
      break;
  }
  return;
}/* ds_3gpp_rmsm_at_um_inst_evt_hdlr */

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_AT_PROCESS_CMD()

  DESCRIPTION
    This function receives a cmd posted to DS_3GPP_RMSM AT interface 
    in DS task and passes it to the appropriate handling.

  RETURN VALUE
    ds_cmd_ptr: Cmd buffer passed with the cmd

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void ds_3gpp_rmsm_at_process_cmd
(
  ds_command_type *ds_cmd_ptr
)
{
  ds_command_enum_type cmd;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(NULL == ds_cmd_ptr)
    return;

  cmd  = ds_cmd_ptr->hdr.cmd_id;
  switch(cmd)
  {
    case DS_CMD_3GPP_RMSM_AT_PDN_EV:
      ds_3gpp_rmsm_at_um_inst_evt_hdlr(ds_cmd_ptr);
      break;

    case DS_CMD_3GPP_RMSM_AT_SEND_DATA:
      ds_3gpp_rmsm_at_data_cmd_hdlr(ds_cmd_ptr);
      break;
    
    default:
      DS_MSG0(MSG_LEGACY_ERROR,"Invalid cmd, ignoring");
      break;
  }
  return;
} /* ds_3gpp_rmsm_at_process_cmd */

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_AT_DATA_CMD_HDLR

  DESCRIPTION
    This function process data sending command handler.

  RETURN VALUE
    ds_cmd_ptr: Cmd buffer passed with the cmd

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void ds_3gpp_rmsm_at_data_cmd_hdlr
(
  ds_command_type *ds_cmd_ptr
)
{
  ds_3gpp_rmsm_at_instance_type  instance = DS_3GPP_RMSM_AT_UM_MAX_INSTANCES;
  ds_3gpp_ati_pdp_state_type    *info_ptr = NULL;
  dsat_rmsm_info_s_type          dsat_rmsm_info;
  ds_3gpp_rmsm_at_type          *rmsm_at_ptr = NULL;
  ds_local_err_val_e_type        err_val = DS_LOCAL_ERR_VAL_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if((NULL == ds_cmd_ptr) || (NULL == ds_cmd_ptr->cmd_payload_ptr))
  {
    err_val = DS_LOCAL_ERR_VAL_0;
    goto func_exit;
  }

  rmsm_at_ptr = (ds_3gpp_rmsm_at_type*)ds_cmd_ptr->cmd_payload_ptr;

  instance = rmsm_at_ptr->ds_3gpp_rmsm_at_instance;
  if((instance < 0 )|| (instance >= DS_3GPP_RMSM_AT_UM_MAX_INSTANCES))
  {
    err_val = DS_LOCAL_ERR_VAL_1;
    goto func_exit;
  }
  DS_MSG0(MSG_LEGACY_HIGH,"ds_3gpp_rmsm_at_data_cmd_hdlr");
  info_ptr = ds_3gpp_rmsm_get_info_ptr_from_instance(instance);

  if( NULL == info_ptr )
  {
    /*-------------------------------------------------------------------------  
      Disable the PS signal
    -------------------------------------------------------------------------*/
    ps_disable_sig(PS_RMSM_AT_UM_RX_Q_SIGNAL);
    /* Send final response to ATCoP */
    dsat_rmsm_info.result = DSAT_ERROR;
    dsat_rmsm_info.reason_type = DSAT_RMSM_INVALID_REASON;
    dsat_rmsm_info.user_info_ptr = NULL;
    dsat_rmsm_done_handler(DSAT_RMSM_QCDGEN_CMD,dsat_rmsm_info);
    err_val = DS_LOCAL_ERR_VAL_2;
    goto func_exit;
  }

  if(!ds_3gpp_rmsm_ati_pdp_state_type_validate(info_ptr))
  {
    err_val = DS_LOCAL_ERR_VAL_3;
    goto func_exit;
  } 
  
  /* check if any pending data for transfer and flow is enabled */
  if(info_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->pending_data !=0)
  {
    /*---------------------------------------------------------------------
     Send signal to PS only if flow is enabled
    ---------------------------------------------------------------------*/
    if(info_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->um_flow_enabled == TRUE )
    {
      DS_MSG0(MSG_LEGACY_HIGH,"setting signal ");
      PS_SET_SIGNAL( PS_RMSM_AT_UM_RX_Q_SIGNAL );
    }
    goto func_exit;
  }

  /*-------------------------------------------------------------------------  
    Disable the PS signal
  -------------------------------------------------------------------------*/
  ps_disable_sig(PS_RMSM_AT_UM_RX_Q_SIGNAL);

  DS_MSG1(MSG_LEGACY_HIGH,"Data transfer is complete, return result %d",
                (int)rmsm_at_ptr->result);
  /* Send final response to ATCoP */
  if(rmsm_at_ptr->result == TRUE)
  {
    dsat_rmsm_info.result = DSAT_OK;
  }
  else
  {
    dsat_rmsm_info.result = DSAT_ERROR;
  }
  dsat_rmsm_info.reason_type = DSAT_RMSM_INVALID_REASON;
  dsat_rmsm_info.user_info_ptr = info_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->user_info_ptr;
  dsat_rmsm_done_handler(DSAT_RMSM_QCDGEN_CMD,dsat_rmsm_info);
  /* clear user info pointer */
  info_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->user_info_ptr= NULL;

func_exit:
  if(err_val != DS_LOCAL_ERR_VAL_NONE)
    DS_MSG1(MSG_LEGACY_ERROR,"ds_3gpp_rmsm_at_data_cmd_hdlr err_val : %d",err_val);
  
  return;
}/* ds_3gpp_rmsm_at_data_cmd_hdlr */

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_ATI_UM_INST_UP_HANDLER

  DESCRIPTION
    This function process the INSTANCE_UP event posted to DS_3GPP_RMSM AT 
    interface

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void ds_3gpp_rmsm_ati_um_inst_up_handler
(
  ds_3gpp_rmsm_at_type*  rmsm_at_ptr
)
{
  ds_3gpp_ati_pdp_state_type     *info_ptr= NULL;
  dsat_rmsm_info_s_type          dsat_rmsm_info;
  dsat_cgerep_evt_info_s_type    pkt_evt_info = {0};
  boolean                        change_state = TRUE;
  ds_3gpp_rmsm_at_instance_type  instance = DS_3GPP_RMSM_AT_UM_MAX_INSTANCES;
  ds_fwk_clnt_handle_type        clt_hdl;
  ds_fwk_clnt_info_s_type        clt_info = {0};
  ds_local_err_val_e_type        err_val = DS_LOCAL_ERR_VAL_NONE;
  ds_mo_ex_user_info_s           *mox_user_info = NULL;
  dsat_rmsm_cmd_type             at_cmd_type = DSAT_RMSM_CGACT_CMD;
  uint8                          profile_num_list[DS_3GPP_MAX_PDN_CONTEXT] = {0};
  uint8                          num_prof_entries = 0;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(rmsm_at_ptr != NULL)
  {
    instance = rmsm_at_ptr->ds_3gpp_rmsm_at_instance;
    clt_hdl  = rmsm_at_ptr->client_handle;
  }
  else
  {
    err_val = DS_LOCAL_ERR_VAL_0;
    goto func_exit;
  }

  if((instance < 0 )|| (instance >= DS_3GPP_RMSM_AT_UM_MAX_INSTANCES))
  {
    err_val = DS_LOCAL_ERR_VAL_1;
    goto func_exit;
  }

  info_ptr = ds_3gpp_rmsm_get_info_ptr_from_instance(instance);
  if(NULL == info_ptr)
  {
    err_val = DS_LOCAL_ERR_VAL_2;
    goto func_exit;
  }

  if(!ds_3gpp_rmsm_ati_pdp_state_type_validate(info_ptr))
  {
    err_val = DS_LOCAL_ERR_VAL_3;
    goto func_exit;
  }
  
  ds_fwk_get_clnt_info_from_handle(DS_FWK_CLIENT_ATCOP,clt_hdl,&clt_info);

  DS_MSG1(MSG_LEGACY_HIGH,"Instance UP Indication for UM PDP instance %d",clt_hdl% DS_RMSM_ATI_MAX_INST);
  info_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->um_inst_ptr[clt_hdl% DS_RMSM_ATI_MAX_INST]->in_use = TRUE;
  info_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->um_inst_ptr[clt_hdl% DS_RMSM_ATI_MAX_INST]->state = DS_INSTANCE_STATE_UP;
  /* Store the fw pointer at corresponding instance */
  info_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->um_inst_ptr[clt_hdl% DS_RMSM_ATI_MAX_INST]->fw_inst = clt_info.fw_inst;
    DS_MSG1(MSG_LEGACY_HIGH,"Fw instance ptr updated to 0x%p ",
                  info_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->um_inst_ptr[clt_hdl% DS_RMSM_ATI_MAX_INST]->fw_inst);
  memset(&dsat_rmsm_info, 0 , sizeof(dsat_rmsm_info_s_type));

  /* wait for both instances UP or DOWN event before sending response */
  if ( ( clt_hdl% DS_RMSM_ATI_MAX_INST == DS_RMSM_ATI_V4_INST) &&
       ( NULL != info_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->um_inst_ptr[DS_RMSM_ATI_V6_INST] ) &&
       (DS_INSTANCE_STATE_UP != ds_3gpp_rmsm_ati_get_v6_um_inst_state (instance ) ) )
  {  
    change_state = FALSE;
  }
  else if ( ( clt_hdl % DS_RMSM_ATI_MAX_INST == DS_RMSM_ATI_V6_INST) &&
            ( NULL != info_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->um_inst_ptr[DS_RMSM_ATI_V4_INST] ) &&
            (DS_INSTANCE_STATE_UP != ds_3gpp_rmsm_ati_get_v4_um_inst_state (instance ) ) )
  {
    change_state = FALSE;
  }
            
  mox_user_info = info_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->mo_ex_data_info;
  /*---------------------------------------------------------------------
    Um instance is up. 
  ---------------------------------------------------------------------*/
  if( info_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->connect_state == DS_3GPP_ATIF_PDP_ACTIVATING )
  {
    if (change_state == TRUE )
    {
      /*See if any MO excep data is pending to be sent*/
      if((mox_user_info != NULL) && (mox_user_info->is_mox_data_pending))
      {
        if(FALSE == ds_3gpp_rmsm_ati_send_user_data(instance,mox_user_info->profile_num,
                          mox_user_info->pkt_data_len,mox_user_info->pkt_ptr,
                          mox_user_info->rai_ind,mox_user_info->is_mox_data_pending))
        {
          /*Free the packet pointer*/
          dsm_free_packet(mox_user_info->pkt_ptr);
        }
        
        mox_user_info->is_mox_data_pending = FALSE;
        /* Free the mox user info buffer */
        DS_SYSTEM_HEAP_MEM_FREE(mox_user_info);
        info_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->mo_ex_data_info = NULL;
        at_cmd_type = DSAT_RMSM_CSODCP_CMD;
      }
      
      /* Send success of activation  if any of the instance is in UP state */
      info_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->connect_state = DS_3GPP_ATIF_PDP_ACTIVATED;
      /* Call ATCoP complete handler ,with OK */
      dsat_rmsm_info.result = DSAT_OK;
      dsat_rmsm_info.user_info_ptr = info_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->user_info_ptr;
      dsat_rmsm_done_handler(at_cmd_type,dsat_rmsm_info);
      /* Clear the user infor pointer for activation */
      info_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->user_info_ptr = NULL;

      /* send Packet event reporting */
      num_prof_entries = ds_3gpp_rmsm_ati_get_profilelist_from_prof_mask
                      (info_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->profile_mask,
                      profile_num_list);
      if(num_prof_entries > 0)
      {
        pkt_evt_info.cid = profile_num_list[0];
      }
      dsat_rmsm_pkt_event_rpt_handler(DSAT_PDN_ACTIVATE,&pkt_evt_info);
    }
  }
  /* We are in de-activating phase */
  else if ((info_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->connect_state == DS_3GPP_ATIF_PDP_DEACTIVATING) || 
           (info_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->connect_state == DS_3GPP_ATIF_PDP_DEACTIVATED) )
  {
    if((mox_user_info != NULL) && (mox_user_info->is_mox_data_pending))
    {
      if(mox_user_info->pkt_ptr)
      {
        /*Free the packet pointer*/
        dsm_free_packet(mox_user_info->pkt_ptr);
      }
      mox_user_info->is_mox_data_pending = FALSE;
      /* Free the mox user info buffer */
      DS_SYSTEM_HEAP_MEM_FREE(mox_user_info);
      info_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->mo_ex_data_info = NULL;
      at_cmd_type = DSAT_RMSM_CSODCP_CMD;
    }
    
    /* failure of +CGACT de-activating command , 
       return ERROR and de-allocate the instance */
    DS_MSG0(MSG_LEGACY_ERROR,"+CGACT de-activation is failure ");
    dsat_rmsm_info.result = DSAT_ERROR;
    dsat_rmsm_info.reason_type = DSAT_RMSM_IFACE_NET_DOWN_REASON;
    dsat_rmsm_info.reason.iface_net_down= DS_NET_DOWN_REASON_NOT_SPECIFIED;
    dsat_rmsm_info.user_info_ptr = info_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->user_info_ptr;
    dsat_rmsm_done_handler(at_cmd_type,dsat_rmsm_info);

    /* deallocate only if no other profiles associated with the instance */
    if(info_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->profile_mask == 0)
    {
      DS_SYSTEM_HEAP_MEM_FREE(info_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->um_inst_ptr[DS_RMSM_ATI_V4_INST]);
      DS_SYSTEM_HEAP_MEM_FREE(info_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->um_inst_ptr[DS_RMSM_ATI_V6_INST]);
      DS_SYSTEM_HEAP_MEM_FREE(info_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->um_inst_ptr[DS_RMSM_ATI_NONIP_INST]);
    ds_3gpp_rmsm_ati_deallocate_instance (instance);
  }
  }
  else
  {
    DS_MSG0(MSG_LEGACY_HIGH,"Ignoring as not in proper connect state ");
  }
  
func_exit:
  if(err_val != DS_LOCAL_ERR_VAL_NONE)
  {
    DS_MSG1(MSG_LEGACY_ERROR,"ds_3gpp_rmsm_ati_um_inst_up_handler local_err: %d",
                              err_val);
  }
  return;
} /* ds_3gpp_rmsm_ati_um_inst_up_handler */

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_ATI_UM_INST_DOWN_HANDLER

  DESCRIPTION
    This function process the instance_DOWN event posted to DS_3GPP_RMSM AT 
    interface

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void ds_3gpp_rmsm_ati_um_inst_down_handler
(
  ds_3gpp_rmsm_at_type*  rmsm_at_ptr
)
{
  ds_3gpp_ati_pdp_state_type     *info_ptr= NULL;
  dsat_result_enum_type          result = DSAT_ASYNC_CMD;
  dsat_rmsm_info_s_type          dsat_rmsm_info = {0};
  dsat_cgerep_evt_info_s_type    pkt_evt_info = {0};
  ds_3gpp_rmsm_at_instance_type  instance = DS_3GPP_RMSM_AT_UM_MAX_INSTANCES;
  ds_fwk_clnt_handle_type        clt_hdl;
  ds_local_err_val_e_type        err_val = DS_LOCAL_ERR_VAL_NONE;
  uint8                          profile_num_list[DS_3GPP_MAX_PDN_CONTEXT] = {0};
  ds_profile_type_s              profile_info = {{0}};
  uint8                          num_prof_entries = 0;
  int                            idx = 0;
  ds_mo_ex_user_info_s           *mox_user_info = NULL;
  dsat_rmsm_cmd_type             at_cmd_type = DSAT_RMSM_CGACT_CMD;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(rmsm_at_ptr != NULL)
  {
    instance = rmsm_at_ptr->ds_3gpp_rmsm_at_instance;
    clt_hdl  = rmsm_at_ptr->client_handle;
  }
  else
  {
    err_val = DS_LOCAL_ERR_VAL_0;
    goto func_exit;
  }
  
  if((instance < 0 ) || (instance >= DS_3GPP_RMSM_AT_UM_MAX_INSTANCES))
  {
    err_val = DS_LOCAL_ERR_VAL_1;
    goto func_exit;
  }

  info_ptr = ds_3gpp_rmsm_get_info_ptr_from_instance(instance);
  if (NULL == info_ptr)
  {
    err_val = DS_LOCAL_ERR_VAL_2;
    goto func_exit; 
  }

  if(!ds_3gpp_rmsm_ati_pdp_state_type_validate(info_ptr))
  {
    err_val = DS_LOCAL_ERR_VAL_3;
    goto func_exit;
  } 

  num_prof_entries = ds_3gpp_rmsm_ati_get_profilelist_from_prof_mask(
                      info_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->profile_mask,
                      profile_num_list);
  if(num_prof_entries > 1)
  {
    for(idx = 0; idx < num_prof_entries; idx++)
    {
      if(DS_PROFILE_RESULT_SUCCESS != 
            ds_profile_setting_from_profile_id(profile_num_list[idx], &profile_info))
      {
        DS_MSG1(MSG_LEGACY_HIGH,"Unable to fetch profile params for profile %d",
                                                         profile_num_list[idx]);
        continue;
      }
      switch(profile_info.context.pdp_type)
      {
        case DS_PROFILE_PDP_IPV4:
          if(clt_hdl % DS_RMSM_ATI_MAX_INST == DS_RMSM_ATI_V4_INST)
          {
            info_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->profile_mask &=
                               ~(1 << (profile_num_list[idx] - 1));
            
          }
          break;
          
        case DS_PROFILE_PDP_IPV6:
          if(clt_hdl % DS_RMSM_ATI_MAX_INST == DS_RMSM_ATI_V6_INST)
          {
            info_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->profile_mask &=
                   ~(1 << (profile_num_list[idx] - 1));
          }
          break;
          
        case DS_PROFILE_PDP_IPV4V6:
          if(((clt_hdl % DS_RMSM_ATI_MAX_INST == DS_RMSM_ATI_V4_INST) &&
             (DS_INSTANCE_STATE_DOWN == ds_3gpp_rmsm_ati_get_v6_um_inst_state(instance))) ||
             ((clt_hdl % DS_RMSM_ATI_MAX_INST == DS_RMSM_ATI_V6_INST) &&
             (DS_INSTANCE_STATE_DOWN == ds_3gpp_rmsm_ati_get_v4_um_inst_state(instance))))
          {
            info_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->profile_mask &=
                   ~(1 << (profile_num_list[idx] - 1));            
          }
          break;
             
        case DS_PROFILE_PDP_NON_IP:
          if(clt_hdl % DS_RMSM_ATI_MAX_INST == DS_RMSM_ATI_NONIP_INST)
          {
            info_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->profile_mask &=
                   ~(1 << (profile_num_list[idx] - 1));          
          }
          break;

        default:
          DS_MSG0(MSG_LEGACY_HIGH,"Invalid profile pdp type");
          break;
      }
    }
  }
  
  /*---------------------------------------------------------------------
     Um instance is DOWN. 
  ---------------------------------------------------------------------*/
  if( info_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->connect_state == DS_3GPP_ATIF_PDP_DEACTIVATING )
  {
 /* success of de-activation until both ipv4 and ipv6 ifaces down*/
    if(((clt_hdl % DS_RMSM_ATI_MAX_INST == DS_RMSM_ATI_V6_INST) &&
        (NULL == info_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->um_inst_ptr[DS_RMSM_ATI_V4_INST])) ||
        ((clt_hdl % DS_RMSM_ATI_MAX_INST == DS_RMSM_ATI_V4_INST) &&
        (NULL == info_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->um_inst_ptr[DS_RMSM_ATI_V6_INST])) ||
        (clt_hdl % DS_RMSM_ATI_MAX_INST == DS_RMSM_ATI_NONIP_INST))
    {
      result = DSAT_OK;
    }
    else
    {
      result = DSAT_ASYNC_CMD;
    }
  }
  else if ( info_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->connect_state == DS_3GPP_ATIF_PDP_ACTIVATING )
  {
    if ( ( clt_hdl % DS_RMSM_ATI_MAX_INST == DS_RMSM_ATI_V6_INST) &&
          (NULL != info_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->um_inst_ptr[DS_RMSM_ATI_V4_INST]) )
    {
      if (DS_INSTANCE_STATE_UP == ds_3gpp_rmsm_ati_get_v4_um_inst_state(instance))
      {
        result = DSAT_OK;
        DS_MSG1(MSG_LEGACY_HIGH,"For ipv4v6, v6 down,ipv4 succeeded AT Inst %d, returning OK",instance);
      }
      else
      {
        DS_MSG1(MSG_LEGACY_HIGH,"For ipv4v6, v6 down, v4 not up, returning ASYNC CMD , inst %d",instance);
        result = DSAT_ASYNC_CMD;
      }
    }
    else if ( ( clt_hdl% DS_RMSM_ATI_MAX_INST == DS_RMSM_ATI_V4_INST ) &&
          (NULL != info_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->um_inst_ptr[DS_RMSM_ATI_V6_INST]) )
    {
      if (DS_INSTANCE_STATE_UP == ds_3gpp_rmsm_ati_get_v6_um_inst_state(instance))
      {
        result = DSAT_OK;
        DS_MSG1(MSG_LEGACY_HIGH,"For ipv4v6, v4 down,ipv6 succeded AT Inst %d, returning OK",
                           instance);
      }
      else
      {
        DS_MSG1(MSG_LEGACY_HIGH,"For ipv4v6, v4 down, v6 not up, returning ASYNC CMD , inst %d",
                           instance);
        result = DSAT_ASYNC_CMD;
      }
    }
    else
    {
      /* failure of +CGACT command , return ERROR and 
         de-allocate the instance */
      DS_MSG1(MSG_LEGACY_ERROR,"+CGACT activation is failure AT Inst %d result ERROR",instance);
      result = DSAT_ERROR;
    }
  }
  if( DSAT_ASYNC_CMD != result )
  {
    mox_user_info = info_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->mo_ex_data_info;
    if((mox_user_info != NULL) && (mox_user_info->is_mox_data_pending))
    {
      if(mox_user_info->pkt_ptr)
      {
        /*Free the packet pointer*/
        dsm_free_packet(mox_user_info->pkt_ptr);
      }
      mox_user_info->is_mox_data_pending = FALSE;
      /* Free the mox user info buffer */
      DS_SYSTEM_HEAP_MEM_FREE(mox_user_info);
      info_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->mo_ex_data_info = NULL;
      at_cmd_type = DSAT_RMSM_CSODCP_CMD;
    }
    
    /* Send final response to ATCoP */
    dsat_rmsm_info.result = result;
    dsat_rmsm_info.reason_type = DSAT_RMSM_IFACE_NET_DOWN_REASON;
    dsat_rmsm_info.reason.iface_net_down= 
                 rmsm_at_ptr->event_info.pdn_down_info.netdown_reason;
    dsat_rmsm_info.user_info_ptr = info_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->user_info_ptr;
    dsat_rmsm_done_handler(at_cmd_type,dsat_rmsm_info);

    /* send Packet event reporting */
    if(num_prof_entries > 0)
    {
      pkt_evt_info.cid = profile_num_list[0];
    }
    dsat_rmsm_pkt_event_rpt_handler(DSAT_PDN_DEACTIVATE,&pkt_evt_info);
  }

  /* if pending data is present then QCDGEN command 
     response is pending , send result code 
  */
  if( (info_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->pending_data !=0) &&
                     ((clt_hdl % DS_RMSM_ATI_MAX_INST) == DS_RMSM_ATI_V4_INST))
  {
    dsat_rmsm_info.result = DSAT_ERROR;
    dsat_rmsm_info.user_info_ptr = info_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->user_info_ptr;
    dsat_rmsm_info.reason_type = DSAT_RMSM_INVALID_REASON;  
    dsat_rmsm_done_handler(DSAT_RMSM_QCDGEN_CMD,dsat_rmsm_info);
    /*-------------------------------------------------------------------------  
      Disable the PS signal
    -------------------------------------------------------------------------*/
    ps_disable_sig(PS_RMSM_AT_UM_RX_Q_SIGNAL);
  }

  if(clt_hdl % DS_RMSM_ATI_MAX_INST == DS_RMSM_ATI_V4_INST)
  {
    DS_SYSTEM_HEAP_MEM_FREE(info_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->um_inst_ptr[DS_RMSM_ATI_V4_INST]);
    info_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->um_inst_ptr[DS_RMSM_ATI_V4_INST] = NULL;
  }
  else if(clt_hdl % DS_RMSM_ATI_MAX_INST == DS_RMSM_ATI_V6_INST)
  {
    DS_SYSTEM_HEAP_MEM_FREE(info_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->um_inst_ptr[DS_RMSM_ATI_V6_INST]);
    info_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->um_inst_ptr[DS_RMSM_ATI_V6_INST] = NULL;
  }
  else if(clt_hdl % DS_RMSM_ATI_MAX_INST == DS_RMSM_ATI_NONIP_INST)
  {
    DS_SYSTEM_HEAP_MEM_FREE(info_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->um_inst_ptr[DS_RMSM_ATI_NONIP_INST]);
    info_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->um_inst_ptr[DS_RMSM_ATI_NONIP_INST] = NULL;
  }
  /* de allocate the instance already brought up 
     if all instance pointers are NULL */
  if( ( info_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->um_inst_ptr[DS_RMSM_ATI_V4_INST] == NULL ) &&
      ( info_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->um_inst_ptr[DS_RMSM_ATI_V6_INST] == NULL ) &&
      ( info_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->um_inst_ptr[DS_RMSM_ATI_NONIP_INST] == NULL)) 
  {
    ds_3gpp_rmsm_ati_deallocate_instance (instance);
  }
      
func_exit:
  if(err_val != DS_LOCAL_ERR_VAL_NONE)
  {
    DS_MSG1(MSG_LEGACY_ERROR,"ds_3gpp_rmsm_ati_um_inst_down_handler local_err: %d",
                              err_val);
  }
  return;
}/* ds_3gpp_rmsm_ati_um_inst_down_handler*/
      
/*===========================================================================
  FUNCTION DS_3GPP_RMSM_ATI_CGACT_QUERY

  DESCRIPTION
    This function returns teh profile status of all PDP contexts. It 
    retrieves data from DS_3GPP_RMSM_ST interface and Mode handler for
    valid PDP profiles.

  RETURN VALUE
    DSAT_OK -- if SUCCESS
    DSAT_ERROR -- if FAILURE

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
dsat_result_enum_type ds_3gpp_rmsm_ati_cgact_query
(
  ds_3gpp_atif_profile_status_info_type * profile_status_info_ptr
)
{
  ds_3gpp_rmsm_at_instance_type  instance = 0;
  int                            i=0;
  uint8                          profile_num;
  uint8                          profile_count = 0;
  ds_profile_list_stype          *profile_list = NULL;
  ds_profile_ident_info          prof_ident = {{0}};
  ds_pdn_mgr_s*                  pdn_mgr_p = NULL;
  ds_local_err_val_e_type        err_val = DS_LOCAL_ERR_VAL_NONE;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  DS_MSG0(MSG_LEGACY_HIGH,"ds_3gpp_rmsm_ati_cgact_query ");
  if (NULL == profile_status_info_ptr)
  {
    err_val = DS_LOCAL_ERR_VAL_0;
    goto func_exit;
  }

  DS_SYSTEM_HEAP_MEM_ALLOC(profile_list, (sizeof(ds_profile_list_stype)*DS_PROFILE_MAX_PROFILE_NUM),
                                        ds_profile_list_stype*);
  if(profile_list == NULL)
  {
    err_val = DS_LOCAL_ERR_VAL_1;
    goto func_exit;
  }
  memset(profile_list,0,(sizeof(ds_profile_list_stype)*DS_PROFILE_MAX_PROFILE_NUM));
  memset((void*)profile_status_info_ptr,0x0,sizeof(ds_3gpp_atif_profile_status_info_type));

  profile_count = ds_profile_get_profile_list(profile_list);

  profile_status_info_ptr->profile_count = profile_count;

  for (i=0; i < profile_count ;i++)
  {

    memscpy(prof_ident.apn, sizeof(profile_list[i].apn ), 
                profile_list[i].apn, sizeof(profile_list[i].apn ));
 
    prof_ident.pdp_type = profile_list[i].pdp_type;   
    profile_num = ds_profile_get_profile_id(prof_ident);
    instance = ds_3gpp_rmsm_ati_get_inst_num_from_profile(profile_num);
    if(instance != DS_3GPP_RMSM_AT_UM_MAX_INSTANCES)
    {
      DS_MSG1(MSG_LEGACY_HIGH,"profile %d activated by +CGACT",profile_num);
      profile_status_info_ptr->list[i].profile_num = profile_num;
      profile_status_info_ptr->list[i].act_status = DS_3GPP_ATIF_PDP_ACTIVATED;
    }
    else
    {
      profile_status_info_ptr->list[i].profile_num = profile_num;
      /* Query LTE MH to check whether the profile is active */
      pdn_mgr_p = ds_pdn_mgr_get_pdn_from_profile_id(profile_num);
      if((NULL != pdn_mgr_p) && ( DS_PDN_MGR_STATE_DOWN != pdn_mgr_p->state))
      {
        /* Update the state as active if state is not in DOWN state */
        profile_status_info_ptr->list[i].act_status = DS_3GPP_ATIF_PDP_ACTIVATED;
      }
    }/* else */
  }/* for */

  DS_SYSTEM_HEAP_MEM_FREE(profile_list);
  return DSAT_OK;
  
func_exit:
    DS_MSG1(MSG_LEGACY_ERROR,"ds_3gpp_rmsm_ati_cgact_query local_err: %d", err_val);
    return DSAT_ERROR;
}/* ds_3gpp_rmsm_ati_cgact_query */

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_ATI_PROCESS_QCDGEN

  DESCRIPTION
    This function process data to a context activated
    using CGACT PDP IP profile.

  RETURN VALUE
    DSAT_ERROR -- In case of any FAILURE
    DSAT_ASYNC_CMD -- in case of SUCCESS

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
dsat_result_enum_type ds_3gpp_rmsm_ati_process_qcdgen
(
  uint32                                  profile_number,
  uint32                                  total_data_len,
  void *                                  user_info_ptr
)
{

  ds_3gpp_rmsm_at_instance_type  instance = DS_3GPP_RMSM_AT_UM_MAX_INSTANCES;
  ds_profile_type_s              profile_info = {{0}};
  ds_3gpp_ati_pdp_state_type     *info_ptr = NULL;
  ds_at_pdn_instance             *pdn_inst_ptr = NULL;
  ds_local_err_val_e_type        err_val = DS_LOCAL_ERR_VAL_NONE;
  ds_profile_status_etype        result = DS_PROFILE_RESULT_FAIL;
  ds_bearer_mgr_s                *bearer_mgr_p = NULL;
  boolean                        is_um_flow_enabled = FALSE;
  ds_rmsm_ati_inst_type_e        rm_sm_inst = DS_RMSM_ATI_MIN_INST;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check whether the profile is already activated by +CGACT */
  instance = ds_3gpp_rmsm_ati_get_inst_num_from_profile(profile_number);
  if ( DS_3GPP_RMSM_AT_UM_MAX_INSTANCES == instance)
  {
    err_val = DS_LOCAL_ERR_VAL_0;
    goto func_exit;
  }
  info_ptr = ds_3gpp_rmsm_get_info_ptr_from_instance(instance);
  if(!ds_3gpp_rmsm_ati_pdp_state_type_validate(info_ptr))
  {
    err_val = DS_LOCAL_ERR_VAL_1;
    goto func_exit;
  } 
  result = ds_profile_setting_from_profile_id((int8)profile_number,&profile_info);
  if( result != DS_PROFILE_RESULT_SUCCESS ) 
  {
    err_val = DS_LOCAL_ERR_VAL_5;
    goto func_exit;
  }
  else
  {
      DS_MSG1(MSG_LEGACY_HIGH,"Profile:%d valid : using it for qcdgen", profile_number);
  }
  /*QCDGEN command onlyfor IPV4,Ipv4v6 context where v4 instance is valid.*/
  if( (profile_info.context.pdp_type == DS_PROFILE_PDP_NON_IP) ||    //to remove this check here for non IP data
      ((NULL == info_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->um_inst_ptr[DS_RMSM_ATI_V4_INST] ) &&
        (NULL == info_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->um_inst_ptr[DS_RMSM_ATI_V6_INST] )))
  {
    err_val = DS_LOCAL_ERR_VAL_2;
    goto func_exit;
  }

 switch(profile_info.context.pdp_type)
 {
   case DS_PROFILE_PDP_IPV4:
    rm_sm_inst = DS_RMSM_ATI_V4_INST;
    pdn_inst_ptr = info_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->um_inst_ptr[DS_RMSM_ATI_V4_INST];
    break;
   case DS_PROFILE_PDP_IPV6:
    rm_sm_inst = DS_RMSM_ATI_V6_INST;
    pdn_inst_ptr = info_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->um_inst_ptr[DS_RMSM_ATI_V6_INST];
    break;
   case DS_PROFILE_PDP_IPV4V6:
    if (info_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->um_inst_ptr[DS_RMSM_ATI_V4_INST])
    {
	  pdn_inst_ptr = info_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->um_inst_ptr[DS_RMSM_ATI_V4_INST];
	  rm_sm_inst = DS_RMSM_ATI_V4_INST;
    }
    else
    {
      pdn_inst_ptr = info_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->um_inst_ptr[DS_RMSM_ATI_V6_INST];
	  rm_sm_inst = DS_RMSM_ATI_V6_INST;	
    }
    break;
   default:
    break;
}

 /* check whether DATA len is > DS_3GPP_RMSM_ATI_MIN_DATA_LEN*/
  if (DS_3GPP_RMSM_ATI_MIN_DATA_LEN > total_data_len)
  {
    err_val = DS_LOCAL_ERR_VAL_3;
    goto func_exit;
  }

  /* Generate data */
  if(NULL == pdn_inst_ptr)
  {
    err_val = DS_LOCAL_ERR_VAL_4;
    goto func_exit;
  }
  
  if (rm_sm_inst == DS_RMSM_ATI_V4_INST)
  {
    bearer_mgr_p = ds_fwk_get_default_bearer(pdn_inst_ptr->fw_inst, IPV4_ADDR);
  }
  else if(rm_sm_inst == DS_RMSM_ATI_V6_INST)
  {
    bearer_mgr_p = ds_fwk_get_default_bearer(pdn_inst_ptr->fw_inst, IPV6_ADDR);
  }
  if(NULL != bearer_mgr_p)
  {
    is_um_flow_enabled = ds_bearer_mgr_is_tx_flow_enabled(bearer_mgr_p);
    DS_3GPP_RMSM_SET_FLOW_STATE(instance,is_um_flow_enabled);
    DS_MSG2(MSG_LEGACY_HIGH,"Instance : %d  UM Flow state : %d",instance,
                                      DS_3GPP_RMSM_GET_FLOW_STATE(instance));
  }
  else
  {
    err_val = DS_LOCAL_ERR_VAL_5;
    goto func_exit;
  }

  if(DS_3GPP_RMSM_GET_FLOW_STATE(instance) == FALSE)
  {
    /*Flow is Disabled for this instance*/
    err_val = DS_LOCAL_ERR_VAL_6;
    goto func_exit;
  }
  
  info_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->pending_data = total_data_len;
  /* Update user info pointer */
  info_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->user_info_ptr = user_info_ptr;

  /*-------------------------------------------------------------------------  
    Set the handler for the PS_RMSM_AT_UM_RX_Q_SIGNAL
    signal for data transfer.
  -------------------------------------------------------------------------*/
  (void)ps_set_sig_handler(
                    PS_RMSM_AT_UM_RX_Q_SIGNAL,
                    ds_3gpp_rmsm_ip_tx_um_data_cb, 
                    (void *)instance);
  
  /*-------------------------------------------------------------------------  
    Enable it.
  -------------------------------------------------------------------------*/
  ps_enable_sig(PS_RMSM_AT_UM_RX_Q_SIGNAL);

  /*---------------------------------------------------------------------
   Send signal to PS to send data to UM instance to start flow
  ---------------------------------------------------------------------*/
  PS_SET_SIGNAL( PS_RMSM_AT_UM_RX_Q_SIGNAL );

  return DSAT_ASYNC_CMD;
  
func_exit:
  DS_MSG1(MSG_LEGACY_ERROR,"ds_3gpp_rmsm_ati_process_qcdgen local_err: %d", err_val);
  return DSAT_ERROR;
} /* ds_3gpp_rmsm_ati_process_qcdgen */

/*===========================================================================
FUNCTION DS_3GPP_RMSM_IP_TX_UM_DATA_CB

DESCRIPTION
  This function forwards pkts received for Um interface 
  from ATCoP . 

PARAMETERS   
    sig    - Signal raised
    instance   - instance of teh instance for which data transfer 
                 is pending
DEPENDENCIES 
  None.
  
RETURN VALUE 
  FALSE -- failure
  TRUE  -- Success

SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_3gpp_rmsm_ip_tx_um_data_cb
(
  ps_sig_enum_type sig,
  void             *pkt_instance
)
{
  ds_3gpp_rmsm_at_instance_type   instance = DS_3GPP_RMSM_AT_UM_MAX_INSTANCES;
  ds_3gpp_ati_pdp_state_type     *info_ptr=NULL;
  ds_command_type                *cmd_ptr;
  ds_3gpp_rmsm_at_type           *data_ptr = NULL;
  boolean                         result = TRUE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  instance = (ds_3gpp_rmsm_at_instance_type)pkt_instance;

  DS_RMSM_ENTER_CRIT_SECTION(&ds_3gpp_rmsm_crit_sect);
  info_ptr = ds_3gpp_rmsm_get_info_ptr_from_instance(instance);

    if(!ds_3gpp_rmsm_ati_pdp_state_type_validate(info_ptr))
  {
    DS_RMSM_LEAVE_CRIT_SECTION(&ds_3gpp_rmsm_crit_sect);
    DS_MSG0(MSG_LEGACY_ERROR,"Not a valid instance ds_3gpp_rmsm_ati_pdp_state");
    return FALSE;
  } 

  if (info_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->pending_data != 0 )
  {
    if(info_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->um_inst_ptr[DS_RMSM_ATI_V4_INST] != NULL)
    {
    result = ds_3gpp_rmsm_ati_send_data(info_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->um_inst_ptr[DS_RMSM_ATI_V4_INST]->fw_inst,
                                      &info_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->pending_data);
    }
    else if (info_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->um_inst_ptr[DS_RMSM_ATI_V6_INST] != NULL)
    {
      result = ds_3gpp_rmsm_ati_send_data(info_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->um_inst_ptr[DS_RMSM_ATI_V6_INST]->fw_inst,
                                       &info_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->pending_data);
    }		
  }
  else
  {
    /* This should not have happened */
    DS_MSG0(MSG_LEGACY_ERROR,"No pending data");

    DS_RMSM_LEAVE_CRIT_SECTION(&ds_3gpp_rmsm_crit_sect);
    return result;
  }
  /*Override result if Flow is Disabled*/
  if(DS_3GPP_RMSM_GET_FLOW_STATE(instance) == FALSE)
  {
    /* UM Flow is Disabled */
    DS_MSG0(MSG_LEGACY_ERROR,"Um Flow Disabled ! Stop sending further");
    result = FALSE;
  }
  
  if(result == FALSE)
  {
    info_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->pending_data=0;
    DS_MSG0(MSG_LEGACY_ERROR,"Failed to send data to UM instance");
  }
  
   DS_RMSM_LEAVE_CRIT_SECTION(&ds_3gpp_rmsm_crit_sect);
  /* post a command to set signal again */
  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_3gpp_rmsm_at_type));
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    return FALSE;
  }

  cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_RMSM_AT_SEND_DATA;
  data_ptr = (ds_3gpp_rmsm_at_type*)cmd_ptr->cmd_payload_ptr;

  data_ptr->ds_3gpp_rmsm_at_instance = instance;
  data_ptr->result = result;
  ds_put_cmd( cmd_ptr);
  /* always return true */
  return TRUE;
}/* ds_3gpp_rmsm_ip_tx_um_data_cb */

/*===========================================================================

FUNCTION DS_3GPP_RMSM_ATI_GENERATE_IP_FRAME

DESCRIPTION
  This function allocates a dsm_item_type and uses dsm_pushdown_tail() to
  fill with canned IP frame of total length specified and using values for
  fields:

DEPENDENCIES
  None

RETURN VALUE
  returns a boolean: TRUE on success, FALSE on failure.

SIDE EFFECTS
  On success, sets ip_frame_buf to point to allocated and IP frame filled 
  dsm_item_type chain.

===========================================================================*/
boolean ds_3gpp_rmsm_ati_generate_ip_frame
(
  dsm_item_type **ip_frame,
  uint16 total_data_len
)
{
  /* Default values for fields of IP header */
#define VERSION       4
#define IHL           5           /* Size in 32b words */
#define TOS           0
#define DEF_PKT_LEN   1500      
#define IDENT         0
#define TTL           32
#define PROTO         17          /* UDP */
#define SRC_ADDR      0xC0A80001  /* 192.168.0.1 */
#define DEST_ADDR     0xC0A80002  /* 192.168.0.2 */
/* Other constants */
#define MIN_PKT_SIZE  ((IHL << 2) + 1) /* IP header + 1 byte data */
#define CALC_CKSUM    0

  boolean result = TRUE;
  static uint16 identifier = 0;
  uint16 pkt_data_len;
  struct ip ip_hdr;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_MSG1(MSG_LEGACY_HIGH,"data len = %d",total_data_len);
  ASSERT( NULL != ip_frame );
  ASSERT((total_data_len >= MIN_PKT_SIZE) && 
         (total_data_len <= DEF_PKT_LEN));

  if ( NULL == (*ip_frame = dsm_new_buffer(DSM_DS_LARGE_ITEM_POOL)) )
  {
    DS_MSG0(MSG_LEGACY_ERROR,"Running out of DSM large items");
    result = FALSE;
  }
  else
  {
    memset((void *)&ip_hdr, 0, sizeof(struct ip));

    ip_hdr.version       = VERSION;
    ip_hdr.tos           = TOS;
    ip_hdr.length        = total_data_len;
    ip_hdr.id            = ++identifier;
    ip_hdr.ttl           = TTL;
    ip_hdr.protocol      = PROTO;
    ip_hdr.ihl           = IHL;
    ip_hdr.source.ps_s_addr = ps_htonl(SRC_ADDR);              
    ip_hdr.dest.ps_s_addr   = ps_htonl(DEST_ADDR);              
    pkt_data_len = total_data_len - (IHL << 2);

    /* If pushing IP packet header or packet data onto tail of dsm item chain
       should fail, exit with result set FALSE */
    if ( pkt_data_len != dsm_pushdown_tail( ip_frame, 
                                            (void*)NULL, 
                                            pkt_data_len,
                                                     DSM_ITEM_POOL(*ip_frame) ) ||
         !ps_ip4_hdr_create( ip_frame,
                             &ip_hdr,
                             IP_CS_NEW ) )
    {
      DS_MSG0(MSG_LEGACY_ERROR,"Error creating dummy IP frame");
      result = FALSE;
    }
  }
  if ( FALSE == result && 
       *ip_frame != NULL )
  {
    dsm_free_packet(ip_frame);
  }
  return result;
} /* ds_3gpp_rmsm_ati_generate_ip_frame */


/*===========================================================================

FUNCTION DS_3GPP_RMSM_ATI_GENERATE_IPV6_FRAME

DESCRIPTION
  This function allocates a dsm_item_type and uses dsm_pushdown_tail() to
  fill with canned IP frame of total length specified and using values for
  fields:

DEPENDENCIES
  None

RETURN VALUE
  returns a boolean: TRUE on success, FALSE on failure.

SIDE EFFECTS
  On success, sets ip_frame_buf to point to allocated and IP frame filled 
  dsm_item_type chain.

===========================================================================*/
boolean ds_3gpp_rmsm_ati_generate_ipv6_frame
(
  dsm_item_type **ip_frame,
  uint16 total_data_len
)
{
  /* Default values for fields of IP header */
#define V6_VERSION       6
/* Other constants */
#define MIN_V6_PKT_SIZE  ((40) + 1) /* IPv6 header + 1 byte data */

  boolean result = TRUE;
  uint16 pkt_data_len;
  
  ip6_hdr_type   ip6_hdr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_MSG1(MSG_LEGACY_HIGH,"ds_3gpp_rmsm_ati_generate_ipv6_frame data len = %d",total_data_len);
  ASSERT( NULL != ip_frame );
  ASSERT((total_data_len >= MIN_V6_PKT_SIZE) && 
         (total_data_len <= DEF_PKT_LEN));

  if ( NULL == (*ip_frame = dsm_new_buffer(DSM_DS_LARGE_ITEM_POOL)) )
  {
    DS_MSG0(MSG_LEGACY_ERROR,"Running out of DSM large items");
    result = FALSE;
  }
  else
  {
    memset((void *)&ip6_hdr, 0, sizeof(ip6_hdr_type));

    ip6_hdr.hdr_type       = PS_IPV6_BASE_HDR;
	
    ip6_hdr.hdr_body.base_hdr.src_addr.in6_u.u6_addr64[1] = 0xB30A8C0AB30A8C0ULL;
    ip6_hdr.hdr_body.base_hdr.dst_addr.in6_u.u6_addr64[1] = 0xB30A8C0AB30A8C1ULL;
	ip6_hdr.hdr_body.base_hdr.version = V6_VERSION;
    ip6_hdr.hdr_body.base_hdr.next_hdr = PROTO; 
             
    pkt_data_len = total_data_len - (IHL << 2);

    /* If pushing IP packet header or packet data onto tail of dsm item chain
       should fail, exit with result set FALSE */
    if ( pkt_data_len != dsm_pushdown_tail( ip_frame, 
                                            (void*)NULL, 
                                            pkt_data_len,
                                                     DSM_ITEM_POOL(*ip_frame) ) ||
         !ps_ip6_hdr_create_ex( ip_frame,
                             &ip6_hdr) )
    {
      DS_MSG0(MSG_LEGACY_ERROR,"Error creating dummy IP frame");
      result = FALSE;
    }
  }
  if ( FALSE == result && 
       *ip_frame != NULL )
  {
    dsm_free_packet(ip_frame);
  }
  return result;
} /* ds_3gpp_rmsm_ati_generate_ip_frame */

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_ATI_SEND_DATA

  DESCRIPTION
    This function sends data to a PS instance pointer 
    and updates the data len with remaining data_bytes.

  RETURN VALUE
    TRUE -- Success of data transfer
    FALSE -- failure of data transfer

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean ds_3gpp_rmsm_ati_send_data
(
  ds_fwk_s_type                    *fw_inst_ptr,
  uint32                           *data_len_ptr
)
{
  uint32                  data_len_to_send = 0;
  dsm_item_type           *ip_frame        = NULL;
  ps_tx_meta_info_type    *ati_tx_meta_info_ptr = NULL;
  ds_local_err_val_e_type err_val = DS_LOCAL_ERR_VAL_NONE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if((NULL == fw_inst_ptr) || (NULL == data_len_ptr))
  {
    err_val = DS_LOCAL_ERR_VAL_0;
    goto func_exit;
  }
  /* Choose for data_len a value less than or equal to maximimum frame
     size that won't leave a quantity of data less than minimum data 
     length left to send
  */
  if ( *data_len_ptr > DS_3GPP_RMSM_ATI_MAX_FRAME_SIZE )
  {
    data_len_to_send = 
      ( (*data_len_ptr - DS_3GPP_RMSM_ATI_MAX_FRAME_SIZE) >= 
        DS_3GPP_RMSM_ATI_MIN_DATA_LEN ) ?
      DS_3GPP_RMSM_ATI_MAX_FRAME_SIZE : 
      (uint32)(*data_len_ptr - DS_3GPP_RMSM_ATI_MIN_DATA_LEN);
  }
  else
  {
    data_len_to_send = *data_len_ptr;
  }
  
  if (fw_inst_ptr->v4_pdn_ptr != NULL)
  {
  /* Genarate IP frame */
  if( FALSE == ds_3gpp_rmsm_ati_generate_ip_frame(&ip_frame,
                                                  (uint16)data_len_to_send))
  {
    err_val = DS_LOCAL_ERR_VAL_1;
    goto func_exit;
  }
  }
  else
  {
    /* Genarate IP6 frame */
    if( FALSE == ds_3gpp_rmsm_ati_generate_ipv6_frame(&ip_frame,
                                                  (uint16)data_len_to_send))
    {
      err_val = DS_LOCAL_ERR_VAL_2;
      goto func_exit;
    }
  }

  PS_TX_META_INFO_AND_RT_META_INFO_GET(ati_tx_meta_info_ptr);
  if (ati_tx_meta_info_ptr == NULL ||
  PS_TX_META_GET_RT_META_INFO_PTR(ati_tx_meta_info_ptr) == NULL)
  {
    err_val = DS_LOCAL_ERR_VAL_2;
    goto func_exit;
  }
  
  ati_tx_meta_info_ptr->rt_meta_info_ptr->pkt_info.direction = TRUE;
  /* Transmit the generated packet */
  if( -1 == ds_fwk_tx_cmd(fw_inst_ptr,&ip_frame,ati_tx_meta_info_ptr))
  {
    dsm_free_packet(&ip_frame);
    err_val = DS_LOCAL_ERR_VAL_3;
    goto func_exit;
  }
  *data_len_ptr = *data_len_ptr - data_len_to_send;
  DS_MSG3(MSG_LEGACY_HIGH,"Sent data on fw_inst_ptr %x, qty %d, rem bytes %d",
             fw_inst_ptr, data_len_to_send,*data_len_ptr);
  return TRUE;

func_exit:
  DS_MSG1(MSG_LEGACY_ERROR,"ds_3gpp_rmsm_ati_send_data local_err: %d", err_val);
  return FALSE;
}/* ds_3gpp_rmsm_ati_send_data */

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_ATI_GET_PDP_ADDR

  DESCRIPTION
  
  This function returns the PDP addr of the PDN connection for which 
  the Profile number is passed.

  RETURN VALUE
    DSAT_OK -- if SUCCESS
    DSAT_ERROR -- if FAILURE

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
dsat_result_enum_type ds_3gpp_rmsm_ati_get_pdp_addr
(
  uint32                    profile_num,
  ds_profile_addr_type      *pdp_addr_ptr
)
{
  ds_pdn_mgr_s*     pdn_mgr_p = NULL;
  ds_fwk_s_type*    fw_inst_p = NULL;
  ps_ip_addr_type   ip_addr = {0};
  ds_profile_type_s profile_s = {{0}};
  ds_local_err_val_e_type err_val =  DS_LOCAL_ERR_VAL_NONE;
  ds_pdn_mgr_apn_table_s   *apn_table_p = NULL;
  ds_3gpp_rmsm_at_instance_type instance = DS_3GPP_RMSM_AT_UM_MAX_INSTANCES;
  /*----------------------------------------------------------------------*/
  
  if(NULL == pdp_addr_ptr)
  {
    DS_MSG0(MSG_LEGACY_ERROR, "NULL pointer passed");
    return DSAT_ERROR;
  }

  pdn_mgr_p = ds_pdn_mgr_get_pdn_from_profile_id(profile_num);

  if(ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p) == FALSE)
  {
  /*----------------------------------------------------------------------
    If PDN oorresponding to profile is not found, then check if APN table 
    has entry for this profile APN as resolved APN for some other PDN.
    Also, check this only if profile is already active with CGACT.
  ----------------------------------------------------------------------*/

    instance = ds_3gpp_rmsm_ati_get_inst_num_from_profile(profile_num);
    if(DS_3GPP_RMSM_AT_UM_MAX_INSTANCES == instance)
    {
      DS_MSG0(MSG_LEGACY_ERROR, "Profile not active !");
      return DSAT_ERROR;
    }
    	
    if((DS_PROFILE_RESULT_ERR_INVAL != 
                 ds_profile_setting_from_profile_id(profile_num,&profile_s)) &&
       (ds_apn_table_get_apn_table_ptr((char*)profile_s.context.apn, 
                                        &apn_table_p) != -1))
    {
      if(apn_table_p != NULL)
      {
        if(apn_table_p->v4_pdn_ptr != NULL)
        {
          pdn_mgr_p = apn_table_p->v4_pdn_ptr;
        }
        else if(apn_table_p->v6_pdn_ptr != NULL)
        {
          pdn_mgr_p = apn_table_p->v6_pdn_ptr;
        }
        else
        {
          pdn_mgr_p = apn_table_p->non_ip_pdn_ptr;
        }
      }

      if(ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p) == FALSE)
      {
        err_val = DS_LOCAL_ERR_VAL_1;	
        goto func_exit;
      }
    }
    else
    {
      err_val = DS_LOCAL_ERR_VAL_2;	
      goto func_exit;
    }
  }
  
  fw_inst_p = ds_fwk_get_inst_ptr_from_index(pdn_mgr_p->fw_index);

  if(pdn_mgr_p->state != DS_PDN_MGR_STATE_DOWN)
  {
    if((pdn_mgr_p->pdp_profile->context.pdp_type == DS_PROFILE_PDP_IPV4) ||
      (pdn_mgr_p->pdp_profile->context.pdp_type == DS_PROFILE_PDP_IPV4V6))
    {
      ds_fwk_get_ip_addr(fw_inst_p, IPV4_ADDR, &ip_addr);
      if((0 != ip_addr.addr.v4.ps_s_addr) && (IP_ADDR_INVALID != ip_addr.type))
      {
        pdp_addr_ptr->ip_vsn = DS_PROFILE_IP_V4;
        pdp_addr_ptr->pdp_addr.pdp_addr_ipv4 = ip_addr.addr.v4.ps_s_addr;
      }
    }

    if((pdn_mgr_p->pdp_profile->context.pdp_type == DS_PROFILE_PDP_IPV6) ||
      (pdn_mgr_p->pdp_profile->context.pdp_type == DS_PROFILE_PDP_IPV4V6))
    {
      ds_fwk_get_ip_addr(fw_inst_p, IPV6_ADDR, &ip_addr);
      if((0 != ip_addr.addr.v6.ps_s6_addr64[1]) && (IP_ADDR_INVALID != ip_addr.type))
      {
          pdp_addr_ptr->ip_vsn = (ds_profile_ip_version_enum_type)
          ((int)pdp_addr_ptr->ip_vsn + (int)DS_PROFILE_IP_V6);
          pdp_addr_ptr->pdp_addr.pdp_addr_ipv6.in6_u.u6_addr64[0] = ip_addr.addr.v6.ps_s6_addr64[0];
          pdp_addr_ptr->pdp_addr.pdp_addr_ipv6.in6_u.u6_addr64[1] = ip_addr.addr.v6.ps_s6_addr64[1];     
      }
    }
    
  }
  else
  {
    err_val =  DS_LOCAL_ERR_VAL_3;
    goto func_exit;
  }

  return DSAT_OK;
  
func_exit:
  if((err_val != DS_LOCAL_ERR_VAL_NONE) && (DS_PROFILE_RESULT_ERR_INVAL != 
      ds_profile_setting_from_profile_id(profile_num,&profile_s)))
  {
    memscpy((void *)pdp_addr_ptr,sizeof(ds_profile_addr_type),
              (void *)&(profile_s.pdp_addr), sizeof(ds_profile_addr_type));
  }   
  return DSAT_ERROR;
}/* ds_3gpp_rmsm_ati_get_pdp_addr */

/*===========================================================================
  FUNCTION   DS_3GPP_RMSM_ATI_READ_PRIM_DYNAMIC_PARAMS
  
  DESCRIPTION
    This function returns the primary dynamic params requested 
    by ATCoP for LTE.

  DEPENDENCIES
    None.
  
  RETURN VALUE
    FALSE: in case of Failure
    TRUE: If retrieval of dynamic data is success.
  
  SIDE EFFECTS
    None
===========================================================================*/
boolean ds_3gpp_rmsm_ati_read_prim_dynamic_params
(
  uint32                                      profile_number,
  ds_3gpp_atif_dynamic_params_info_type*      dynamic_param_ptr
)
{
  ds_3gpp_atif_primary_dynamic_info_type*  prim_info_ptr =NULL;
  ip_addr_type            prim_dns_addr;
  ip_addr_type            sec_dns_addr;
  ps_ip_addr_type         gateway_addr;
  ps_ip_addr_type         pdp_addr;
  ip_addr_type            pcscf_addr[DS_3GPP_ATIF_MAX_PCSCF_SERVERS]; //align with pdn mgr macros
  uint32                  count = DS_3GPP_ATIF_MAX_PCSCF_SERVERS;
  uint32                  v6_count = DS_3GPP_ATIF_MAX_PCSCF_SERVERS;
  ds_pdn_mgr_s*           pdn_mgr_p = NULL;
  ds_fwk_s_type*          fw_inst = NULL;
  ds_bearer_mgr_s*        bearer_mgr_p = NULL;
  ds_local_err_val_e_type err_val = DS_LOCAL_ERR_VAL_NONE;
  ds_profile_type_s       profile_s = {{0}};
  ds_pdn_mgr_apn_table_s *apn_table_p = NULL;
  ds_3gpp_rmsm_at_instance_type instance = DS_3GPP_RMSM_AT_UM_MAX_INSTANCES;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(dynamic_param_ptr == NULL)
  {
    err_val = DS_LOCAL_ERR_VAL_0;
    goto func_exit;
  }

  pdn_mgr_p = ds_pdn_mgr_get_pdn_from_profile_id(profile_number);

  if(ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p) == FALSE)
  {    
   /*----------------------------------------------------------------------
     If PDN oorresponding to profile is not found, then check if APN table 
     has entry for this profile APN as resolved APN for some other PDN.
     Also, check this only if profile is already active with CGACT.
   ----------------------------------------------------------------------*/

    instance = ds_3gpp_rmsm_ati_get_inst_num_from_profile(profile_number);
    if(DS_3GPP_RMSM_AT_UM_MAX_INSTANCES == instance)
    {
      DS_MSG0(MSG_LEGACY_ERROR, "Profile not active !");
      return FALSE;
    }

    if((DS_PROFILE_RESULT_ERR_INVAL != 
              ds_profile_setting_from_profile_id(profile_number,&profile_s)) &&
       (ds_apn_table_get_apn_table_ptr((char*)profile_s.context.apn, 
                                        &apn_table_p) != -1))
    {
      if(apn_table_p != NULL)
      {
        if(apn_table_p->v4_pdn_ptr != NULL)
        {
          pdn_mgr_p = apn_table_p->v4_pdn_ptr;
        }
        else if(apn_table_p->v6_pdn_ptr != NULL)
        {
          pdn_mgr_p = apn_table_p->v6_pdn_ptr;
        }
        else
        {
          pdn_mgr_p = apn_table_p->non_ip_pdn_ptr;
        }
      }

      if(ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p) == FALSE)
      {
        err_val = DS_LOCAL_ERR_VAL_1;
        goto func_exit;
      }
    }
    else
    {
      err_val = DS_LOCAL_ERR_VAL_2;	
      goto func_exit;
    }
  }
  
  fw_inst = ds_fwk_get_inst_ptr_from_index(pdn_mgr_p->fw_index);
  if(fw_inst ==  NULL)
  {
    err_val = DS_LOCAL_ERR_VAL_3;
    goto func_exit;
  }
  
  /* Query MH to check whether the profile is active */
  if( DS_PDN_MGR_STATE_DOWN == pdn_mgr_p->state)
  {
    err_val = DS_LOCAL_ERR_VAL_4;
    goto func_exit;
  }

  DS_MSG0(MSG_LEGACY_HIGH,"Returning DS_3GPP_ATIF_PRIM_INFO");
  prim_info_ptr = &(dynamic_param_ptr->u.prim_dynamic_info);
  dynamic_param_ptr->info_type = DS_3GPP_ATIF_PRIM_INFO;

  if(pdn_mgr_p->pdp_profile->context.apn[0] != '\0')
  {
    prim_info_ptr->apn = pdn_mgr_p->pdp_profile->context.apn; 
  }
  else
  {
    ds_apn_table_get_resolved_apn_name(pdn_mgr_p,(char**)&prim_info_ptr->apn);
  }

  /* Populate bearer ID, APN */
  bearer_mgr_p = ds_pdn_mgr_get_default_bearer(pdn_mgr_p);
  if(bearer_mgr_p == NULL)
  {
    err_val = DS_LOCAL_ERR_VAL_5;
    goto func_exit;
  }

  prim_info_ptr->bearer_id = bearer_mgr_p->bearer_id;
  prim_dns_addr.type = IP_ADDR_INVALID;
  sec_dns_addr.type  = IP_ADDR_INVALID;

  /* Get V4 DNS , gateway address, Primary and secondary CSCF addr */
  if((pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV4] != NULL) && 
     (pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV4]->state == DS_INSTANCE_STATE_UP))
  {
    /* get PDP address */
    ds_fwk_get_ip_addr(fw_inst,IPV4_ADDR,&pdp_addr);
    prim_info_ptr->ip_addr.ip_vsn = DS_PROFILE_IP_V4;
    prim_info_ptr->ip_addr.pdp_addr.pdp_addr_ipv4 = pdp_addr.addr.v4.ps_s_addr;
    
    ds_fwk_get_v4_dns_addrs(fw_inst,&prim_dns_addr,
                              &sec_dns_addr);
    if ( IP_ADDR_INVALID != prim_dns_addr.type )
      prim_info_ptr->v4_ipcp.ipcp4_options.primary_dns = prim_dns_addr.addr.v4;
    
    if ( IP_ADDR_INVALID != sec_dns_addr.type )
      prim_info_ptr->v4_ipcp.ipcp4_options.secondary_dns = sec_dns_addr.addr.v4;

    /* Gateway address */
    gateway_addr.type = IPV4_ADDR;
    ds_fwk_get_peer_addr(pdn_mgr_p,&gateway_addr,DS_PDN_INSTANCE_IPV4,fw_inst);
    if (gateway_addr.type == IPV4_ADDR)
      prim_info_ptr->v4_ipcp.ipcp4_options.gateway_addr = gateway_addr.addr.v4.ps_s_addr;

    memset(&pcscf_addr[0],0x0,sizeof(pcscf_addr));
    /* CSCF address */
    ds_fwk_get_sip_serv_addr_ex(fw_inst, &pcscf_addr[0],
                               &count,DS_PDN_INSTANCE_IPV4 );
    if(pcscf_addr[0].type == IPV4_ADDR)
    {
      /* copy the first into primary and second to Scscf */
      prim_info_ptr->prim_pcscf_addr.ip_vsn = DS_PROFILE_IP_V4;  
      prim_info_ptr->prim_pcscf_addr.pdp_addr.pdp_addr_ipv4 = pcscf_addr[0].addr.v4;
    }
    if(pcscf_addr[1].type == IPV4_ADDR)
    {
      prim_info_ptr->sec_pcscf_addr.ip_vsn = DS_PROFILE_IP_V4;
      prim_info_ptr->sec_pcscf_addr.pdp_addr.pdp_addr_ipv4 = pcscf_addr[1].addr.v4;
    }
  }

  /* Get V6 DNS address */
  prim_dns_addr.type = IP_ADDR_INVALID;
  sec_dns_addr.type  = IP_ADDR_INVALID;

  if((pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV6] != NULL) && 
     (pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV6]->state == DS_INSTANCE_STATE_UP))
  {
    ds_fwk_get_ip_addr(fw_inst,IPV6_ADDR,&pdp_addr);
    prim_info_ptr->ip_addr.ip_vsn = (ds_profile_ip_version_enum_type)
                               ((int)prim_info_ptr->ip_addr.ip_vsn + (int)DS_PROFILE_IP_V6);
    prim_info_ptr->ip_addr.pdp_addr.pdp_addr_ipv6.in6_u.u6_addr64[0] = pdp_addr.addr.v6.in6_u.u6_addr64[0];
    prim_info_ptr->ip_addr.pdp_addr.pdp_addr_ipv6.in6_u.u6_addr64[1] = pdp_addr.addr.v6.in6_u.u6_addr64[1];
    
    ds_fwk_get_v6_dns_addrs(fw_inst,&prim_dns_addr,
                              &sec_dns_addr);
    if ( IP_ADDR_INVALID != prim_dns_addr.type )
    {
      prim_info_ptr->v6_ipcp.ipv6cp_options.primary_dns.in6_u.u6_addr64[0] = 
             prim_dns_addr.addr.v6[0];
      prim_info_ptr->v6_ipcp.ipv6cp_options.primary_dns.in6_u.u6_addr64[1] = 
             prim_dns_addr.addr.v6[1];
    }
    if ( IP_ADDR_INVALID != sec_dns_addr.type )
    {
      prim_info_ptr->v6_ipcp.ipv6cp_options.secondary_dns.in6_u.u6_addr64[0] =
               sec_dns_addr.addr.v6[0];
      prim_info_ptr->v6_ipcp.ipv6cp_options.secondary_dns.in6_u.u6_addr64[1] =
               sec_dns_addr.addr.v6[1];
    }

    gateway_addr.type = IPV6_ADDR;
    ds_fwk_get_peer_addr(pdn_mgr_p,&gateway_addr,DS_PDN_INSTANCE_IPV6,fw_inst);
    if (gateway_addr.type == IPV6_ADDR)
    {
      prim_info_ptr->v6_ipcp.ipv6cp_options.gateway_addr.in6_u.u6_addr64[0] =
             gateway_addr.addr.v6.in6_u.u6_addr64[0];
      prim_info_ptr->v6_ipcp.ipv6cp_options.gateway_addr.in6_u.u6_addr64[1] =
             gateway_addr.addr.v6.in6_u.u6_addr64[1];
    }
    else
    {
      DS_MSG0(MSG_LEGACY_ERROR,"No valid V6 Gateway address");
    }

    memset(&pcscf_addr[0],0x0,sizeof(pcscf_addr));
    /* V6 CSCF address */
    ds_fwk_get_sip_serv_addr_ex(fw_inst, &pcscf_addr[0],
                               &v6_count,DS_PDN_INSTANCE_IPV6);
    DS_MSG0(MSG_LEGACY_HIGH,"Trying to fetch IPV6 PCSCF addr from ATCmds");

    if(pcscf_addr[0].type == IPV6_ADDR)
    {
      /* copy the first into primary and second to Scscf */
      prim_info_ptr->prim_pcscf_addr.ip_vsn = (ds_profile_ip_version_enum_type)
           ((int)prim_info_ptr->prim_pcscf_addr.ip_vsn + (int)DS_PROFILE_IP_V6);
      prim_info_ptr->prim_pcscf_addr.pdp_addr.pdp_addr_ipv6.in6_u.u6_addr64[0] = 
              pcscf_addr[0].addr.v6[0];
      prim_info_ptr->prim_pcscf_addr.pdp_addr.pdp_addr_ipv6.in6_u.u6_addr64[1] = 
              pcscf_addr[0].addr.v6[1];
    }
    if(pcscf_addr[1].type == IPV6_ADDR)
    {
      prim_info_ptr->sec_pcscf_addr.ip_vsn = (ds_profile_ip_version_enum_type)
           ((int)prim_info_ptr->sec_pcscf_addr.ip_vsn + (int)DS_PROFILE_IP_V6);
      prim_info_ptr->sec_pcscf_addr.pdp_addr.pdp_addr_ipv6.in6_u.u6_addr64[0] =
                            pcscf_addr[1].addr.v6[0];
      prim_info_ptr->sec_pcscf_addr.pdp_addr.pdp_addr_ipv6.in6_u.u6_addr64[1] =
                            pcscf_addr[1].addr.v6[1];
    }
  }
  return TRUE;

func_exit:
  if(err_val != DS_LOCAL_ERR_VAL_NONE)
    DS_MSG1(MSG_LEGACY_ERROR,"ds_3gpp_rmsm_ati_read_prim_dynamic_params"
                              "err_val:%d",err_val);
  return FALSE;
}/* ds_3gpp_rmsm_ati_read_prim_dynamic_params */


/*===========================================================================
  FUNCTION   DS_3GPP_RMSM_ATI_GET_PROFILELIST_FROM_PROF_MASK
  
  DESCRIPTION
    This function returns list of profiles from input profile mask

  DEPENDENCIES
    None.
  
  RETURN VALUE
    Count of the profiles present in the profile mask
  
  SIDE EFFECTS
    None
===========================================================================*/
uint8 ds_3gpp_rmsm_ati_get_profilelist_from_prof_mask
(
  uint16 prof_mask,
  uint8* prof_num_list
)
{
  uint8 index = 0;
  uint8 profile_num = 1;
  
  while(prof_mask && (index < DS_3GPP_MAX_PDN_CONTEXT))
  {
    if(prof_mask & 0x1)
    {
      prof_num_list[index++] = profile_num;
    }
    prof_mask >>= 1;
    profile_num++;
  }
  return index;
}

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_ATI_PROCESS_CSODCP

  DESCRIPTION
    This function is used to transmit data over
    control plane to the network from AT terminal.

  PARAMETERS
    profile_number - profile no
    cpdata_length  - number of octets in the data payload
    data_frame     - data payload
    is_MOX_data    â€“ to indicate MO exception data

  RETURN VALUE
    DSAT_ERROR -- In case of any FAILURE
    DSAT_ASYNC_CMD -- in case of SUCCESS

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
dsat_result_enum_type ds_3gpp_rmsm_ati_process_csodcp
(
  uint32                      profile_number,
  uint32                      cpdata_length ,
  dsm_item_type             **data_frame,
  uint8                       rai_ind,
  boolean                     is_MOX_data
)
{
  ds_3gpp_rmsm_at_instance_type   instance = DS_3GPP_RMSM_AT_UM_MAX_INSTANCES;
  ds_local_err_val_e_type         local_err = DS_LOCAL_ERR_VAL_NONE;
  ds_mo_ex_user_info_s            *mox_user_info = NULL;
  dsat_result_enum_type           result = DSAT_ERROR;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Disallow this function if cp data is not enabled */
  if (ds_pdn_cp_data_is_enabled() == FALSE)
  {
    local_err = DS_LOCAL_ERR_VAL_0;
    goto func_exit;
  }
  DS_MSG1(MSG_LEGACY_HIGH, "IS CP_DATA_ENABLED : %d", ds_pdn_cp_data_is_enabled());
  
  /* Check whether the profile is activated by +CGACT */
  instance = ds_3gpp_rmsm_ati_get_inst_num_from_profile(profile_number);
  if ( DS_3GPP_RMSM_AT_UM_MAX_INSTANCES == instance)
  {
    if(is_MOX_data)
    {
      /* Save the context in user info ptr */
      DS_SYSTEM_HEAP_MEM_ALLOC(mox_user_info, sizeof(ds_mo_ex_user_info_s),
                                ds_mo_ex_user_info_s*);
      if(mox_user_info == NULL)
      {
        local_err = DS_LOCAL_ERR_VAL_4;
        goto func_exit;
      }
      memset(mox_user_info,0,(sizeof(ds_mo_ex_user_info_s)));
      mox_user_info->profile_num  = profile_number;
      mox_user_info->pkt_data_len = cpdata_length;
      mox_user_info->pkt_ptr      = data_frame;
      mox_user_info->rai_ind = (ds_pdn_rai_ind_enum_type)rai_ind;
      mox_user_info->is_mox_data_pending = TRUE;

      result = ds_3gpp_rmsm_ati_cgact_activation(profile_number,(void*)mox_user_info);
      if( result == DSAT_ERROR)
      {
        local_err = DS_LOCAL_ERR_VAL_1;
        goto func_exit;
      }
      else if( result == DSAT_OK)
      {
        DS_MSG0(MSG_LEGACY_HIGH,"DSAT_OK returned for forced PDN activation");
        /* Free the mox user info buffer as its no longer needed */
        DS_SYSTEM_HEAP_MEM_FREE(mox_user_info);
        
        instance = ds_3gpp_rmsm_ati_get_inst_num_from_profile(profile_number);
        /* Still couldnt populate instance,return ERROR.
           Shouldn't happen ideally */
        if(DS_3GPP_RMSM_AT_UM_MAX_INSTANCES == instance)
        {
          local_err = DS_LOCAL_ERR_VAL_4;
          goto func_exit;
        }
      }
      else
      {
        DS_MSG0(MSG_LEGACY_HIGH,"DSAT_ASYNC: Bringing up the PDN to send MO ex data");
        /*Triggered PDN Bringup. Wait for the UP/DOWN event*/
        return DSAT_ASYNC_CMD;
      }
    }
    else
    {
      /* profile num, not activated by AT cmd */
      local_err = DS_LOCAL_ERR_VAL_2;
      goto func_exit;
    }
  }
  
  if(FALSE == ds_3gpp_rmsm_ati_send_user_data(instance,profile_number,
                          cpdata_length,data_frame,rai_ind,is_MOX_data))
  {
    local_err = DS_LOCAL_ERR_VAL_3;
    goto func_exit;
  }
    
  return DSAT_OK;
  
func_exit:
  if(local_err != DS_LOCAL_ERR_VAL_NONE)
  {
    DS_MSG1(MSG_LEGACY_HIGH,"ds_3gpp_rmsm_ati_csodcp_hdlr local_err %d",
                       local_err);
  }
  return DSAT_ERROR;
} /* ds_3gpp_rmsm_ati_process_csodcp */

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_ATI_CRTDCP_HANDLER

  DESCRIPTION
    This function is used to indicate MH that AT is interested in
    listening incoming cp data from network.

  PARAMETERS

  RETURN VALUE
    DSAT_ERROR -- In case of any FAILURE
    DSAT_ASYNC_CMD -- in case of SUCCESS

  DEPENDENCIES
    None

  SIDE EFFECTS
    None

===========================================================================*/
dsat_result_enum_type ds_3gpp_rmsm_ati_crtdcp_handler
(
  boolean                            enable_data_rx,
  downlink_data_cback_type           cb_fptr
)
{
  ds_local_err_val_e_type         local_err = DS_LOCAL_ERR_VAL_NONE;
  dsat_result_enum_type           ret_val = DSAT_OK;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Disallow this function if cp data is not enabled */
  if (ds_pdn_cp_data_is_enabled() == FALSE)
  {
    local_err = DS_LOCAL_ERR_VAL_0;
    ret_val = DSAT_ERROR;
    goto func_exit;
  }

  DS_MSG1(MSG_LEGACY_HIGH, "IS CP_DATA_ENABLED : %d", ds_pdn_cp_data_is_enabled());

  /* mark that AT is interested / not interested in data transfer*/
  if(enable_data_rx &&
     !ds_3gpp_ati_rx_data_info.data_rx_enabled)
  {
    if(cb_fptr == NULL)
    {
      /* cb function can't be null for sending data to AT */
      local_err = DS_LOCAL_ERR_VAL_2;
      ret_val = DSAT_ERROR;
      goto func_exit;
    }
      
    ds_3gpp_rmsm_ati_setup_rx_data_path(TRUE);
    ds_3gpp_ati_rx_data_info.data_rx_enabled = TRUE;
    ds_3gpp_ati_rx_data_info.data_rx_cb_func = cb_fptr;
 
  }
  else if ((!enable_data_rx)  && 
           (ds_3gpp_ati_rx_data_info.data_rx_enabled))
  {
    ds_3gpp_rmsm_ati_setup_rx_data_path(FALSE);
    ds_3gpp_ati_rx_data_info.data_rx_enabled = FALSE;
    ds_3gpp_ati_rx_data_info.data_rx_cb_func = NULL;
    
  }

func_exit:
  if(local_err != DS_LOCAL_ERR_VAL_NONE)
  {
    DS_MSG1(MSG_LEGACY_HIGH,"ds_3gpp_rmsm_ati_crtdcp_hdlr local_err %d",
                       local_err);
  }
  
  return ret_val;
} /* ds_3gpp_rmsm_ati_crtdcp_handler */


/*===========================================================================
  FUNCTION DS_3GPP_RMSM_ATI_RX_DATA_CB

  DESCRIPTION
    This function is used to indicate MH that AT is interested in
    listening incoming non-ip data from network.

  PARAMETERS
  wm_p -> ptr to wm
  call_back_data -> will contain bearer_context_p

  RETURN VALUE

  DEPENDENCIES
    None

  SIDE EFFECTS
    None

===========================================================================*/
void ds_3gpp_rmsm_ati_rx_data_cb
( 
  ds_fwk_clnt_handle_type    client_handle,
  dsm_item_type              *dsm_item,
  void                       *user_data_ptr
)
{
  ds_local_err_val_e_type          local_err = DS_LOCAL_ERR_VAL_NONE;
  uint32                           profile_number = 0;
  ds_3gpp_rmsm_at_instance_type    inst_num;
  ds_3gpp_ati_pdp_state_type *     ati_pdp_ptr = NULL;
  uint8                          profile_num_list[DS_3GPP_MAX_PDN_CONTEXT] = {0};
  uint8                          num_prof_entries = 0;
  ds_fwk_clnt_info_s_type        client_info = {0};
  ds_bearer_mgr_s                *def_bearer_mgr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( FALSE == ds_3gpp_ati_rx_data_info.data_rx_enabled ||
      NULL == ds_3gpp_ati_rx_data_info.data_rx_cb_func)
  {
    /* AT client not interested in Rx, or cb function is NULL
       so drop packet */
    local_err = DS_LOCAL_ERR_VAL_1;
    goto func_exit;
  }
      
  inst_num = ds_3gpp_rmsm_ati_get_inst_num_from_client_handle(client_handle);
  if ( DS_3GPP_RMSM_AT_UM_MAX_INSTANCES <= inst_num)
  {
    local_err = DS_LOCAL_ERR_VAL_2;
    goto func_exit;
  }

  ati_pdp_ptr = ds_3gpp_rmsm_get_info_ptr_from_instance(inst_num);
  if(FALSE == ds_3gpp_rmsm_ati_pdp_state_type_validate(ati_pdp_ptr))
  {
    local_err = DS_LOCAL_ERR_VAL_3;
    goto func_exit;
  }
  
  ds_fwk_get_clnt_info_from_handle(DS_FWK_CLIENT_ATCOP,client_handle,&client_info);
  
  def_bearer_mgr = ds_fwk_get_default_bearer(client_info.fw_inst,client_info.ip_type);
  if(def_bearer_mgr == NULL)
  {
    local_err = DS_LOCAL_ERR_VAL_4;
    goto func_exit;
  }

  if((def_bearer_mgr->eps_bearer != NULL) && def_bearer_mgr->eps_bearer->is_registered_pdcp)
  {
    /*Filter out User plane Data*/
    local_err = DS_LOCAL_ERR_VAL_5;
    goto func_exit;
  }
  else
  {
    DS_MSG0(MSG_LEGACY_HIGH,"Incoming CP data..FWD it to AT");
  }
  
  num_prof_entries = ds_3gpp_rmsm_ati_get_profilelist_from_prof_mask
                      (ati_pdp_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->profile_mask,
                      profile_num_list);
  if(num_prof_entries > 1)
  {
    DS_MSG0(MSG_LEGACY_HIGH,"Multiple profiles present for the given inst_num..Pick first one");
    profile_number = profile_num_list[0];
  }
  else if(num_prof_entries == 1)
  {
    profile_number = profile_num_list[0];
  }
  else
  {
    local_err = DS_LOCAL_ERR_VAL_6;
    goto func_exit;
  }
  DS_MSG1(MSG_LEGACY_HIGH,"Profile num on which data recvd %d ",profile_number);
  
  ds_3gpp_ati_rx_data_info.data_rx_cb_func( profile_number,
                                                &dsm_item);
  func_exit:
    if(local_err != DS_LOCAL_ERR_VAL_NONE)
    {
      dsm_free_packet(&dsm_item);
    }
    DS_MSG1(MSG_LEGACY_HIGH,"ds_3gpp_rmsm_ati_rx_data_cb local_err %d",
                       local_err);
    return;
}/* ds_3gpp_rmsm_ati_rx_data_cb */

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_ATI_SEND_USER_DATA

  DESCRIPTION

  PARAMETERS

  RETURN VALUE

  DEPENDENCIES
    None

  SIDE EFFECTS
    None

===========================================================================*/
boolean ds_3gpp_rmsm_ati_send_user_data
(
  ds_3gpp_rmsm_at_instance_type instance,
  uint32                        profile_num,
  uint32                        cpdata_length,
  dsm_item_type                 **data_frame,
  uint8                         rai_ind,
  boolean                       is_mox_data
)
{
  ds_local_err_val_e_type        local_err = DS_LOCAL_ERR_VAL_NONE;
  ps_tx_meta_info_type           *tx_meta_info_ptr = NULL;
  ds_rmsm_ati_inst_type_e        inst_type = DS_RMSM_ATI_MIN_INST;
  ds_3gpp_ati_pdp_state_type     *ati_pdp_ptr = NULL;
  ds_profile_status_etype         res = DS_PROFILE_RESULT_SUCCESS;  
  ds_profile_type_s               profile_info = {{0}};  
  ds_profile_pdp_type_e           pdp_type = DS_PROFILE_PDP_MAX;  
  ds_fwk_s_type                  *fwk_inst = NULL;
  ds_pdn_mgr_s                    *pdn_mgr_p = NULL;
  ds_bearer_mgr_s                 *def_bearer = NULL;
  ip_addr_enum_type               input_ip_ver = IP_ADDR_INVALID;
  /*-----------------------------------------------------*/

  ati_pdp_ptr = ds_3gpp_rmsm_get_info_ptr_from_instance(instance);
  if(!ds_3gpp_rmsm_ati_pdp_state_type_validate(ati_pdp_ptr))
  {
    local_err = DS_LOCAL_ERR_VAL_0;
    goto func_exit;
  }
  DS_MSG3(MSG_LEGACY_HIGH,"Data Frame %x  data frame content %x cp_data len %d",
                       data_frame,(data_frame?(*data_frame):NULL),cpdata_length);
  
  res = ds_profile_setting_from_profile_id((int8)profile_num,&profile_info);
  if(res != DS_PROFILE_RESULT_SUCCESS)
  {
    local_err = DS_LOCAL_ERR_VAL_1;
    goto func_exit;
  }
  
  pdp_type = profile_info.context.pdp_type;  
  /*Allocate the TX meta info ptr*/
  PS_TX_META_INFO_GET_ALL(tx_meta_info_ptr);
  if((tx_meta_info_ptr == NULL) || (tx_meta_info_ptr->rt_meta_info_ptr == NULL))
  {
    local_err = DS_LOCAL_ERR_VAL_2;
    goto func_exit;
  }
  
  if(pdp_type == DS_PROFILE_PDP_IPV4)
  {
    tx_meta_info_ptr->rt_meta_info_ptr->pkt_info.ip_vsn = IP_V4;
    inst_type = DS_RMSM_ATI_V4_INST;
    input_ip_ver = IPV4_ADDR;
  }
  else if(pdp_type == DS_PROFILE_PDP_IPV6)
  {
    tx_meta_info_ptr->rt_meta_info_ptr->pkt_info.ip_vsn = IP_V6;
    inst_type = DS_RMSM_ATI_V6_INST;
    input_ip_ver = IPV6_ADDR;
  }
  else if(pdp_type == DS_PROFILE_PDP_IPV4V6)
  {
    if(ds_3gpp_rmsm_get_v4_inst_ptr(instance) != NULL)
    {
      inst_type = DS_RMSM_ATI_V4_INST;
      tx_meta_info_ptr->rt_meta_info_ptr->pkt_info.ip_vsn = IP_V4;
      input_ip_ver = IPV4_ADDR;
    }
    else if(ds_3gpp_rmsm_get_v6_inst_ptr(instance) != NULL)
    {
      inst_type = DS_RMSM_ATI_V6_INST;
      tx_meta_info_ptr->rt_meta_info_ptr->pkt_info.ip_vsn = IP_V6;
      input_ip_ver = IPV6_ADDR;
    }
  }
  else if(pdp_type == DS_PROFILE_PDP_NON_IP)
  {
    tx_meta_info_ptr->rt_meta_info_ptr->pkt_info.ip_vsn = NON_IP;
    inst_type = DS_RMSM_ATI_NONIP_INST;
    input_ip_ver = NON_IP_ADDR;
  }

  if((inst_type < DS_RMSM_ATI_MAX_INST) && 
     (ati_pdp_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->um_inst_ptr[inst_type] != NULL))
  {
    fwk_inst = ati_pdp_ptr->ds_3gpp_ati_pdp_state_type_dyn_p->um_inst_ptr[inst_type]->fw_inst;
    if(fwk_inst == NULL)
    {
      local_err = DS_LOCAL_ERR_VAL_3;
      goto func_exit;
    }
  }
  
  pdn_mgr_p = ds_fwk_get_pdn_ptr_from_fwk_inst(fwk_inst,input_ip_ver);
  if(pdn_mgr_p == NULL)
  {
    local_err = DS_LOCAL_ERR_VAL_4;
    goto func_exit;
  }
  
  def_bearer = pdn_mgr_p->bearer_ptr_tbl[DS_3GPP_DEFAULT_BEARER_INDEX];
  if(!ds_bearer_mgr_validate_bearer_ptr(def_bearer))
  {
    local_err = DS_LOCAL_ERR_VAL_5;
    goto func_exit;
  }
  
  /* send indication for RAI  */
  ds_pdn_cp_data_set_release_assistance_ind(def_bearer->index,
                                            (ds_pdn_rai_ind_enum_type)rai_ind);
  
  /*If empty DSM item is passed, just update the RAI param and dont send data
    Return OK in this case since the intention is to update the RAI alone */
  if((data_frame == NULL) || (*data_frame == NULL) || (cpdata_length == 0))
  {
    DS_MSG0(MSG_LEGACY_HIGH,"Empty data passed. Return TRUE");
    PS_TX_META_INFO_FREE(&tx_meta_info_ptr);
    return TRUE;
  }

  /*-----------------------------------------------------------------------
    Step 6 Update meta info with routing cache and QoS filter result
  -----------------------------------------------------------------------*/
  PS_TX_META_SET_ROUTING_CACHE(tx_meta_info_ptr, fwk_inst);
  PS_TX_META_SET_FILTER_RESULT(tx_meta_info_ptr,
                               FLTR_CLIENT_QOS_OUTPUT,
                               (uint32) NULL);
  /*---------------------------------------------------------------------
     Step 6 Set the filter result if data is MO exceptiona data.
  ----------------------------------------------------------------------*/
  if(is_mox_data)
  {
    DS_MSG0(MSG_LEGACY_HIGH,"Process csodcp MO Exceptional flag is set");
    PS_TX_META_SET_FILTER_RESULT( tx_meta_info_ptr, FLTR_CLIENT_MO_EXCP, TRUE);
  }
  else
  {
    PS_TX_META_SET_FILTER_RESULT( tx_meta_info_ptr, FLTR_CLIENT_MO_EXCP, FALSE);
  }

  /* Send data */
  if( -1 == ds_fwk_default_tx_cmd(fwk_inst, data_frame, tx_meta_info_ptr))
  {
    local_err = DS_LOCAL_ERR_VAL_6;
    goto func_exit;
  }

  return TRUE;
  
func_exit:
  if(local_err != DS_LOCAL_ERR_VAL_NONE)
  {
    DS_MSG1(MSG_LEGACY_ERROR,"ds_3gpp_rmsm_ati_send_user_data local_err %d",
                       local_err);
    PS_TX_META_INFO_FREE(&tx_meta_info_ptr);
  }
  return FALSE;
}

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_ATI_SETUP_RX_DATA_PATH

  DESCRIPTION
    When AT is interested in Rx data, dereg DPM and setup data path to AT.
    When AT is not interested in Rx data, register to DPM.

  PARAMETERS
    enable_AT_data_rx -> indicates if AT is interested in Rx data

  RETURN VALUE

  DEPENDENCIES
    None

  SIDE EFFECTS
    None

===========================================================================*/
void ds_3gpp_rmsm_ati_setup_rx_data_path
(
  boolean                         enable_AT_data_rx
)
{
  
  if(enable_AT_data_rx)
  {
    ds_fwk_reg_tx_cb(DS_FWK_CLIENT_ATCOP, &ds_3gpp_rmsm_ati_rx_data_cb ,NULL);
    ds_fwk_reg_nipd_tx_cb(DS_FWK_CLIENT_ATCOP, &ds_3gpp_rmsm_ati_rx_data_cb ,NULL);
  }
  return;

}/* ds_3gpp_rmsm_ati_setup_rx_data_path */

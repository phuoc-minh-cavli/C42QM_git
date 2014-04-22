/*===========================================================================
                       D S U M T S _ R M S M _ I P . C

DESCRIPTION
  The Data Services UMTS RM State Machine for pdp-ip calls source file. 
  This state machine manages the Um and Rm interfaces for pdp-ip type 
  pkt data calls initiated from laptop or connected terminals .
  
  
EXTERNALIZED FUNCTIONS

Copyright (c) 2002-2019 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/dataiot.mpss/2.0/3gpp/rmsm/src/ds_rmsm_ip.c#5 $ 
  $DateTime: 2021/08/26 02:47:57 $ $Author: pwbldsvc $


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "memory.h"
#include "err.h"
#include "msg.h"

#include "dstask.h"
#include "ds3gsiolib.h"
#include "dstaski.h"
#include "ps_in.h"
#include "ps_ppp.h"
#include "ps_ppp_snoop.h"
#include "ps_ppp_fsm.h"
#include "ps_ppp_ipcp.h"
#include "ps_meta_info.h"
#include "ps_byte.h"

#ifdef FEATURE_GSM_DTM
#include "rr_ds_if.h"
#endif /* FEATURE_GSM_DTM */

#include "ds_rmsmi.h"
#include "ds_rmsm_ip.h"
#include "ps_ip_netconfig.h"
#include "ds_3gpp_rmsm_autoconfig_sm.h"
#include "ds_3gpp_rmsm_ip_common.h"
#include "ds_fwki.h"



/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                            INTERNAL DEFINITIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/


/*===========================================================================
                            Forward Declarations
===========================================================================*/

/*---------------------------------------------------------------------------
  DS_RMSM IP state management function
---------------------------------------------------------------------------*/

void ds_rmsm_ip_transition_state
(
  ds_rmsmi_sm_info_type    *sm_ptr,
  ds_rmsm_ipi_state_type   new_state
);

/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                        EXTERNAL FUNCTION DEFINTIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/
/*===========================================================================
  FUNCTION DS_RMSM_IP_INIT ()

  DESCRIPTION
    This function initializes the RmSm IP State Machine.

  PARAMETERS
    sm_ptr: Pointer to the RmSm IP State Machine

  RETURN VALUE
    TRUE,  if success
    FALSE, if failure

  DEPENDENCIES
    none  

  SIDE EFFECTS
    None
===========================================================================*/
boolean ds_rmsm_ip_init
(
  ds_rmsmi_sm_info_type* sm_ptr
)
{
  if (sm_ptr == NULL) 
  {
    DS_ASSERT(0);
    return FALSE;
  }
  
  sm_ptr->pdp_specific_info.pdp_ip.state = DS_RMSM_IP_NULL_STATE;
  
  return ds_3gpp_rmsm_autoconfig_sm_init(sm_ptr);
} /*ds_rmsm_ip_init*/

/*===========================================================================
  FUNCTION DS_RMSM_IP_PROCESS_CMD()

  DESCRIPTION
    This function receives a cmd posted to DS_RMSM (for pdp-ip calls)
    in DS task and calls an appropriate function to handle it.

  PARAMETERS
    inst_ptr: Pointer to the Rm Dev Instance and SM Instance
    cmd id:   Command to be processed

  RETURN VALUE
    none

  DEPENDENCIES
    none  
    
  SIDE EFFECTS
    None
===========================================================================*/
void ds_rmsm_ip_process_cmd
(
  ds_rmsm_instance_type* inst_ptr,
  ds_command_enum_type   cmd
)
{
  ds_rmsmi_sm_info_type*    sm_ptr = NULL;
  ds_local_err_val_e_type   err_val = DS_LOCAL_ERR_VAL_NONE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (inst_ptr == NULL)
  {
    err_val = DS_LOCAL_ERR_VAL_0;
    goto func_exit;
  }
  
  sm_ptr = ds_rmsmi_get_sm_info_ptr(inst_ptr);

  if (sm_ptr == NULL)
  {
    err_val = DS_LOCAL_ERR_VAL_1;
    goto func_exit;
  }

  switch(cmd)
  { 
    case DS_CMD_3GPP_RMSM_ADDR_CFG_COMPLETE:
      ds_rmsm_ip_process_addr_cfg_complete_ev(sm_ptr);
      break;

    case DS_CMD_3GPP_RMSM_ADDR_CFG_END:
      ds_rmsm_ip_process_addr_cfg_end_ev(sm_ptr);
      break;

    case DS_CMD_3GPP_RMSM_ADDR_CFG_FAIL:
      ds_rmsm_ip_process_addr_cfg_failure_ev(sm_ptr);
      break;

    case DS_CMD_3GPP_RMSM_ADDR_CFG_CHANGED:
      ds_rmsm_ip_process_addr_cfg_changed_ev(sm_ptr);
      break;      

    default:
      DS_MSG3(MSG_LEGACY_ERROR,"Invalid UMTS RMSM sm instance: %d, "
              "rm dev instance: %d, cmd %d, ignoring", 
              inst_ptr->sm_instance, inst_ptr->rm_dev_instance,cmd);
  }

func_exit:
  if(err_val != DS_LOCAL_ERR_VAL_NONE)
    DS_MSG1(MSG_LEGACY_ERROR,"ds_rmsm_ip_process_cmd err_val:%d",err_val);
  return;
} /*ds_rmsm_ip_process_cmd*/

/*===========================================================================
  FUNCTION DS_RMSM_IP_PROCESS_BRING_UP_UM_IFACE_EV

  DESCRIPTION
    This function processes UMTS_RMSM_RM_BRING_UP_UM_IFACE_EV event. 

  PARAMETERS
    sm_ptr: Pointer to the RmSm IP State Machine

  RETURN VALUE
    None

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
void ds_rmsm_ip_process_bring_up_pdn_ev
(
  ds_rmsmi_sm_info_type *sm_ptr
)
{

  ds_rmsmi_info_type*       rmsmi_info_ptr = NULL;
  ds_local_err_val_e_type   err_val = DS_LOCAL_ERR_VAL_NONE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (sm_ptr == NULL) 
  {
    err_val = DS_LOCAL_ERR_VAL_0;
    goto func_exit;
  }

  rmsmi_info_ptr = (ds_rmsmi_info_type*)sm_ptr->vobj_data_ptr;
  if (rmsmi_info_ptr == NULL) 
  {
    err_val = DS_LOCAL_ERR_VAL_1;
    goto func_exit;
  }
  
  DS_MSG2(MSG_LEGACY_HIGH, "Recd RM_BRING_UP_UM_INST_EV, sm instance:%d state:%d", 
           sm_ptr->instance,sm_ptr->pdp_specific_info.pdp_ip.state);

  switch( sm_ptr->pdp_specific_info.pdp_ip.state )
  {
    case DS_RMSM_IP_NULL_STATE:
    /*-----------------------------------------------------------------
      Transition the state to waiting for Um up state which would 
      bring up the um packet call
     -----------------------------------------------------------------*/
      ds_rmsm_ip_transition_state( sm_ptr, DS_RMSM_IP_WAITING_FOR_UM_UP_STATE );
    break;

    case DS_RMSM_IP_WAITING_FOR_UM_UP_STATE:
    /*---------------------------------------------------------------------
      We have requested Um iface be bought up already. The new packet would
      have been stored in the callback & old one freed. Just ignore this
      request as we have already kick started bring up the Um interface
      ..fall through
    ---------------------------------------------------------------------*/      
      DS_MSG0(MSG_LEGACY_HIGH,"IPCP confg req ignored. waiting for Um to be up");
      /* Fall Through */

    case DS_RMSM_IP_WAITING_FOR_AUTOCONFIG_COMPLETE_STATE:
    case DS_RMSM_IP_WAITING_FOR_RM_PPP_UP_STATE:
    case DS_RMSM_IP_UM_RM_UP_STATE:
    case DS_RMSM_IP_WAITING_FOR_UM_DOWN_STATE:
      /*---------------------------------------------------------------------
        Ignore event in all other states.
      ---------------------------------------------------------------------*/
      DS_MSG2(MSG_LEGACY_HIGH, "Ignoring RM_BRING_UP_UM_IFACE_EV Sm instance :%d state:%d",
                sm_ptr->instance,sm_ptr->pdp_specific_info.pdp_ip.state);
      break;

    default:
      /*---------------------------------------------------------------------
        Invalid state
      ---------------------------------------------------------------------*/
      DS_MSG1(MSG_LEGACY_ERROR, "Invalid RMSM state %d",
                                sm_ptr->pdp_specific_info.pdp_ip.state);    
      break;

  } /* switch( sm_ptr->pdp_specific_info.pdp_ip.state ) */
  
func_exit:
    if(err_val != DS_LOCAL_ERR_VAL_NONE)
      DS_MSG1(MSG_LEGACY_ERROR,"ds_rmsm_ip_process_bring_up_pdn_ev err_val:%d",err_val);
    return;

} /*ds_rmsm_ip_process_bring_up_um_iface_ev() */

/*===========================================================================
  FUNCTION DS_RMSM_IP_PROCESS_ADDR_CFG_COMPLETE_EV()

  DESCRIPTION
    This function processes ADDR_CFG_COMPLETE_EV event. 
    
  PARAMETERS
    sm_ptr: Pointer to the RmSm IP State Machine

  RETURN VALUE
    None

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
void ds_rmsm_ip_process_addr_cfg_complete_ev
(
  ds_rmsmi_sm_info_type *sm_ptr
)
{
  ds_local_err_val_e_type   err_val = DS_LOCAL_ERR_VAL_NONE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (sm_ptr == NULL) 
  {
    err_val = DS_LOCAL_ERR_VAL_0;
    goto func_exit;
  }

  DS_MSG2(MSG_LEGACY_HIGH, "Recd ADDR_CFG_COMPLETE_EV, sm instance:%d state:%d", 
           sm_ptr->instance,sm_ptr->pdp_specific_info.pdp_ip.state);

  switch( sm_ptr->pdp_specific_info.pdp_ip.state )
  {
    case DS_RMSM_IP_WAITING_FOR_AUTOCONFIG_COMPLETE_STATE:
    case DS_RMSM_IP_RM_RECONFIGURING_STATE:
    case DS_RMSM_IP_WAITING_FOR_RM_PPP_UP_STATE:
      ds_rmsm_ip_transition_state( sm_ptr, DS_RMSM_IP_UM_RM_UP_STATE);
      break;

    case DS_RMSM_IP_WAITING_FOR_UM_UP_STATE:
    case DS_RMSM_IP_UM_RM_UP_STATE:
    case DS_RMSM_IP_NULL_STATE:
    case DS_RMSM_IP_WAITING_FOR_UM_DOWN_STATE:
      /*---------------------------------------------------------------------
        Ignore event in all other states.
      ---------------------------------------------------------------------*/
      DS_MSG2(MSG_LEGACY_HIGH, "Ignoring RM_IFACE_UP_EV Sm Instance :%d state:%d",
                sm_ptr->instance,sm_ptr->pdp_specific_info.pdp_ip.state);
      break;

    default:
      /*---------------------------------------------------------------------
        Invalid state
      ---------------------------------------------------------------------*/
      DS_MSG1(MSG_LEGACY_ERROR, "Invalid RMSM state %d",
                                 sm_ptr->pdp_specific_info.pdp_ip.state);
      break;
  } /* switch( sm_ptr->pdp_specific_info.pdp_ip.state ) */
func_exit:
  if(err_val != DS_LOCAL_ERR_VAL_NONE)
    DS_MSG1(MSG_LEGACY_ERROR,"ds_rmsm_ip_process_addr_cfg_complete_ev err_val:%d",err_val);
  return;
} /* ds_rmsm_ip_process_addr_cfg_complete_ev */

/*===========================================================================
  FUNCTION DS_RMSM_IP_PROCESS_ADDR_CFG_END_EV()

  DESCRIPTION
    This function processes ADDR_CFG_END_EV event. 
    
  PARAMETERS
    sm_ptr: Pointer to the RmSm IP State Machine

  RETURN VALUE
    None

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
void ds_rmsm_ip_process_addr_cfg_end_ev
(
  ds_rmsmi_sm_info_type *sm_ptr
)
{
  ds_local_err_val_e_type   err_val = DS_LOCAL_ERR_VAL_NONE;
/*-------------------------------------------------------------------------*/
  
  if (sm_ptr == NULL) 
  {
    err_val = DS_LOCAL_ERR_VAL_0;
    goto func_exit;
  }

  DS_MSG2(MSG_LEGACY_HIGH, "Recd ADDR_CFG_END_EV, sm instance:%d state:%d", 
           sm_ptr->instance,sm_ptr->pdp_specific_info.pdp_ip.state);

  switch( sm_ptr->pdp_specific_info.pdp_ip.state )
  {
    case DS_RMSM_IP_WAITING_FOR_AUTOCONFIG_COMPLETE_STATE:
    case DS_RMSM_IP_UM_UP_WAITING_FOR_AUTOCONFIG_END_STATE:
    /*---------------------------------------------------------------------
      Um interface should be shut down. Clean UP UM instance
    ---------------------------------------------------------------------*/      
      ds_rmsm_ip_transition_state( sm_ptr, 
                                DS_RMSM_IP_WAITING_FOR_UM_DOWN_STATE );
      break;

    case DS_RMSM_IP_UM_DOWN_WAITING_FOR_AUTOCONFIG_END_STATE:
      ds_rmsm_ip_transition_state(sm_ptr, DS_RMSM_IP_NULL_STATE);
      break;

    case DS_RMSM_IP_NULL_STATE:
    case DS_RMSM_IP_WAITING_FOR_UM_UP_STATE:
    case DS_RMSM_IP_WAITING_FOR_UM_DOWN_STATE:
    case DS_RMSM_IP_UM_RM_UP_STATE:
      /*---------------------------------------------------------------------
        Ignore event in all other states.
      ---------------------------------------------------------------------*/
      DS_MSG2(MSG_LEGACY_HIGH, "Ignoring ADDR_CFG_END_EV Sm Instance :%d state:%d",
                sm_ptr->instance,sm_ptr->pdp_specific_info.pdp_ip.state);
      break;

    default:
      /*---------------------------------------------------------------------
        Invalid state
      ---------------------------------------------------------------------*/
      DS_MSG1(MSG_LEGACY_ERROR, "Invalid RMSM state %d",
           sm_ptr->pdp_specific_info.pdp_ip.state);
      break;

  } /* switch( sm_ptr->pdp_specific_info.pdp_ip.state ) */
func_exit:
  if(err_val != DS_LOCAL_ERR_VAL_NONE)
    DS_MSG1(MSG_LEGACY_ERROR,"ds_rmsm_ip_process_addr_cfg_end_ev err_val:%d",err_val);
  return;

} /* ds_rmsm_ip_process_addr_cfg_end_ev */


/*===========================================================================
  FUNCTION DS_RMSM_IP_PROCESS_PDN_DOWN_EV()

  DESCRIPTION
    This function processes UM_PDN_DOWN_EV event. 
    
  PARAMETERS
    sm_ptr: Pointer to the RmSm IP State Machine

  RETURN VALUE
    None

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
void ds_rmsm_ip_process_pdn_down_ev
(
  ds_rmsmi_sm_info_type *sm_ptr
)
{
  ds_rmsmi_info_type       *rmsmi_info_ptr = NULL;
  ds_rmsm_instance_type    inst = {0};
  ds_local_err_val_e_type  err_val = DS_LOCAL_ERR_VAL_NONE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  if (sm_ptr == NULL) 
  {
    err_val = DS_LOCAL_ERR_VAL_0;
    goto func_exit;
  }

  rmsmi_info_ptr = (ds_rmsmi_info_type*)sm_ptr->vobj_data_ptr;
  if (rmsmi_info_ptr == NULL) 
  {
    err_val = DS_LOCAL_ERR_VAL_1;
    goto func_exit;
  }
        
  inst.rm_dev_instance = rmsmi_info_ptr->instance;
  inst.sm_instance = sm_ptr->instance;

  DS_MSG2(MSG_LEGACY_HIGH, "Recd UM_PDN_INST_DOWN_EV, sm instance: %d "
                           "internal state:%d",sm_ptr->instance,
                            sm_ptr->pdp_specific_info.pdp_ip.state);
  
  switch( sm_ptr->pdp_specific_info.pdp_ip.state )
  {
    case DS_RMSM_IP_WAITING_FOR_UM_DOWN_STATE:
    case DS_RMSM_IP_WAITING_FOR_UM_UP_STATE:   
      ds_rmsm_ip_transition_state( sm_ptr, DS_RMSM_IP_NULL_STATE );        
      break;
      
    case DS_RMSM_IP_UM_RM_UP_STATE:
    case DS_RMSM_IP_RM_RECONFIGURING_STATE:
    case DS_RMSM_IP_WAITING_FOR_AUTOCONFIG_COMPLETE_STATE:
    case DS_RMSM_IP_WAITING_FOR_RM_PPP_UP_STATE: 
      if (sm_ptr->instance == DS_RMSM_V6_SM_INSTANCE)
      {
        ds_3gpp_rmsm_autoconfig_sm_post_event(&inst, 
                                             AUTOCONFIG_SM_STOP_ADDR_CFG_EV);
        ds_rmsm_ip_transition_state(sm_ptr,
                     DS_RMSM_IP_UM_DOWN_WAITING_FOR_AUTOCONFIG_END_STATE);
      }
      else if (sm_ptr->instance == DS_RMSM_V4_SM_INSTANCE)
      {
        ds_rmsm_ip_transition_state( sm_ptr, DS_RMSM_IP_NULL_STATE );
      }
      else
      {
        err_val = DS_LOCAL_ERR_VAL_2;
        goto func_exit;
      }
      break;

    case DS_RMSM_IP_NULL_STATE:
      /*---------------------------------------------------------------------
        Ignore event in all other states.
      ---------------------------------------------------------------------*/
      DS_MSG2(MSG_LEGACY_HIGH, "Ignoring UM_PDN_DOWN_EV Sm type :%d state:%d",
                sm_ptr->instance,sm_ptr->pdp_specific_info.pdp_ip.state);
      break;

    default:
      /*---------------------------------------------------------------------
        Invalid state
      ---------------------------------------------------------------------*/
      DS_MSG1(MSG_LEGACY_ERROR, "Invalid RMSM state %d",
                                sm_ptr->pdp_specific_info.pdp_ip.state);
      break;

  } /* switch( sm_ptr->pdp_specific_info.pdp_ip.st */
func_exit:
  if(err_val != DS_LOCAL_ERR_VAL_NONE)
    DS_MSG1(MSG_LEGACY_ERROR,"ds_rmsm_ip_process_pdn_down_ev err_val:%d",err_val);
  return;
} /* ds_rmsm_ip_process_um_iface_down_ev() */

/*===========================================================================
  FUNCTION DS_RMSM_IP_PROCESS_RM_PPP_UP_EV()
 
 DESCRIPTION
    This function processes RM_PPP_UP_EV event. 
 
 PARAMETERS
   	sm_ptr: Pointer to the RmSm IP State Machine
 
 RETURN VALUE
 None
 
 DEPENDENCIES
 none
 
 SIDE EFFECTS
 None
 ===========================================================================*/
void ds_rmsm_ip_process_rm_ppp_up_ev
(
  ds_rmsmi_sm_info_type *sm_ptr
)
{

  ds_rmsmi_info_type* rmsmi_info_ptr;
  ds_local_err_val_e_type   err_val = DS_LOCAL_ERR_VAL_NONE;
  /*------------------------------------------------------------------------*/
  
  DS_MSG0(MSG_LEGACY_HIGH,"Received Rm PPP Up EV");

  if (sm_ptr == NULL) 
  {
    err_val = DS_LOCAL_ERR_VAL_0;
    goto func_exit;
  }
  
  rmsmi_info_ptr = (ds_rmsmi_info_type*)sm_ptr->vobj_data_ptr;
  if (rmsmi_info_ptr == NULL) 
  {
    err_val = DS_LOCAL_ERR_VAL_1;
    goto func_exit;
  }
      
  DS_MSG3(MSG_LEGACY_HIGH, "Processing Rm PPP UP EV, sm instance:%d state:%d", 
           sm_ptr->instance,sm_ptr->pdp_specific_info.pdp_ip.state, 0);

  switch( sm_ptr->pdp_specific_info.pdp_ip.state )
  {
    case DS_RMSM_IP_WAITING_FOR_RM_PPP_UP_STATE:
    {
      if (sm_ptr->instance == DS_RMSM_V6_SM_INSTANCE)
      {
        ds_rmsm_ip_transition_state( sm_ptr,
                DS_RMSM_IP_WAITING_FOR_AUTOCONFIG_COMPLETE_STATE);
      }      
      else if (sm_ptr->instance == DS_RMSM_V4_SM_INSTANCE)
      {
        ds_rmsm_ip_transition_state( sm_ptr,DS_RMSM_IP_UM_RM_UP_STATE );
      }
      else
      {
         DS_MSG1(MSG_LEGACY_ERROR,"Rm PPP UP EV received for wrong PDP Type: %d",
                            DS_RMSMI_GET_PDP_TYPE(rmsmi_info_ptr));
      }

      break;
    }
      
    case DS_RMSM_IP_UM_RM_UP_STATE:
    {
       DS_MSG0(MSG_LEGACY_HIGH,"Ignoring ppp_up_ev:"
                        "Already in DS_RMSM_IP_UM_RM_UP_STATE ");
       break;
    }
      
    default:
     /*---------------------------------------------------------------------
        rmsm_ip sm state for v6 call is not reset after previous successfuly
        call connect and disconnect as per protocol design and they may post
        up event for v6. Just ignore rm ppp up if we are not expecting
      ---------------------------------------------------------------------*/
     DS_MSG1(MSG_LEGACY_ERROR,"PPP Configured EV received in wrong state %d", 
                        sm_ptr->pdp_specific_info.pdp_ip.state);
  } /*switch( sm_ptr->pdp_specific_info.pdp_ip.state )*/
func_exit:
  if(err_val != DS_LOCAL_ERR_VAL_NONE)
    DS_MSG1(MSG_LEGACY_ERROR,"ds_rmsm_ip_process_rm_ppp_up_ev err_val:%d",err_val);
  return;      
} /*ds_rmsm_ip_process_rm_ppp_up_ev*/

/*===========================================================================
  FUNCTION DS_RMSM_IP_PROCESS_RM_PPP_DOWN_EV

  DESCRIPTION
    This function processes RM_PPP_DOWN_EV event. 
    
  PARAMETERS
    sm_ptr: Pointer to the RmSm IP State Machine

  RETURN VALUE
    None

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
void ds_rmsm_ip_process_rm_ppp_down_ev
(
  ds_rmsmi_sm_info_type *sm_ptr
)
{
  ds_rmsmi_info_type             *rmsmi_info_ptr = NULL;
  ds_rmsm_instance_type          inst = {0};
  ds_local_err_val_e_type        err_val = DS_LOCAL_ERR_VAL_NONE;
  /*----------------------------------------------------------------------*/
  
  DS_MSG0(MSG_LEGACY_HIGH,"Received Rm PPP down EV");

  if (sm_ptr == NULL) 
  {
    err_val = DS_LOCAL_ERR_VAL_0;
    goto func_exit;
  }
      
  rmsmi_info_ptr = (ds_rmsmi_info_type*)sm_ptr->vobj_data_ptr;
  if (rmsmi_info_ptr == NULL) 
  {
    err_val = DS_LOCAL_ERR_VAL_1;
    goto func_exit;
  }
  
  inst.rm_dev_instance = rmsmi_info_ptr->instance;
  inst.sm_instance = sm_ptr->instance;

  DS_MSG2(MSG_LEGACY_HIGH, "Processing Rm PPP down EV, sm instance:%d internal state:%d", 
           sm_ptr->instance,sm_ptr->pdp_specific_info.pdp_ip.state);

  switch( sm_ptr->pdp_specific_info.pdp_ip.state )
  {
    case DS_RMSM_IP_WAITING_FOR_AUTOCONFIG_COMPLETE_STATE:
    case DS_RMSM_IP_UM_RM_UP_STATE:
    case DS_RMSM_IP_RM_RECONFIGURING_STATE:
    case DS_RMSM_IP_WAITING_FOR_RM_PPP_UP_STATE:
    {
      if (sm_ptr->instance == DS_RMSM_V6_SM_INSTANCE)
      {
        ds_3gpp_rmsm_autoconfig_sm_post_event(&inst,AUTOCONFIG_SM_STOP_ADDR_CFG_EV);
        ds_rmsm_ip_transition_state(sm_ptr, 
                              DS_RMSM_IP_UM_UP_WAITING_FOR_AUTOCONFIG_END_STATE);
      }
      else if (sm_ptr->instance == DS_RMSM_V4_SM_INSTANCE)
      {
        ds_rmsm_ip_transition_state(sm_ptr,
                                  DS_RMSM_IP_WAITING_FOR_UM_DOWN_STATE);
      }
      else
      {
         DS_MSG1(MSG_LEGACY_ERROR,"Rm PPP DOWN EV received for wrong PDP Type: %d",
                            DS_RMSMI_GET_PDP_TYPE(rmsmi_info_ptr));
         err_val = DS_LOCAL_ERR_VAL_2;
         goto func_exit;
      }   
    }
    break;
    
    case DS_RMSM_IP_WAITING_FOR_UM_UP_STATE:
      ds_rmsm_ip_transition_state(sm_ptr, DS_RMSM_IP_WAITING_FOR_UM_DOWN_STATE);
      break;

    case DS_RMSM_IP_UM_UP_WAITING_FOR_AUTOCONFIG_END_STATE:
    case DS_RMSM_IP_UM_DOWN_WAITING_FOR_AUTOCONFIG_END_STATE:
    case DS_RMSM_IP_WAITING_FOR_UM_DOWN_STATE:
    case DS_RMSM_IP_NULL_STATE:
      DS_MSG3(MSG_LEGACY_HIGH,"Rm PPP Down EV received in state %d, ignoring", 
                         sm_ptr->pdp_specific_info.pdp_ip.state,0,0);
      break;

    default:
     DS_MSG1(MSG_LEGACY_ERROR,"PPP Configured EV received in wrong state %d", 
                        sm_ptr->pdp_specific_info.pdp_ip.state);
  }/* switch( sm_ptr->pdp_specific_info.pdp_ip.state )*/
  
func_exit:
  if(err_val != DS_LOCAL_ERR_VAL_NONE)
    DS_MSG1(MSG_LEGACY_ERROR,"ds_rmsm_ip_process_rm_ppp_down_ev err_val:%d",err_val);
  return; 

} /*ds_rmsm_ip_process_rm_ppp_down_ev*/

/*===========================================================================
  FUNCTION DS_RMSM_IP_PROCESS_ADDR_CFG_FAILURE_EV()

  DESCRIPTION
    This function processes ADDR_CFG_FAILURE_EV event. 

  PARAMETERS
    sm_ptr: Pointer to the RmSm IP State Machine

  RETURN VALUE
    None

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
void ds_rmsm_ip_process_addr_cfg_failure_ev
(
  ds_rmsmi_sm_info_type *sm_ptr
)
{
  ds_rmsmi_info_type      *rmsmi_info_ptr = NULL;
  ds_rmsm_instance_type   inst = {0};
  ds_local_err_val_e_type err_val = DS_LOCAL_ERR_VAL_NONE;

  /*------------------------------------------------------------*/
  if (sm_ptr == NULL) 
  {
    err_val = DS_LOCAL_ERR_VAL_0;
    goto func_exit;
  }

  rmsmi_info_ptr = (ds_rmsmi_info_type*)sm_ptr->vobj_data_ptr;
  if (rmsmi_info_ptr == NULL) 
  {
    err_val = DS_LOCAL_ERR_VAL_1;
    goto func_exit;
  }

  inst.rm_dev_instance = rmsmi_info_ptr->instance;
  inst.sm_instance = sm_ptr->instance;

  DS_MSG3(MSG_LEGACY_HIGH, "Processing ADDR_CFG_FAILURE_EV, sm instance:%d state:%d", 
           sm_ptr->instance,sm_ptr->pdp_specific_info.pdp_ip.state, 0);

  if (sm_ptr->instance == DS_RMSM_V6_SM_INSTANCE)
  {
    switch( sm_ptr->pdp_specific_info.pdp_ip.state )
    {
      case DS_RMSM_IP_WAITING_FOR_RM_PPP_UP_STATE:
      case DS_RMSM_IP_RM_RECONFIGURING_STATE:

        ds_3gpp_rmsm_autoconfig_sm_post_event(&inst, 
                                              AUTOCONFIG_SM_STOP_ADDR_CFG_EV);
        ds_rmsm_ip_transition_state(sm_ptr,
                                    DS_RMSM_IP_WAITING_FOR_UM_DOWN_STATE);
      break;

      default:
        DS_MSG1(MSG_LEGACY_ERROR,"Address Configuration Failure EV received in wrong "
                          "state %d", sm_ptr->pdp_specific_info.pdp_ip.state);
      break;
    }
  }
  else
  {
    err_val = DS_LOCAL_ERR_VAL_2;
    goto func_exit;
  }
  
func_exit:
  if(err_val != DS_LOCAL_ERR_VAL_NONE)
    DS_MSG1(MSG_LEGACY_ERROR,"ds_rmsm_ip_process_addr_cfg_failure_ev err_val:%d",err_val);
  return; 
} /*ds_rmsm_ip_process_addr_cfg_failure_ev*/


/*===========================================================================
  FUNCTION DS_RMSM_IP_PROCESS_ADDR_CFG_CHANGED_EV()

  DESCRIPTION
    This function processes ADDR_CFG_CHANGED_EV event. 

  PARAMETERS
    sm_ptr: Pointer to the RmSm IP State Machine

  RETURN VALUE
    None

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
 void ds_rmsm_ip_process_addr_cfg_changed_ev
(
  ds_rmsmi_sm_info_type *sm_ptr
)
{
  ds_rmsmi_info_type* rmsmi_info_ptr;
  ds_local_err_val_e_type err_val = DS_LOCAL_ERR_VAL_NONE;
  /*----------------------------------------------------------*/

  if (sm_ptr == NULL) 
  {
    err_val = DS_LOCAL_ERR_VAL_0;
    goto func_exit;
  }

  rmsmi_info_ptr = (ds_rmsmi_info_type*)sm_ptr->vobj_data_ptr;
  if (rmsmi_info_ptr == NULL) 
  {
    err_val = DS_LOCAL_ERR_VAL_1;
    goto func_exit;
  }

  DS_MSG3(MSG_LEGACY_HIGH,"Processing Address Configuration changed EV,"
                          "sm instance:%d state:%d", sm_ptr->instance,
                          sm_ptr->pdp_specific_info.pdp_ip.state ,0);

  if (sm_ptr->instance == DS_RMSM_V6_SM_INSTANCE)
  {
    switch( sm_ptr->pdp_specific_info.pdp_ip.state )
    {
      case DS_RMSM_IP_UM_RM_UP_STATE:
        ds_rmsm_ip_transition_state (sm_ptr,
                               DS_RMSM_IP_RM_RECONFIGURING_STATE);
        break;

      default:
       DS_MSG1(MSG_LEGACY_ERROR,"Address Configuration Changed EV received in wrong "
                          "state %d", sm_ptr->pdp_specific_info.pdp_ip.state);
       break;
    }
  }
  else
  {
    err_val = DS_LOCAL_ERR_VAL_2;
    goto func_exit;
  }
  
func_exit:
  if(err_val != DS_LOCAL_ERR_VAL_NONE)
    DS_MSG1(MSG_LEGACY_ERROR,"ds_rmsm_ip_process_addr_cfg_changed_ev err_val:%d",err_val);
  return;   
  
} /*ds_rmsm_ip_process_addr_cfg_changed_ev*/
    

/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                          LOCAL FUNCTION DEFINTIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/
  
/*===========================================================================
  FUNCTION DS_RMSM_IP_TRANSITION_STATE()

  DESCRIPTION
    This function effects the transition to the state that is passed in.

  PARAMETERS
    sm_ptr: Pointer to the RmSm IP State Machine
    new_state: the state to transition to

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void ds_rmsm_ip_transition_state
(
  ds_rmsmi_sm_info_type       *sm_ptr,
  ds_rmsm_ipi_state_type      new_state
)
{
  int32                            retval         = 0;
  ds_rmsmi_info_type               *rmsmi_info_ptr =NULL;
  ds_rmsm_instance_type            inst;
  ds_3gpp_rmsm_ip_common_info_type *info_ptr = NULL;
  ds_fwk_clnt_info_s_type          clt_info = {0};
  ds_local_err_val_e_type          err_val = DS_LOCAL_ERR_VAL_NONE;
  ds_pdn_mgr_s                     *pdn_mgr_p = NULL;
  ds_pdn_mgr_instance_state_e       pdn_inst_state = DS_INSTANCE_STATE_DOWN;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (sm_ptr == NULL) 
  {
    err_val = DS_LOCAL_ERR_VAL_0;
    goto func_exit;
  }

  rmsmi_info_ptr = (ds_rmsmi_info_type*)sm_ptr->vobj_data_ptr;
  if (rmsmi_info_ptr == NULL) 
  {
    err_val = DS_LOCAL_ERR_VAL_1;
    goto func_exit;
  }

  inst.rm_dev_instance = rmsmi_info_ptr->instance;
  inst.sm_instance = sm_ptr->instance;

  info_ptr = rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr;
  if(info_ptr == NULL)
  {
    err_val = DS_LOCAL_ERR_VAL_2;
    goto func_exit;
  }

  DS_MSG3(MSG_LEGACY_HIGH, "DS_RMSM:sm_instance %d Internal state change from %d to %d", 
          sm_ptr->instance, sm_ptr->pdp_specific_info.pdp_ip.state, new_state );
      
  sm_ptr->pdp_specific_info.pdp_ip.state = new_state;   
  /*-------------------------------------------------------------------------
    Perform action associated with entering into new state.
  -------------------------------------------------------------------------*/
  switch( new_state )
  {
    case DS_RMSM_IP_NULL_STATE:
    {
      sm_ptr->sio_inst->state = RMSM_SIO_INST_DOWN;
    } 
    break;
    
    case DS_RMSM_IP_WAITING_FOR_UM_UP_STATE:
    {
      retval = ds_rmsm_bringup_pdn(sm_ptr);
      if(retval == DS_EWOULDBLOCK)
      {
        DS_MSG1(MSG_LEGACY_HIGH,
            "Trying to bringup RMSM call for client handle %d ",
            sm_ptr->instance_hdl);
      }
      else if(retval == -1)
      {
        DS_MSG0(MSG_LEGACY_ERROR,"Um Instance bringup failed!");
        /*-------------------------------------------------------------------
          Um instance could not be brought up, post the appropriate event
         -------------------------------------------------------------------*/
        ds_3gpp_rmsm_ip_common_post_event( &inst, DS_CMD_3GPP_RMSM_UM_PDN_DOWN );
        break;
      }
      else if(retval == 1)
      {
        DS_MSG1(MSG_LEGACY_HIGH,
            "RMSM instance in UM is already UP for client handle %d ",
            sm_ptr->instance_hdl);
        /*Update the fwk instance & RM config ptr with fw instance here
          as we are processing PDN UP here*/
        ds_fwk_get_clnt_info_from_handle(DS_FWK_CLIENT_RMSM,
                         (ds_fwk_clnt_handle_type)sm_ptr->instance_hdl,&clt_info);
        sm_ptr->fw_inst = clt_info.fw_inst;
        if(sm_ptr->instance == DS_RMSM_V4_SM_INSTANCE)
        {
          rmsmi_info_ptr->rm_ppp_config_ptr->ipcp_info.fwk_inst_ptr = sm_ptr->fw_inst;
          pdn_mgr_p = ds_fwk_get_pdn_ptr_from_fwk_inst(sm_ptr->fw_inst,IPV4_ADDR);
          if(pdn_mgr_p && pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV4])
          {
            pdn_inst_state = pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV4]->state;
          }
        }
        else if(sm_ptr->instance == DS_RMSM_V6_SM_INSTANCE)
        {  
          rmsmi_info_ptr->rm_ppp_config_ptr->ipv6cp_info.fwk_inst_ptr = sm_ptr->fw_inst;
          pdn_mgr_p = ds_fwk_get_pdn_ptr_from_fwk_inst(sm_ptr->fw_inst,IPV6_ADDR);
          if(pdn_mgr_p && pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV6])
          {
            pdn_inst_state = pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV6]->state;
          }
        }
        DS_MSG1(MSG_LEGACY_HIGH,"PDN instance state  %d ",pdn_inst_state);
        if(pdn_inst_state == DS_INSTANCE_STATE_UP)
        {
          ds_rmsm_ip_process_pdn_up_ev(sm_ptr);
        }
        else
        {
          DS_MSG1(MSG_LEGACY_HIGH,
            "Skip indicating PDN UP event as UM still coming UP for inst handle %d",
            sm_ptr->instance_hdl);
        }
        break;
      }

      ds_fwk_get_clnt_info_from_handle(DS_FWK_CLIENT_RMSM,
                         (ds_fwk_clnt_handle_type)sm_ptr->instance_hdl,
                         &clt_info);
      
      /* Store the fw index in sm_info_ptr */
      sm_ptr->fw_inst = clt_info.fw_inst;
      DS_MSG1(MSG_LEGACY_HIGH,"Fw instance ptr updated to 0x%p ",
                                sm_ptr->fw_inst);

    }
    break;
    
    case DS_RMSM_IP_UM_RM_UP_STATE:
      sm_ptr->sio_inst->state = RMSM_SIO_INST_UP;
    break;

    case DS_RMSM_IP_WAITING_FOR_UM_DOWN_STATE:
    {
      retval = ds_rmsm_bringdown_pdn(sm_ptr);
      if(retval == 0 )
      {
	      DS_MSG0(MSG_LEGACY_HIGH,"PDN brought down successfully.Wait for PDN callback");
        //ds_3gpp_rmsm_ip_common_post_event(&inst, DS_CMD_3GPP_RMSM_UM_PDN_DOWN);
      }
    }
    break;
  
    case DS_RMSM_IP_WAITING_FOR_RM_PPP_UP_STATE:
    case DS_RMSM_IP_WAITING_FOR_AUTOCONFIG_COMPLETE_STATE:
    case DS_RMSM_IP_UM_UP_WAITING_FOR_AUTOCONFIG_END_STATE:
    case DS_RMSM_IP_UM_DOWN_WAITING_FOR_AUTOCONFIG_END_STATE:
    case DS_RMSM_IP_RM_RECONFIGURING_STATE:
      break;

    default:
      DS_MSG1(MSG_LEGACY_ERROR,"Invalid Internal state %d - transistion", new_state);
      break;

  } /* switch(new state) */

func_exit:
  if(err_val != DS_LOCAL_ERR_VAL_NONE)
    DS_MSG1(MSG_LEGACY_ERROR,"ds_rmsm_ip_transition_state err_val:%d",err_val);
  
  return;
} /* ds_rmsm_ip_transition_state() */


/*===========================================================================
  FUNCTION DS_RMSM_IP_GET_STATE()

  DESCRIPTION
    This function effects the transition to the state that is passed in.

  PARAMETERS
    sm_ptr: Pointer to the RmSm IP State Machine

  RETURN VALUE
    ds_rmsm_ipi_state_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

ds_rmsm_ipi_state_type ds_rmsm_ip_get_state
(
  ds_rmsmi_sm_info_type *sm_ptr
)
{
  if (sm_ptr == NULL)
  {
    DS_MSG0(MSG_LEGACY_ERROR,"Invalid sm ptr passed");
    return DS_RMSM_IP_NULL_STATE;
  }

  return sm_ptr->pdp_specific_info.pdp_ip.state;
}

/*===========================================================================
  FUNCTION DS_RMSM_IP_PROCESS_PDN_UP_EV()

  DESCRIPTION
    This function processes the Um side PDN UP event callback from FW.

  PARAMETERS
    sm_ptr: Pointer to the RmSm IP State Machine

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void ds_rmsm_ip_process_pdn_up_ev
(
  ds_rmsmi_sm_info_type *sm_ptr
)
{
  ds_rmsmi_info_type               *rmsmi_info_ptr = NULL;
  ds_3gpp_rmsm_ip_common_info_type *info_ptr = NULL;
  ds_rmsm_instance_type            inst = {0};
  ds_local_err_val_e_type          err_val = DS_LOCAL_ERR_VAL_NONE;
  /*- - - - - - - - - - - - - - - - - - - --- - - - - - - - - - - - - - - - -*/

  if (sm_ptr == NULL) 
  {
    err_val = DS_LOCAL_ERR_VAL_0;
    goto func_exit;
  }

  rmsmi_info_ptr = (ds_rmsmi_info_type*)sm_ptr->vobj_data_ptr;
  if (rmsmi_info_ptr == NULL) 
  {
    err_val = DS_LOCAL_ERR_VAL_1;
    goto func_exit;
  }
  inst.rm_dev_instance = rmsmi_info_ptr->instance;
  inst.sm_instance = sm_ptr->instance;

  info_ptr = rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr;
  if(info_ptr == NULL)
  {
    err_val = DS_LOCAL_ERR_VAL_2;
    goto func_exit;
  }

  DS_MSG2(MSG_LEGACY_HIGH, "Recd UM_PDN_UP_EV, sm type: %d state:%d", 
          sm_ptr->instance,sm_ptr->pdp_specific_info.pdp_ip.state);

  switch( sm_ptr->pdp_specific_info.pdp_ip.state )
  {
    case DS_RMSM_IP_WAITING_FOR_UM_UP_STATE:
    case DS_RMSM_IP_WAITING_FOR_RM_PPP_UP_STATE:
      /*---------------------------------------------------------------------
        PDN is up. Get PPP parameters from Um
      ---------------------------------------------------------------------*/
      if (sm_ptr->instance == DS_RMSM_V4_SM_INSTANCE)
      {
        if(IPCP_CREQ_RECD(info_ptr) == FALSE)
          break;
        
        if( -1 == ds_rmsm_set_addr_family(sm_ptr->sio_inst,
                                            IPV4_ADDR))
        {
          DS_MSG1(MSG_LEGACY_ERROR,"Error while setting the Address Family to V4,"
                             " sio_inst ptr: %x", sm_ptr->sio_inst);
          ds_rmsm_ip_transition_state(sm_ptr, 
                        DS_RMSM_IP_WAITING_FOR_UM_DOWN_STATE);                              
          break;
        }
        else 
        {       
          if (info_ptr->configure_ppp_f_ptr == NULL)
          {
            err_val = DS_LOCAL_ERR_VAL_3;
            goto func_exit;
          }
          if (info_ptr->configure_ppp_f_ptr ((void*)rmsmi_info_ptr, 
                                              PPP_PROTOCOL_IPCP) == FALSE)
          {
            DS_MSG0(MSG_LEGACY_ERROR,"PPP Configuration failed");
            ds_rmsm_ip_transition_state(sm_ptr, DS_RMSM_IP_WAITING_FOR_UM_DOWN_STATE);
          }
          else
          {
            DS_MSG0(MSG_LEGACY_HIGH,"PPP Configuration successful (v4)");
            ds_rmsm_ip_transition_state(sm_ptr, DS_RMSM_IP_WAITING_FOR_RM_PPP_UP_STATE);
          } 
        } 
      }
      else if (sm_ptr->instance == DS_RMSM_V6_SM_INSTANCE)
      {
        if(IPV6CP_CREQ_RECD(info_ptr) == FALSE)
          break;
        
        ds_3gpp_rmsm_autoconfig_sm_init_config (sm_ptr);
        ds_3gpp_rmsm_autoconfig_sm_post_event (&inst,
                             AUTOCONFIG_SM_START_ADDR_CFG_EV);
        ds_rmsm_ip_transition_state(sm_ptr,
                      DS_RMSM_IP_WAITING_FOR_RM_PPP_UP_STATE);
      }
      else
      {
        DS_MSG1(MSG_LEGACY_ERROR,"Invalid PDP Type %d", 
                           DS_RMSMI_GET_PDP_TYPE(rmsmi_info_ptr));
      }      
      break;

    case DS_RMSM_IP_WAITING_FOR_AUTOCONFIG_COMPLETE_STATE:
    case DS_RMSM_IP_UM_RM_UP_STATE:
    case DS_RMSM_IP_WAITING_FOR_UM_DOWN_STATE:
    case DS_RMSM_IP_NULL_STATE:
      /*---------------------------------------------------------------------
        Ignore event in all other states.
      ---------------------------------------------------------------------*/
      DS_MSG2(MSG_LEGACY_HIGH, "Ignoring PDN UP event Sm type :%d state:%d",
                sm_ptr->instance,sm_ptr->pdp_specific_info.pdp_ip.state);
      break;

    default:
      DS_MSG1(MSG_LEGACY_ERROR, "Invalid RMSM state %d",
           sm_ptr->pdp_specific_info.pdp_ip.state);
      break;

  } /* switch( sm_ptr->pdp_specific_info.pdp_ip.st*/
  
func_exit:
  if(err_val != DS_LOCAL_ERR_VAL_NONE)
    DS_MSG1(MSG_LEGACY_ERROR,"ds_rmsm_ip_process_cmd err_val:%d",err_val);
  return;
}

/*===========================================================================
  FUNCTION DS_RMSM_SET_ADDR_FAMILY

  DESCRIPTION
    This function processes the Um side PDN UP event callback from FW.

  PARAMETERS
    sm_ptr: Pointer to the RmSm IP State Machine

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
int ds_rmsm_set_addr_family(ds_rmsm_sio_instance* sio_inst,
                                    ip_addr_enum_type ip_addr_family )
{
  if(sio_inst == NULL)
    return -1;
  
  sio_inst->addr_family = ip_addr_family;
  return 0;
}


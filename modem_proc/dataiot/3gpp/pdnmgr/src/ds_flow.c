/*===========================================================================
                      DS_FLOW.C
DESCRIPTION
This file defines DS flow APIs.

Copyright (c) 2018 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/3gpp/pdnmgr/src/ds_flow.c#6 $
  $DateTime: 2023/02/02 03:45:03 $$Author: pwbldsvc $

when           who    what, where, why
--------    ---    ----------------------------------------------------------
08/09/17    sd      First checkin
===========================================================================*/

#include "ds_pdn_defs.h"
#include "ds_flow.h"
#include "ds_pdn_manager.h"
#include "ds_bearer_flow_manager.h"
#include "ds_fwk_rx_qos_fltr.h"


ds_flow_type *ds_flow_context_tbl[DS_3GPP_MAX_FLOW_CONTEXT] = {NULL};


/*===========================================================================
FUNCTION DS_FLOW_IS_VALID()

DESCRIPTION
  This function checks for validity of flow ptr.

PARAMETERS
  flow_ptr

RETURN VALUE
  boolean.

DEPENDENCIES
  None.

SIDE EFFECTS
  None
===========================================================================*/
boolean ds_flow_is_valid
(
  ds_flow_type                  *flow_p
)
{
  if(flow_p != NULL &&
  	  flow_p->this_flow == flow_p)
  {
    return TRUE;
  }

  return FALSE;
}/* ds_flow_is_valid */

/*===========================================================================
FUNCTION DS_FLOW_SET_CAPABILITY()

DESCRIPTION
  This function sets capability for flow.

PARAMETERS
  flow_ptr
  flow capability

RETURN VALUE
  None.

DEPENDENCIES
  None.

SIDE EFFECTS
  None
===========================================================================*/
void ds_flow_set_capability
(
  ds_flow_type                  *flow_p,
  ds_flow_capability_enum_type   flow_cap
)
{
  if(!ds_flow_is_valid(flow_p))
  {
    return;
  }
  	
  if(flow_cap < DS_FLOW_CAPABILITY_MAX)
  {
    flow_p->capability_mask |= (0x1 << (uint8)(flow_cap));
  }
}/* ds_flow_set_capability */


/*===========================================================================
FUNCTION DS_FLOW_RESET_CAPABILITY()

DESCRIPTION
  This function resets capability for flow.

PARAMETERS
  flow_ptr
  flow capability

RETURN VALUE
  None.

DEPENDENCIES
  None.

SIDE EFFECTS
  None
===========================================================================*/
void ds_flow_reset_capability
(
  ds_flow_type                  *flow_p,
  ds_flow_capability_enum_type   flow_cap
)
{
  if(!ds_flow_is_valid(flow_p))
  {
    return;
  }
  
  if(flow_cap < DS_FLOW_CAPABILITY_MAX)
  {
    flow_p->capability_mask &= ~(0x1 << (uint8)(flow_cap));
  }
}/* ds_flow_reset_capability */


/*===========================================================================
FUNCTION DS_FLOW_ALLOCATE_CONTEXT()

DESCRIPTION
  This function creates a new flow and initializes it.

PARAMETERS
  inst_ptr

RETURN VALUE
  ds_flow_type - Flow ptr.

DEPENDENCIES
  None.

SIDE EFFECTS
  None
===========================================================================*/
ds_flow_type* ds_flow_allocate_context
(
  ds_pdn_mgr_inst_type_s  *inst_p
)
{
  uint8          index = 0;
  ds_flow_type  *flow_p = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for(index=0; index<DS_3GPP_MAX_FLOW_CONTEXT; index++)
  {
    if(ds_flow_context_tbl[index] == NULL)
    {
      DS_SYSTEM_HEAP_MEM_ALLOC(flow_p, sizeof(ds_flow_type),
                               ds_flow_type*);
      if(flow_p == NULL)
      {
        return NULL;
      }
    
      memset(flow_p, 0, sizeof(ds_flow_type));
    
      flow_p->state = DS_FLOW_STATE_DOWN;
      flow_p->inst_ptr = inst_p;
      flow_p->this_flow = flow_p;
      
      if(inst_p->type == DS_PDN_INSTANCE_IPV4)
      {
        flow_p->flow_type = DS_FLOW_NW_FLOW_TYPE | DS_FLOW_V4_FLOW_TYPE;
      }
      else if(inst_p->type == DS_PDN_INSTANCE_IPV6)
      {
        flow_p->flow_type = DS_FLOW_NW_FLOW_TYPE | DS_FLOW_V6_FLOW_TYPE;
      }
      
      DS_SYSTEM_HEAP_MEM_ALLOC(flow_p->qos_info_ptr,
      	                        sizeof(qos_spec_type),qos_spec_type*);
      if(flow_p->qos_info_ptr == NULL)
      {
        DS_SYSTEM_HEAP_MEM_FREE(flow_p);
        return NULL;
      }
    
      memset(flow_p->qos_info_ptr, 0, sizeof(qos_spec_type));

      flow_p->index = index; 
      flow_p->sdf_id = index+1;

      ds_flow_context_tbl[index] = flow_p;
      
      DS_MSG1(MSG_LEGACY_HIGH, "Allocated flow context at index %d !", index);
      return flow_p;
    }
  }

  return NULL;
} /* ds_flow_create() */

/*===========================================================================
FUNCTION DS_FLOW_CONFIGURE_FLOW_CONTEXT

DESCRIPTION
  This function is invoked to configure the flow context.

PARAMETERS
  PDN ptr, bearer ptr, and instance type

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
===========================================================================*/
void ds_flow_configure_flow_context
( 
  ds_flow_type               *flow_context_p,
  ds_bearer_mgr_s            *bearer_mgr_p
)
{  
  if(!ds_flow_is_valid(flow_context_p))
  {
    return;
  }

  flow_context_p->bearer_flow_mgr_ptr = 
                            ds_bearer_flow_manager_find_entry(bearer_mgr_p);
  return;
}

/*===========================================================================
FUNCTION DS_FLOW_ENABLE_DISABLE_TX
 
DESCRIPTION
  This function processes the TX flow enable/disable event posted to DS flow

PARAMETERS 
  Flow pointer
  boolean - Flow enabled/disabled
 
DEPENDENCIES 
  None. 
  
RETURN VALUE 
  None
 
SIDE EFFECTS 
  None
===========================================================================*/
void ds_flow_enable_disable_tx
(
  ds_flow_type        *flow_p,
  boolean              enable_flow
)
{
  if(!ds_flow_is_valid(flow_p))
  {
    return;
  }

  if(flow_p->tx_flow_enabled != enable_flow)
  {
    flow_p->tx_flow_enabled = enable_flow;
    DS_MSG2(MSG_LEGACY_HIGH, "Setting flow enabled to %d on flow %x",
            enable_flow, flow_p);    
  }

  return;
}

/*===========================================================================
FUNCTION DS_FLOW_GO_NULL_IND

DESCRIPTION
  Indicates that a flow is torn down

PARAMETERS
  flow_ptr  : ptr to flow on which to operate on.

RETURN VALUE
   None

DEPENDENCIES
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void ds_flow_go_null_ind
(
  ds_flow_type      *flow_ptr
)
{
  ds_pdn_event_info_u_type  event_info ;

  if(!ds_flow_is_valid(flow_ptr))
  {
    return;
  }

  memset(&event_info,0,sizeof(ds_pdn_event_info_u_type));
  // FLOW NULL event, if bearer is not linked, no need to notify.
  if(ds_bearer_mgr_validate_bearer_ptr(flow_ptr->bearer_ptr))
  {
    event_info.net_initiated_qos_info.qos_handle = 
                                (ds_net_initiated_qos_req_handle_type)flow_ptr;
    ds_fwk_notify_events(flow_ptr->bearer_ptr->pdn_mgr_ptr->fw_index,
                         ds_pdn_mgr_convert_pdn_inst_to_fw_ip_type(
                                         flow_ptr->inst_ptr->type),
                         PDN_FLOW_DELETED_EV,
                         event_info);
  }
  else
  {
    DS_MSG1(MSG_LEGACY_HIGH, "Invalid bearer associated with flow 0x%x !", flow_ptr);
  }

  // Enable flow and unbind bearer
  ds_flow_enable_disable_tx(flow_ptr, TRUE);

  flow_ptr->state = DS_FLOW_STATE_DOWN;

  return;  
} /* ds_flow_go_null_ind() */



/*===========================================================================
FUNCTION DS_FLOW_MGR_CONFIGURE_DEFAULT_FLOW

DESCRIPTION
  This function is invoked to configure the default flow for a given instance.
  Here we bind flow to the default bearer 

PARAMETERS
  PDN ptr, bearer ptr, and instance type

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
===========================================================================*/
void ds_flow_mgr_configure_default_flow
(  
  ds_pdn_mgr_s               *pdn_mgr_p,
  ds_bearer_mgr_s            *bearer_mgr_p,
  ds_pdn_mgr_inst_type_s     *inst_p
)
{  
   ds_flow_type               *default_flow_ptr;
  /*-------------------------------------------------------------------------*/

  if(inst_p == NULL)
  {    
    DS_ERROR_LOG_0("Instance Pointer is NULL");
    return;
  }

  if(inst_p->type != DS_PDN_INSTANCE_IPV4 && inst_p->type != DS_PDN_INSTANCE_IPV6)
  {
   	return;
  }

  default_flow_ptr = inst_p->flow->default_flow_ptr;
  if(!ds_flow_is_valid(default_flow_ptr))
  {
    return;
  }

  /*----------------------------------------------------------------------------
    Bind the default flow with the bearer and instance
   ----------------------------------------------------------------------------*/  
  default_flow_ptr->bearer_ptr = bearer_mgr_p;
  default_flow_ptr->inst_ptr = inst_p;

  // Allocate filter list for default flow
  if(default_flow_ptr->qos_info_ptr->rx.fltr_template.list_ptr == NULL)
  {
    DS_SYSTEM_HEAP_MEM_ALLOC(
       default_flow_ptr->qos_info_ptr->rx.fltr_template.list_ptr,
       sizeof(ip_filter_type)*MAX_PACKET_FILTERS,
       ip_filter_type*);
    memset(default_flow_ptr->qos_info_ptr->rx.fltr_template.list_ptr, 0,
    	      sizeof(ip_filter_type)*MAX_PACKET_FILTERS);
  }
  
  if(default_flow_ptr->qos_info_ptr->tx.fltr_template.list_ptr == NULL)
  {
    DS_SYSTEM_HEAP_MEM_ALLOC(
       default_flow_ptr->qos_info_ptr->tx.fltr_template.list_ptr,
       sizeof(ip_filter_type)*MAX_PACKET_FILTERS,
       ip_filter_type*);
    memset(default_flow_ptr->qos_info_ptr->tx.fltr_template.list_ptr, 0,
    	      sizeof(ip_filter_type)*MAX_PACKET_FILTERS);
  }
  
  ds_bearer_flow_manager_add_flow_context_to_flow_manager(bearer_mgr_p, 
                                                          default_flow_ptr);
  return;
}

/*===========================================================================
FUNCTION DS_FLOW_UNBIND_DEFAULT_FLOWS

DESCRIPTION
  This function is used to unbind the phys link from all the default flows
  associated with the instances of a given PDN context

PARAMETERS   
  pointer to the PDN context
    
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void ds_flow_unbind_default_flows
(
  ds_pdn_mgr_s    *pdn_mgr_p
)
{
  uint8                    index;
  ds_pdn_mgr_inst_type_s  *inst_p;
  ds_flow_type            *ds_flow_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for( index = DS_PDN_INSTANCE_IPV4; 
       index <= DS_PDN_INSTANCE_IPV6;
       index++ )
  {
    inst_p = pdn_mgr_p->inst_ptr[index];
    if (inst_p == NULL)
    {
      continue;
    }

    ds_flow_ptr = inst_p->flow->default_flow_ptr;    
    if(ds_flow_ptr != NULL)
    {
      // reset capability
      ds_flow_reset_capability(ds_flow_ptr, DS_FLOW_CAPABILITY_DATA_DISALLOWED);      
    }

    ds_flow_go_null_ind(ds_flow_ptr);
  }/* for each instance in the PDN context */

  return;
} /* ds_flow_unbind_default_flows */

/*===========================================================================
FUNCTION DS_FLOW_CONTEXT_SET_GRANTED_FLOW

DESCRIPTION
 Set the granted flow.

PARAMETERS
  flow_context_p   - Flow Context information.

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_flow_context_set_granted_flow
( 
  ds_flow_type    *flow_context_p
)
{
  ds_bearer_flow_manager_s *bearer_flow_mgr_p = NULL;  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  if(!ds_flow_is_valid(flow_context_p))
  {
    return;
  }
  
  bearer_flow_mgr_p = flow_context_p->bearer_flow_mgr_ptr; 
  if(!bearer_flow_mgr_p)
  {
    return;
  }

  
  DS_MSG1(MSG_LEGACY_HIGH, "Set Granted flow on flow context:0x%x",
          flow_context_p);
  
  flow_context_p->qos_info_ptr->rx.flow_template.granted_flow = 
                                                    bearer_flow_mgr_p->rx_flow;
  flow_context_p->qos_info_ptr->tx.flow_template.granted_flow = 
                                                    bearer_flow_mgr_p->tx_flow;
} /*ds_flow_context_ps_flow_set_granted_flow*/

/*===========================================================================
FUNCTION DS_FLOW_CONTEXT_ACTIVATE_IND

DESCRIPTION
  Post activated indication on the DS flow associated with the flow context.

PARAMETERS
  flow_context_p   - Flow Context information.

DEPENDENCIES
  None.

RETURN VALUE
  
SIDE EFFECTS
  None.
===========================================================================*/
void ds_flow_context_activate_ind
( 
  ds_flow_type                      *flow_context_p
)
{
  ds_pdn_event_info_u_type  event_info;
  if(!ds_flow_is_valid(flow_context_p))
  {
    return;
  }

  if(flow_context_p->bearer_ptr == NULL)
  {
    DS_MSG0(MSG_LEGACY_HIGH, "Flow assoc to a dummy instance. Ignore");
    return;
  }
  memset(&event_info,0,sizeof(ds_pdn_event_info_u_type));
  ds_flow_context_set_granted_flow(flow_context_p);

  // FLOW activated event  
  event_info.net_initiated_qos_info.qos_handle = 
                          (ds_net_initiated_qos_req_handle_type)flow_context_p;
  ds_fwk_notify_events(flow_context_p->bearer_ptr->pdn_mgr_ptr->fw_index,
                       ds_pdn_mgr_convert_pdn_inst_to_fw_ip_type(
                                         flow_context_p->inst_ptr->type),
                       PDN_FLOW_ADDED_EV,
                       event_info);

  ds_flow_enable_disable_tx(flow_context_p, TRUE);
  flow_context_p->state = DS_FLOW_STATE_UP;

  return;
} /* ds_flow_context_activate_ind */

/*===========================================================================
FUNCTION DS_FLOW_DELETE_QOS_MODIFY_SPEC

DESCRIPTION
  This function deletes the QoS modify spec.

PARAMETERS
  flow_ptr       

RETURN VALUE
  None.

DEPENDENCIES
  None.

SIDE EFFECTS
  None
===========================================================================*/
void ds_flow_delete_qos_modify_spec
(
  ds_flow_type           *flow_ptr
)
{
  
  if(!ds_flow_is_valid(flow_ptr))
  {
    return;
  }

  if(!flow_ptr->qos_modify_info_ptr)
  {
    return;
  }

  if(flow_ptr->qos_modify_info_ptr->rx.fltr_template.list_ptr != NULL)
  {
    DS_SYSTEM_HEAP_MEM_FREE(
            flow_ptr->qos_modify_info_ptr->rx.fltr_template.list_ptr);
  }
  
  if(flow_ptr->qos_info_ptr->tx.fltr_template.list_ptr != NULL)
  {
    DS_SYSTEM_HEAP_MEM_FREE(
            flow_ptr->qos_modify_info_ptr->tx.fltr_template.list_ptr);      
  }

  DS_SYSTEM_HEAP_MEM_FREE(flow_ptr->qos_modify_info_ptr);

  DS_MSG1(MSG_LEGACY_HIGH, "ds_flow_delete_qos_modify_spec():"
          "Flow 0x%p: QOS_MODIFY_SPEC released", flow_ptr);

  return;
}/* ds_flow_delete_qos_modify_spec */

/*===========================================================================
FUNCTION DS_FLOW_DELETE

DESCRIPTION
  This function deletes the flow and removes it from instance.

PARAMETERS
  flow_ptr       

RETURN VALUE
  None.

DEPENDENCIES
  None.

SIDE EFFECTS
  None
===========================================================================*/
void ds_flow_delete
(
  ds_flow_type           *flow_ptr
)
{
  ds_flow_type           *tmp_flow_ptr = NULL;
  ds_pdn_mgr_inst_type_s *inst_p = NULL;
  uint8                   index;
  ds_ipfltr_delete_param_type   fltr_delete_param_ptr = {0};
  int16                         err_no;

  if(!ds_flow_is_valid(flow_ptr))
  {
    return;
  }
  
  inst_p = flow_ptr->inst_ptr;
 	index = flow_ptr->index;

  if(flow_ptr != inst_p->flow->default_flow_ptr)
  {
    tmp_flow_ptr = list_peek_front(&(inst_p->flow->sec_flow_list));
    while (tmp_flow_ptr != NULL && tmp_flow_ptr != flow_ptr)
    {
      tmp_flow_ptr =  list_peek_next(&(inst_p->flow->sec_flow_list),
                                     &(tmp_flow_ptr->link));
    }
    
    if (tmp_flow_ptr == NULL)
    {
      return;
    }
    
    list_pop_item(&(inst_p->flow->sec_flow_list),
                  &(flow_ptr->link));
    inst_p->flow->num_sec_flows--;
  }

  if(flow_ptr->qos_info_ptr->tx.fltr_template.fltr_handle != 
                                                    DS_IPFLTR_INVALID_HANDLE)
  {
    fltr_delete_param_ptr.fw_inst_ptr = ds_fwk_get_inst_ptr_from_index(
                                     flow_ptr->bearer_ptr->pdn_mgr_ptr->fw_index);
    fltr_delete_param_ptr.client_id = FLTR_CLIENT_QOS_OUTPUT;
    fltr_delete_param_ptr.fi_handle = 
                        flow_ptr->qos_info_ptr->tx.fltr_template.fltr_handle;
    ds_ipfltr_delete(&fltr_delete_param_ptr, &err_no);
  }

  if(flow_ptr->qos_info_ptr->rx.fltr_template.fltr_handle != 
                                                    DS_IPFLTR_INVALID_HANDLE)
  {
    ds_fwk_rx_qos_fltr_delete(ds_fwk_get_inst_ptr_from_index(
                                     flow_ptr->bearer_ptr->pdn_mgr_ptr->fw_index),
                              flow_ptr->qos_info_ptr->rx.fltr_template.fltr_handle);    
  }

  // Delete qos_modify_info_ptr if valid
  if(flow_ptr->qos_modify_info_ptr != NULL)
  {
    if(flow_ptr->qos_modify_info_ptr->tx.fltr_template.fltr_handle != 
                                                    DS_IPFLTR_INVALID_HANDLE)
    {
      fltr_delete_param_ptr.fw_inst_ptr = ds_fwk_get_inst_ptr_from_index(
                                     flow_ptr->bearer_ptr->pdn_mgr_ptr->fw_index);
      fltr_delete_param_ptr.client_id = FLTR_CLIENT_QOS_OUTPUT;
      fltr_delete_param_ptr.fi_handle = 
                 flow_ptr->qos_modify_info_ptr->tx.fltr_template.fltr_handle;
      ds_ipfltr_delete(&fltr_delete_param_ptr, &err_no);
    }
    
    if(flow_ptr->qos_modify_info_ptr->rx.fltr_template.fltr_handle != 
                                                    DS_IPFLTR_INVALID_HANDLE)
    {
      ds_fwk_rx_qos_fltr_delete(ds_fwk_get_inst_ptr_from_index(
                                     flow_ptr->bearer_ptr->pdn_mgr_ptr->fw_index),
                              flow_ptr->qos_info_ptr->rx.fltr_template.fltr_handle);
    }

    ds_flow_delete_qos_modify_spec(flow_ptr);
  }

  if(flow_ptr->qos_info_ptr->rx.fltr_template.list_ptr != NULL)
  {
    DS_SYSTEM_HEAP_MEM_FREE(
            flow_ptr->qos_info_ptr->rx.fltr_template.list_ptr);
  }
  
  if(flow_ptr->qos_info_ptr->tx.fltr_template.list_ptr != NULL)
  {
    DS_SYSTEM_HEAP_MEM_FREE(
            flow_ptr->qos_info_ptr->tx.fltr_template.list_ptr);      
  }

  DS_SYSTEM_HEAP_MEM_FREE(flow_ptr->qos_info_ptr);

  DS_MSG1(MSG_LEGACY_HIGH, "Flow %x deleted now", flow_ptr);

  flow_ptr->this_flow = NULL;
  if(flow_ptr == inst_p->flow->default_flow_ptr)
  {
    inst_p->flow->default_flow_ptr = NULL;
  }
  DS_SYSTEM_HEAP_MEM_FREE(flow_ptr);
  flow_ptr = NULL;

  ds_flow_context_tbl[index] = NULL;

  return;
}/* ds_flow_delete */

/*===========================================================================
FUNCTION DS_FLOW_RELEASE_FLOW_CONTEXT

DESCRIPTION
  Return the flow context.

PARAMETERS
  flow_context_p  - Pointer to the flow context.

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_flow_release_flow_context
(
  ds_flow_type *flow_context_p
)
{  
  if(ds_flow_is_valid(flow_context_p))
  {
    if(flow_context_p->inst_ptr != NULL)
    {
      ds_flow_go_null_ind(flow_context_p);
      ds_flow_delete(flow_context_p);
    }
  }

  return;
}/* ds_flow_release_flow_context */

/*===========================================================================
FUNCTION DS_FLOW_IS_TX_FLOW_ENABLED
 
DESCRIPTION
  This function returns the TX flow status of the DS flow.

PARAMETERS 
  Flow pointer
 
DEPENDENCIES 
  None. 
  
RETURN VALUE 
  boolean
 
SIDE EFFECTS 
  None
===========================================================================*/
boolean ds_flow_is_tx_flow_enabled
(
  ds_flow_type     *flow_p
)
{
  if(ds_flow_is_valid(flow_p))
  {
    return flow_p->tx_flow_enabled;
  }

  return FALSE;
}

/*===========================================================================
 FUNCTION DS_FLOW_GET_CAPABILITY()
  
DESCRIPTION
  Returns TRUE if a capability of a flow is turned on.
 
  NOTE : Only one capability can be fetched each time.

PARAMETERS
  flow_ptr   : pointer to flow
  capability : capability which needs to be tested
 
RETURN VALUE
  TRUE  : if capability is turned on
  FALSE : otherwise
===========================================================================*/
boolean ds_flow_get_capability
(
  ds_flow_type                  * flow_ptr,
  ds_flow_capability_enum_type    capability
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  if ( capability < DS_FLOW_CAPABILITY_MAX &&
       ds_flow_is_valid(flow_ptr) )
  {
    return (flow_ptr->capability_mask & (0x1 << (uint8)(capability)));
  }
  else
  {
    return FALSE;
  }
} /* ds_flow_get_capability() */

/*===========================================================================
FUNCTION DS_FLOW_CONTEXT_GET_FILTER_SPEC

DESCRIPTION
  Retrieve the current filters assigned to the flow context.

PARAMETERS
  flow_context_p   - Flow Context information.
  modify           - boolean indicates current copy or modify set.
  app_fltr_spec    - Application filter spec to be populated.
  
DEPENDENCIES
  None.

RETURN VALUE
  boolean          - Returns true if filters are retrieved
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_context_get_filter_spec
( 
 ds_flow_type         *flow_context_p,
 boolean               modify,
 qos_spec_type        *app_fltr_spec
)
{
  qos_spec_type       *qos_spec_type = NULL;
  int                  i = 0;

  if(!ds_flow_is_valid(flow_context_p) || !app_fltr_spec)
  {
    return FALSE;
  }

  if(modify)
  {
    qos_spec_type = flow_context_p->qos_modify_info_ptr;
  }
  else
  {
    qos_spec_type = flow_context_p->qos_info_ptr;
  }

  if(qos_spec_type == NULL)
  {
    return FALSE;
  }

  app_fltr_spec->field_mask = qos_spec_type->field_mask;

  app_fltr_spec->rx.fltr_template.num_filters = 0;
  for(i=0; i<qos_spec_type->rx.fltr_template.num_filters; i++)
  {
    app_fltr_spec->rx.fltr_template.list_ptr[i] = 
                                   qos_spec_type->rx.fltr_template.list_ptr[i];
    app_fltr_spec->rx.fltr_template.num_filters++;
  }

  app_fltr_spec->tx.fltr_template.num_filters = 0;
  for(i=0; i<qos_spec_type->tx.fltr_template.num_filters; i++)
  {
    app_fltr_spec->tx.fltr_template.list_ptr[i] = 
                                   qos_spec_type->tx.fltr_template.list_ptr[i];
    app_fltr_spec->tx.fltr_template.num_filters++;
  }

  return TRUE;
}/* ds_flow_context_get_filter_spec */

/*===========================================================================
FUNCTION DS_FLOW_GET_DEFAULT_FLOW_FROM_BEARER

DESCRIPTION
  Return the flow context.

PARAMETERS
  bearer_mgr_p  - Pointer to the bearer.
  instance type - v4/v6

DEPENDENCIES
  None.

RETURN VALUE
  Flow ptr
  
SIDE EFFECTS
  None.
===========================================================================*/
ds_flow_type* ds_flow_mgr_get_default_flow_from_bearer
(  
  ds_bearer_mgr_s              *bearer_mgr_p,
  ds_pdn_mgr_instance_type_e    inst_type
)
{
  ds_pdn_mgr_s           *pdn_mgr_p = NULL;
  ds_pdn_mgr_inst_type_s *inst_p = NULL;  
  
  if( !bearer_mgr_p || inst_type >= DS_PDN_INSTANCE_MAX )
  {
    return NULL;
  }

  pdn_mgr_p = bearer_mgr_p->pdn_mgr_ptr;
  if( pdn_mgr_p != NULL )
  {
    inst_p = pdn_mgr_p->inst_ptr[inst_type];
  	 if( inst_p != NULL &&
  	 	   inst_p->flow != NULL )
    {
      return inst_p->flow->default_flow_ptr;
    }
  }

  return NULL;
}/* ds_flow_mgr_get_default_flow_from_bearer */

/*===========================================================================
FUNCTION ds_flow_log_qos_spec_filter_params

DESCRIPTION
This function prints the filter specifications

PARAMETERS
fltr_buf_ptr   - Pointer to PS QOS spec

DEPENDENCIES

RETURN VALUE
None.

SIDE EFFECTS
None.

===========================================================================*/
void ds_flow_log_qos_spec_filter_params
(
 const ip_filter_type    *fltr_buf_ptr        /* Pointer to PS QOS spec   */
)
{
  if (NULL == fltr_buf_ptr)
  {
    return;
  }

  DS_MSG1(MSG_LEGACY_HIGH,
          "ds_flow_log_qos_spec_filter_params(): ip version: %d", 
          fltr_buf_ptr->ip_vsn );

  if( IP_V4 == fltr_buf_ptr->ip_vsn )
  {
    DS_MSG1(MSG_LEGACY_HIGH, "filter param mask: 0x%x",
            (int)fltr_buf_ptr->ip_hdr.v4.field_mask );	
    if (IPFLTR_MASK_IP4_SRC_ADDR & fltr_buf_ptr->ip_hdr.v4.field_mask)
    {
      DS_MSG2(MSG_LEGACY_HIGH, "src ip addr: %u, src subnet mask: %u",
              ps_ntohl(fltr_buf_ptr->ip_hdr.v4.src.addr.ps_s_addr),
              ps_ntohl(fltr_buf_ptr->ip_hdr.v4.src.subnet_mask.ps_s_addr));
    }

    if (IPFLTR_MASK_IP4_DST_ADDR & fltr_buf_ptr->ip_hdr.v4.field_mask)
    {
      DS_MSG2(MSG_LEGACY_HIGH, "dst ip addr: %u, dst subnet mask: %u",
              ps_ntohl(fltr_buf_ptr->ip_hdr.v4.dst.addr.ps_s_addr),
              ps_ntohl(fltr_buf_ptr->ip_hdr.v4.dst.subnet_mask.ps_s_addr));
    }

    if (IPFLTR_MASK_IP4_TOS & fltr_buf_ptr->ip_hdr.v4.field_mask)
    {
      DS_MSG2(MSG_LEGACY_HIGH, "tos value: %d , tos mask: %d",
              (int)fltr_buf_ptr->ip_hdr.v4.tos.val,
              (int)fltr_buf_ptr->ip_hdr.v4.tos.mask);
    }

    if (IPFLTR_MASK_IP4_NEXT_HDR_PROT & fltr_buf_ptr->ip_hdr.v4.field_mask)
    {
      DS_MSG1(MSG_LEGACY_HIGH, "transport protocol: %d",
             (int)fltr_buf_ptr->ip_hdr.v4.next_hdr_prot );

      if ( PS_IPPROTO_TCP == fltr_buf_ptr->ip_hdr.v4.next_hdr_prot)
      {
        if (IPFLTR_MASK_TCP_SRC_PORT & fltr_buf_ptr->next_prot_hdr.tcp.field_mask)
        {
          DS_MSG2(MSG_LEGACY_HIGH, "tcp src start port: %d , port range: %d",
                  (int)ps_ntohs(fltr_buf_ptr->next_prot_hdr.tcp.src.port),
                  (int)fltr_buf_ptr->next_prot_hdr.tcp.src.range);
        }

        if (IPFLTR_MASK_TCP_DST_PORT &  fltr_buf_ptr->next_prot_hdr.tcp.field_mask)
        {
          DS_MSG2(MSG_LEGACY_HIGH, "tcp dst start port: %d , port range: %d",
                  (int)ps_ntohs(fltr_buf_ptr->next_prot_hdr.tcp.dst.port),
                  (int)fltr_buf_ptr->next_prot_hdr.tcp.dst.range);
        }
      }

      if ( PS_IPPROTO_UDP == fltr_buf_ptr->ip_hdr.v4.next_hdr_prot)
      {
        if (IPFLTR_MASK_UDP_SRC_PORT & fltr_buf_ptr->next_prot_hdr.udp.field_mask)
        {
          DS_MSG2(MSG_LEGACY_HIGH, "udp src start port: %d , port range: %d",
                  (int)ps_ntohs(fltr_buf_ptr->next_prot_hdr.udp.src.port),
                  (int)fltr_buf_ptr->next_prot_hdr.udp.src.range);
        }

        if (IPFLTR_MASK_UDP_DST_PORT &  fltr_buf_ptr->next_prot_hdr.udp.field_mask)
        {
          DS_MSG2(MSG_LEGACY_HIGH, "udp dst start port: %d , port range: %d",
                  (int)ps_ntohs(fltr_buf_ptr->next_prot_hdr.udp.dst.port),
                  (int)fltr_buf_ptr->next_prot_hdr.udp.dst.range);
        }
      }

      if ( PS_IPPROTO_TCP_UDP == fltr_buf_ptr->ip_hdr.v4.next_hdr_prot)
      {
        if (IPFLTR_MASK_TCP_UDP_SRC_PORT & fltr_buf_ptr->next_prot_hdr.tcp_udp_port_range.field_mask)
        {
          DS_MSG2(MSG_LEGACY_HIGH,
                  "tcp+udp src start port: %d , port range: %d",
                  (int)ps_ntohs(fltr_buf_ptr->next_prot_hdr.tcp_udp_port_range.src.port),
                  (int)fltr_buf_ptr->next_prot_hdr.tcp_udp_port_range.src.range);
        }

        if (IPFLTR_MASK_TCP_UDP_DST_PORT & fltr_buf_ptr->next_prot_hdr.tcp_udp_port_range.field_mask)
        {
          DS_MSG2(MSG_LEGACY_HIGH,
                  "tcp+udp dst start port: %d , port range: %d",
                  (int)ps_ntohs(fltr_buf_ptr->next_prot_hdr.tcp_udp_port_range.dst.port),
                  (int)fltr_buf_ptr->next_prot_hdr.tcp_udp_port_range.dst.range);
        }
      }
	  
   	  if ( PS_IPPROTO_ESP == fltr_buf_ptr->ip_hdr.v4.next_hdr_prot)
   	  {
   	    if (IPFLTR_MASK_ESP_SPI & fltr_buf_ptr->next_prot_hdr.esp.field_mask)
        {
          DS_MSG1(MSG_LEGACY_HIGH, "esp spi: 0x%x",
                  fltr_buf_ptr->next_prot_hdr.esp.spi);
        }
      }
      if ( PS_IPPROTO_AH == fltr_buf_ptr->ip_hdr.v4.next_hdr_prot)
      {
        if (IPFLTR_MASK_AH_SPI & fltr_buf_ptr->next_prot_hdr.ah.field_mask)
        {
          DS_MSG1(MSG_LEGACY_HIGH, "ah spi: 0x%x",
                  fltr_buf_ptr->next_prot_hdr.ah.spi);
        }
      } 
    }
  }
  else if( IP_V6 == fltr_buf_ptr->ip_vsn ) 
  {
    DS_MSG1(MSG_LEGACY_HIGH, "filter param mask: 0x%x",
           (int)fltr_buf_ptr->ip_hdr.v6.field_mask );

    if (IPFLTR_MASK_IP6_SRC_ADDR & fltr_buf_ptr->ip_hdr.v6.field_mask)
    {
      DS_MSG5(MSG_LEGACY_HIGH, 
              "src ipv6 addr[0]: %u, src ipv6 addr[1]: %u, "
              "src ipv6 addr[2]: %u, src ipv6 addr[3]: %u, "
              "src ipv6 prefix len: %d",
              ((unsigned int*)&fltr_buf_ptr->ip_hdr.v6.src.addr)[0],
              ((unsigned int*)&fltr_buf_ptr->ip_hdr.v6.src.addr)[1],
              ((unsigned int*)&fltr_buf_ptr->ip_hdr.v6.src.addr)[2],
              ((unsigned int*)&fltr_buf_ptr->ip_hdr.v6.src.addr)[3],
              (int)fltr_buf_ptr->ip_hdr.v6.src.prefix_len);
    }

    if (IPFLTR_MASK_IP6_DST_ADDR & fltr_buf_ptr->ip_hdr.v6.field_mask)
    {
      DS_MSG5(MSG_LEGACY_HIGH, 
              "dst ipv6 addr[0]: %u, dst ipv6 addr[1]: %u, "
              "dst ipv6 addr[2]: %u, dst ipv6 addr[3]: %u, "
              "dst ipv6 prefix len: %d",
              ((unsigned int*)&fltr_buf_ptr->ip_hdr.v6.dst.addr)[0],
              ((unsigned int*)&fltr_buf_ptr->ip_hdr.v6.dst.addr)[1],
              ((unsigned int*)&fltr_buf_ptr->ip_hdr.v6.dst.addr)[2],
              ((unsigned int*)&fltr_buf_ptr->ip_hdr.v6.dst.addr)[3],
              (int)fltr_buf_ptr->ip_hdr.v6.dst.prefix_len);
    }

    if (IPFLTR_MASK_IP6_TRAFFIC_CLASS & fltr_buf_ptr->ip_hdr.v6.field_mask)
    {
      DS_MSG2(MSG_LEGACY_HIGH, "trafic class value: %d , mask: %d",
             (int)fltr_buf_ptr->ip_hdr.v6.trf_cls.val,
             (int)fltr_buf_ptr->ip_hdr.v6.trf_cls.mask);
    }

    if (IPFLTR_MASK_IP6_FLOW_LABEL & fltr_buf_ptr->ip_hdr.v6.field_mask)
    {
      DS_MSG1(MSG_LEGACY_HIGH, "flow label: %u", 
              fltr_buf_ptr->ip_hdr.v6.flow_label );
    }

    if (IPFLTR_MASK_IP6_NEXT_HDR_PROT & fltr_buf_ptr->ip_hdr.v6.field_mask)
    {
      DS_MSG1(MSG_LEGACY_HIGH, "transport protocol: %d",
             (int)fltr_buf_ptr->ip_hdr.v6.next_hdr_prot );

      if ( PS_IPPROTO_TCP == fltr_buf_ptr->ip_hdr.v6.next_hdr_prot)
      {
        if (IPFLTR_MASK_TCP_SRC_PORT & fltr_buf_ptr->next_prot_hdr.tcp.field_mask)
        {
          DS_MSG2(MSG_LEGACY_HIGH, "tcp src start port: %d , port range: %d",
                  (int)ps_ntohs(fltr_buf_ptr->next_prot_hdr.tcp.src.port),
                  (int)fltr_buf_ptr->next_prot_hdr.tcp.src.range);
        }

        if (IPFLTR_MASK_TCP_DST_PORT &  fltr_buf_ptr->next_prot_hdr.tcp.field_mask)
        {
          DS_MSG2(MSG_LEGACY_HIGH, "tcp dst start port: %d , port range: %d",
                  (int)ps_ntohs(fltr_buf_ptr->next_prot_hdr.tcp.dst.port),
                  (int)fltr_buf_ptr->next_prot_hdr.tcp.dst.range);
        }
      }

      if ( PS_IPPROTO_UDP == fltr_buf_ptr->ip_hdr.v6.next_hdr_prot)
      {
        if (IPFLTR_MASK_UDP_SRC_PORT & fltr_buf_ptr->next_prot_hdr.udp.field_mask)
        {
          DS_MSG2(MSG_LEGACY_HIGH, "udp src start port: %d , port range: %d",
                  (int)ps_ntohs(fltr_buf_ptr->next_prot_hdr.udp.src.port),
                  (int)fltr_buf_ptr->next_prot_hdr.udp.src.range);
        }
        
        if (IPFLTR_MASK_UDP_DST_PORT &  fltr_buf_ptr->next_prot_hdr.udp.field_mask)
        {
          DS_MSG2(MSG_LEGACY_HIGH, "udp dst start port: %d , port range: %d",
                  (int)ps_ntohs(fltr_buf_ptr->next_prot_hdr.udp.dst.port),
                  (int)fltr_buf_ptr->next_prot_hdr.udp.dst.range);
        }
      }

      if ( PS_IPPROTO_TCP_UDP == fltr_buf_ptr->ip_hdr.v6.next_hdr_prot)
      {
        if (IPFLTR_MASK_TCP_UDP_SRC_PORT & fltr_buf_ptr->next_prot_hdr.tcp_udp_port_range.field_mask)
        {
          DS_MSG2(MSG_LEGACY_HIGH,
                  "tcp+udp src start port: %d , port range: %d",
                  (int)ps_ntohs(fltr_buf_ptr->next_prot_hdr.tcp_udp_port_range.src.port),
                  (int)fltr_buf_ptr->next_prot_hdr.tcp_udp_port_range.src.range);
        }
        
        if (IPFLTR_MASK_TCP_UDP_DST_PORT & fltr_buf_ptr->next_prot_hdr.tcp_udp_port_range.field_mask)
        {
          DS_MSG2(MSG_LEGACY_HIGH,
                  "tcp+udp dst start port: %d , port range: %d",
                  (int)ps_ntohs(fltr_buf_ptr->next_prot_hdr.tcp_udp_port_range.dst.port),
                  (int)fltr_buf_ptr->next_prot_hdr.tcp_udp_port_range.dst.range);
        }
      }

      if ( PS_IPPROTO_ESP == fltr_buf_ptr->ip_hdr.v6.next_hdr_prot)
      {
        if (IPFLTR_MASK_ESP_SPI & fltr_buf_ptr->next_prot_hdr.esp.field_mask)
        {
          DS_MSG1(MSG_LEGACY_HIGH, "esp spi: 0x%x",
                  fltr_buf_ptr->next_prot_hdr.esp.spi);
        }
      }  
      if ( PS_IPPROTO_AH == fltr_buf_ptr->ip_hdr.v6.next_hdr_prot)
      {
        if (IPFLTR_MASK_AH_SPI & fltr_buf_ptr->next_prot_hdr.ah.field_mask)
        {
          DS_MSG1(MSG_LEGACY_HIGH, "ah spi: 0x%x",
                  fltr_buf_ptr->next_prot_hdr.ah.spi);
        }
      }      
    }
  }

  return;  
} /* ds_flow_log_qos_spec_filter_params */

/*===========================================================================
FUNCTION ds_flow_log_qos_spec_params

DESCRIPTION
This function printf the qos spec params

PARAMETERS
None.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
void ds_flow_log_qos_spec_params
(
 const qos_spec_type*         qos_spec
)
{
  int i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == qos_spec)
  {
    return;
  }

  DS_MSG2(MSG_LEGACY_HIGH, 
          "ds_flow_log_qos_spec_params(): obj 0x%p qos spec field mask: 0x%x",
          qos_spec, qos_spec->field_mask );

  if (qos_spec->rx.fltr_template.num_filters)
  {
    DS_MSG1(MSG_LEGACY_HIGH, "qos spec contains %d RX filters",
            qos_spec->rx.fltr_template.num_filters );
    for (i = 0; i < qos_spec->rx.fltr_template.num_filters; i++)
    {
      ds_flow_log_qos_spec_filter_params(&qos_spec->rx.fltr_template.list_ptr[i]);
    }
  }

  if (qos_spec->tx.fltr_template.num_filters)
  {
   DS_MSG1(MSG_LEGACY_HIGH, "qos spec contains %d TX filters",
           qos_spec->tx.fltr_template.num_filters );
    for (i = 0; i < qos_spec->tx.fltr_template.num_filters; i++)
    {
      ds_flow_log_qos_spec_filter_params(&qos_spec->tx.fltr_template.list_ptr[i]);
    }
  }
} /* ds_flow_log_qos_spec_params */

/*===========================================================================
FUNCTION DS_FLOW_VALIDATE_QOS_FLOW_PARAM()

DESCRIPTION
  this function is used to validate the parameters of a QOS flow

PARAMETERS
  ip_flow_ptr : ptr to the requested QOS flow
  flow_type   : enum stating Rx or Tx flow type

RETURN VALUE
  TRUE  : if parameters are valid
  FALSE : otherwise

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
boolean ds_flow_validate_qos_flow_param
(
  ip_flow_type                   *ip_flow_ptr,
  qos_spec_field_mask_enum_type   flow_type
)
{
  boolean  is_flow_valid = TRUE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ip_flow_ptr->err_mask = 0;

  if (ip_flow_ptr->field_mask == IPFLOW_MASK_NONE)
  {
    DS_ERROR_LOG_2("Invalid IP_FLOW field mask: =x in QOS flow type: =x",
                   ip_flow_ptr->field_mask, flow_type);
    ip_flow_ptr->err_mask = IPFLOW_MASK_ALL;
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    If a bit mask is specified, which is outside the supported range, flag
    error and continue with validation so that other parameters are validated
  -------------------------------------------------------------------------*/
  if (ip_flow_ptr->field_mask & ~IPFLOW_MASK_ALL)
  {
    ip_flow_ptr->err_mask = (ip_flow_ptr->field_mask & ~IPFLOW_MASK_ALL);
    is_flow_valid = FALSE;
  }

  /*-------------------------------------------------------------------------
    For enums make sure that field in between MIN and MAX values. For
    others make sure that field's value is not 0 if specified in field mask.
    Currently these fields are unsigned so bounds checking MIN causes
    compilation warnings. Validation will need to be re-added here for the
    MIN values if they are increased to anything above 0.
  -------------------------------------------------------------------------*/
  if ((ip_flow_ptr->field_mask & IPFLOW_MASK_TRF_CLASS) &&
      (ip_flow_ptr->trf_class >= IP_TRF_CLASS_MAX))
  {
    ip_flow_ptr->err_mask |= IPFLOW_MASK_TRF_CLASS;
    is_flow_valid = FALSE;
  }

  if (ip_flow_ptr->field_mask & IPFLOW_MASK_DATA_RATE)
  {
    if (ip_flow_ptr->data_rate.format_type == DATA_RATE_FORMAT_MIN_MAX_TYPE)
    {
      if (  ip_flow_ptr->data_rate.format.min_max.guaranteed_rate >
            ip_flow_ptr->data_rate.format.min_max.max_rate)
      {
        ip_flow_ptr->err_mask |= IPFLOW_MASK_DATA_RATE;
        is_flow_valid = FALSE;
      }
    }
    else if (ip_flow_ptr->data_rate.format_type == DATA_RATE_FORMAT_MIN_MAX_EX_TYPE)
    {
      if (  ip_flow_ptr->data_rate.format.min_max_ex.guaranteed_rate >
            ip_flow_ptr->data_rate.format.min_max_ex.max_rate)
      {
        ip_flow_ptr->err_mask |= IPFLOW_MASK_DATA_RATE;
        is_flow_valid = FALSE;
      }
    }
    else if (ip_flow_ptr->data_rate.format_type ==
               DATA_RATE_FORMAT_TOKEN_BUCKET_TYPE)
    {
      if (ip_flow_ptr->data_rate.format.token_bucket.peak_rate == 0 ||
          ip_flow_ptr->data_rate.format.token_bucket.token_rate == 0 ||
          ip_flow_ptr->data_rate.format.token_bucket.size == 0)
      {
        ip_flow_ptr->err_mask |= IPFLOW_MASK_DATA_RATE;
        is_flow_valid = FALSE;
      }
    }
    else
    {
      ip_flow_ptr->err_mask |= IPFLOW_MASK_DATA_RATE;
      is_flow_valid = FALSE;
    }
  }

  if ((ip_flow_ptr->field_mask & IPFLOW_MASK_LATENCY) &&
      ip_flow_ptr->latency == 0)
  {
    ip_flow_ptr->err_mask |= IPFLOW_MASK_LATENCY;
    is_flow_valid = FALSE;
  }

  if ((ip_flow_ptr->field_mask & IPFLOW_MASK_LATENCY_VAR) &&
      ip_flow_ptr->latency_var == 0)
  {
    DS_MSG1(MSG_LEGACY_HIGH, "ds_flow_validate_qos_flow_param(): "
            "Warning latency variance = 0 on flow type 0x%x",
            flow_type);
  }

  if ((ip_flow_ptr->field_mask & IPFLOW_MASK_MAX_ALLOWED_PKT_SIZE) &&
      ip_flow_ptr->max_allowed_pkt_size == 0)
  {
    ip_flow_ptr->err_mask |= IPFLOW_MASK_MAX_ALLOWED_PKT_SIZE;
    is_flow_valid = FALSE;
  }

  /*-------------------------------------------------------------------------
    Validation will need to be re-added here for the MIN values if they are
    increased to anything above 0.
  -------------------------------------------------------------------------*/
  if ((ip_flow_ptr->field_mask & IPFLOW_MASK_UMTS_RES_BER) &&
      (ip_flow_ptr->umts_params.res_ber >= UMTS_RES_BIT_ERR_RATE_MAX))
  {
    ip_flow_ptr->err_mask |= IPFLOW_MASK_UMTS_RES_BER;
    is_flow_valid = FALSE;
  }

  if ((ip_flow_ptr->field_mask & IPFLOW_MASK_UMTS_TRF_PRI) &&
      (ip_flow_ptr->umts_params.trf_pri >= UMTS_TRF_HANDLING_PRI_MAX))
  {
    ip_flow_ptr->err_mask |= IPFLOW_MASK_UMTS_TRF_PRI;
    is_flow_valid = FALSE;
  }

  if ((ip_flow_ptr->field_mask & IPFLOW_MASK_WLAN_USER_PRI) &&
      (ip_flow_ptr->wlan_params.user_priority >= WLAN_USER_PRI_MAX))
  {
    ip_flow_ptr->err_mask |= IPFLOW_MASK_WLAN_USER_PRI;
    is_flow_valid = FALSE;
  }

  if ((ip_flow_ptr->field_mask & IPFLOW_MASK_WLAN_MIN_SERVICE_INTERVAL) &&
      ip_flow_ptr->wlan_params.min_service_interval == 0)
  {
    ip_flow_ptr->err_mask |= IPFLOW_MASK_WLAN_MIN_SERVICE_INTERVAL;
    is_flow_valid = FALSE;
  }

  if ((ip_flow_ptr->field_mask & IPFLOW_MASK_WLAN_MAX_SERVICE_INTERVAL) &&
      ip_flow_ptr->wlan_params.max_service_interval == 0)
  {
    ip_flow_ptr->err_mask |= IPFLOW_MASK_WLAN_MAX_SERVICE_INTERVAL;
    is_flow_valid = FALSE;
  }

  if ((ip_flow_ptr->field_mask & IPFLOW_MASK_WLAN_INACTIVITY_INTERVAL) &&
      ip_flow_ptr->wlan_params.inactivity_interval == 0)
  {
    ip_flow_ptr->err_mask |= IPFLOW_MASK_WLAN_INACTIVITY_INTERVAL;
    is_flow_valid = FALSE;
  }

  if (ip_flow_ptr->field_mask & IPFLOW_MASK_NOMINAL_SDU_SIZE)
  {
    if (ip_flow_ptr->nominal_sdu_size.size == 0)
    {
      ip_flow_ptr->err_mask |= IPFLOW_MASK_NOMINAL_SDU_SIZE;
      is_flow_valid = FALSE;
    }
    /*-----------------------------------------------------------------------
      Make sure that nominal size is not greater than max packet size if the
      latter is specified. If nominal size is constant, then it should be
      equal to max packet size because packet size is always the same
    -----------------------------------------------------------------------*/
    else if (ip_flow_ptr->field_mask & IPFLOW_MASK_MAX_ALLOWED_PKT_SIZE)
    {
      if (ip_flow_ptr->nominal_sdu_size.is_fixed == TRUE &&
          ip_flow_ptr->nominal_sdu_size.size !=
            ip_flow_ptr->max_allowed_pkt_size)
      {
        ip_flow_ptr->err_mask |=
          IPFLOW_MASK_NOMINAL_SDU_SIZE | IPFLOW_MASK_MAX_ALLOWED_PKT_SIZE;
        is_flow_valid = FALSE;
      }
      else if (ip_flow_ptr->nominal_sdu_size.is_fixed == FALSE &&
               ip_flow_ptr->nominal_sdu_size.size >
                 ip_flow_ptr->max_allowed_pkt_size)
      {
        ip_flow_ptr->err_mask |=
          IPFLOW_MASK_NOMINAL_SDU_SIZE | IPFLOW_MASK_MAX_ALLOWED_PKT_SIZE;
        is_flow_valid = FALSE;
      }
    }
  }

  if (is_flow_valid == FALSE)
  {
    DS_ERROR_LOG_3("ds_flow_validate_qos_flow_param(): "
                   "IP_FLOW field_mask = =x, Type of FLOW processed: =x, "
                   "IP_FLOW err_mask = =x",
                   ip_flow_ptr->field_mask, flow_type, ip_flow_ptr->err_mask);
  }

  return is_flow_valid;
} /* ds_flow_validate_qos_flow_param() */

/*===========================================================================
FUNCTION DS_FLOW_VALIDATE_FLTR_SPEC()

DESCRIPTION
  Validates fltr spec in one direction

PARAMETERS
  fltr_spec_ptr       : Address where fltr spec is stored. Could be rx or tx
                        from QOS_REQUEST or QOS_MODIFY
  addr_family   : addr family of iface to which this QoS request
                        belongs to
  flow_type           : Specifies direction of flow spec
  is_fltr_spec_valid  : OUT PARAM specifying if validation is successful

RETURN VALUE
  boolean - TRUE if validation succeeds, FALSE otherwise.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_flow_validate_fltr_spec
(
  const ip_filter_spec_type       *fltr_spec_ptr,
  qos_spec_field_mask_enum_type    flow_type
)
{
  ds_ipfltr_client_id_enum_type    client_id = FLTR_CLIENT_MAX;
  ds_local_err_val_e_type          local_err = DS_LOCAL_ERR_VAL_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
    Validate that number of filters is in between (0, MAX_FLTR_PER_REQ]
  -----------------------------------------------------------------------*/
  if (fltr_spec_ptr->num_filters == 0)
  {
    local_err = DS_LOCAL_ERR_VAL_0;
    goto func_exit;
  }
  else if (fltr_spec_ptr->num_filters > MAX_FLTR_PER_REQ)
  {
    local_err = DS_LOCAL_ERR_VAL_1;
    goto func_exit;
  }
  else if (fltr_spec_ptr->list_ptr == NULL)
  {
    local_err = DS_LOCAL_ERR_VAL_2;
    goto func_exit;
  }

  /*-------------------------------------------------------------------------
    Find out IP fltr client based on flow_type
  -------------------------------------------------------------------------*/
  if (flow_type == QOS_MASK_TX_FLOW)
  {
    client_id = FLTR_CLIENT_QOS_OUTPUT;
  }
  else if (flow_type == QOS_MASK_RX_FLOW)
  {
    client_id = FLTR_CLIENT_QOS_INPUT;
  }
  else
  {
    local_err = DS_LOCAL_ERR_VAL_3;
    DS_ASSERT(0);
    goto func_exit;
  }

  if(ds_ipfltr_validate_fltr_param(client_id,
                                   fltr_spec_ptr->list_ptr,
                                   fltr_spec_ptr->num_filters) == FALSE)
  {
    local_err = DS_LOCAL_ERR_VAL_4;
    goto func_exit;
  }

func_exit:
  if(local_err != DS_LOCAL_ERR_VAL_NONE)
  {
    DS_ERROR_LOG_1("ds_flow_validate_fltr_spec failed with err =d",
                   local_err);
    return FALSE;
  }

  return TRUE;
} /* ds_flow_validate_fltr_spec() */

/*===========================================================================
FUNCTION DS_FLOW_VALIDATE_QOS_SPEC()

DESCRIPTION
  This function validates the requested QOS spec. Both flow spec and filter
  spec are validated in each direction. 

PARAMETERS
  qos_spec_type

RETURN VALUE
   boolean - TRUE if validation succeeds, FALSE otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean ds_flow_validate_qos_spec
(
  const qos_spec_type              *qos_spec
)
{
  boolean                     is_qos_spec_valid = TRUE;
  ds_local_err_val_e_type     local_err = DS_LOCAL_ERR_VAL_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(!qos_spec)
  {
    local_err = DS_LOCAL_ERR_VAL_0;
    goto func_exit;
  }

  /*-------------------------------------------------------------------------
    One of Rx or Tx flow specs must be specified
  -------------------------------------------------------------------------*/
  if (!(qos_spec->field_mask & QOS_MASK_RX_FLOW) &&
      !(qos_spec->field_mask & QOS_MASK_TX_FLOW))
  {
    local_err = DS_LOCAL_ERR_VAL_1;
    goto func_exit;
  }

  /*-------------------------------------------------------------------------
    If a bit mask is specified, which is outside the supported range, flag
    error
  -------------------------------------------------------------------------*/
  if (qos_spec->field_mask & ~QOS_MASK_ALL)
  {
    local_err = DS_LOCAL_ERR_VAL_2;
    goto func_exit;
  }

  /*-------------------------------------------------------------------------
    Validate Rx flow spec and Rx fltr spec
  -------------------------------------------------------------------------*/
  if (qos_spec->field_mask & QOS_MASK_RX_FLOW)
  {
    /*-----------------------------------------------------------------------
      Validate Rx required flow spec
    -----------------------------------------------------------------------*/
    is_qos_spec_valid &= ds_flow_validate_qos_flow_param(
                         (ip_flow_type *)(&(qos_spec->rx.flow_template.req_flow)),
                                         QOS_MASK_RX_FLOW);

    /*-----------------------------------------------------------------------
      Validate Rx minimum required flow spec
    -----------------------------------------------------------------------*/
    if (qos_spec->field_mask & QOS_MASK_RX_MIN_FLOW)
    {
      is_qos_spec_valid &= ds_flow_validate_qos_flow_param(
                           (ip_flow_type *)(&(qos_spec->rx.flow_template.min_req_flow)),
                                     QOS_MASK_RX_MIN_FLOW);
    }

    /*-----------------------------------------------------------------------
      Validate Rx fltr spec
    -----------------------------------------------------------------------*/
    if (ds_flow_validate_fltr_spec(&(qos_spec->rx.fltr_template),
                                   QOS_MASK_RX_FLOW) == FALSE)
    {
      local_err = DS_LOCAL_ERR_VAL_3;
      goto func_exit;
    }
  }
  else
  {
    /*-----------------------------------------------------------------------
      Validate that RX_MIN_FLOW or RX_AUXILIARY_FLOWS is not specified. It is
      mandatory to specify required flow spec if app wants to use QOS in a
      direction
    -----------------------------------------------------------------------*/
    if(qos_spec->field_mask & QOS_MASK_RX_MIN_FLOW)
    {
      local_err = DS_LOCAL_ERR_VAL_4;
      goto func_exit;
    }
  }

  /*-------------------------------------------------------------------------
    Validate Tx flow spec and Tx fltr spec
  -------------------------------------------------------------------------*/
  if (qos_spec->field_mask & QOS_MASK_TX_FLOW)
  {
    /*-----------------------------------------------------------------------
      Validate Tx required flow spec
    -----------------------------------------------------------------------*/
    is_qos_spec_valid &= ds_flow_validate_qos_flow_param(
                          (ip_flow_type *)(&(qos_spec->tx.flow_template.req_flow)),
                                         QOS_MASK_TX_FLOW);

    /*-----------------------------------------------------------------------
      Validate Tx minimum required flow spec
    -----------------------------------------------------------------------*/
    if (qos_spec->field_mask & QOS_MASK_TX_MIN_FLOW)
    {
      is_qos_spec_valid &= ds_flow_validate_qos_flow_param(
                            (ip_flow_type *)(&(qos_spec->tx.flow_template.min_req_flow)),
                                     QOS_MASK_TX_MIN_FLOW);
    }

    /*-----------------------------------------------------------------------
      Validate Tx fltr spec
    -----------------------------------------------------------------------*/
    if (ds_flow_validate_fltr_spec(&(qos_spec->tx.fltr_template),
                                   QOS_MASK_TX_FLOW) == FALSE)
    {
      local_err = DS_LOCAL_ERR_VAL_5;
      goto func_exit;
    }
  }
  else /* if TX_FLOW is not specified */
  {
    /*-----------------------------------------------------------------------
      Validate that TX_MIN_FLOW or TX_AUXILIARY_FLOWS is not specified. It is
      mandatory to specify required flow spec if app wants to use QOS in a
      direction
    -----------------------------------------------------------------------*/
    if (qos_spec->field_mask & QOS_MASK_TX_MIN_FLOW)
    {
      local_err = DS_LOCAL_ERR_VAL_6;
      goto func_exit;
    }
  }

func_exit:  
  if(local_err != DS_LOCAL_ERR_VAL_NONE)
  {
    DS_ERROR_LOG_2("ds_flow_validate_qos_spec failed with err =d "
                   "is_qos_spec_valid: =d",
                   local_err, is_qos_spec_valid);
  }

  if (is_qos_spec_valid == FALSE)
  {
    return FALSE;
  }

  return TRUE;
} /* ds_flow_validate_qos_spec() */

/*===========================================================================
FUNCTION DS_FLOW_CREATE_FLOW_CONTEXT

DESCRIPTION
  This function creates a flow, processes the requested QOS spec and stores
  it in the flow control block for later use. Both flow spec and filter spec
  are processed and validated for each direction. Both Rx and Tx flow specs are
  stored as is after making sure all specified parameters are correct. Tx
  filter spec is validated and stored in decreasing order of fltr precedence.
  Tx filter spec is passed to IP filtering library, where it is sorted and stored 
  to be applied to the outgoing IP traffic.

PARAMETERS


RETURN VALUE
   boolean - TRUE if creation passes, FALSE otherwise

DEPENDENCIES
  None.

SIDE EFFECTS
  None
===========================================================================*/
boolean ds_flow_create_flow_context
(
 ds_flow_type               *flow_context,
 qos_spec_type              *qos_spec,
 ds_pdn_mgr_s               *pdn_mgr_p,
 ds_pdn_mgr_instance_type_e  inst_type,
 ds_bearer_mgr_s            *bearer_mgr_p
)
{
  ds_ipfltr_add_param_type     fltr_add_param = {0};
  ds_ipfltr_delete_param_type  fltr_delete_param = {0};
  ds_pdn_mgr_inst_type_s      *inst_p = NULL;
  ds_flow_type                *tmp_flow_ptr = NULL;
  ds_flow_type                *prev_flow_ptr = NULL;
  uint8                        flow_cookie = 0;
  int16                        ds_errno = DS_ENOERR;
  
  if(!ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p) || 
     !ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p) ||
     !ds_flow_is_valid(flow_context) || !qos_spec)
  {
    return FALSE;
  }

  inst_p = pdn_mgr_p->inst_ptr[inst_type];
  if(!inst_p || !inst_p->flow)
  {
    return FALSE;
  }

  DS_MSG2(MSG_LEGACY_HIGH, "ds_flow_create_flow_context for PDN %d inst %d",
          pdn_mgr_p->index, inst_type);

  // Log QoS params
  ds_flow_log_qos_spec_params(qos_spec);

  // Validate QoS params
  if(ds_flow_validate_qos_spec(qos_spec) == FALSE)
  {
    return FALSE;
  }

  // Store QoS spec is not required as spec is stored in flow_context directly.

  /*-------------------------------------------------------------------------
    Save TX filter spec in FWK if Tx flow spec is specified
  -------------------------------------------------------------------------*/
  if (qos_spec->field_mask & QOS_MASK_TX_FLOW)
  {
    fltr_add_param.client_id = FLTR_CLIENT_QOS_OUTPUT;
    fltr_add_param.fi_ptr_arr = qos_spec->tx.fltr_template.list_ptr;    
    fltr_add_param.num_filters = qos_spec->tx.fltr_template.num_filters;
    fltr_add_param.fi_result = (ds_ipfltr_result_type)flow_context;
    fltr_add_param.subset_id = 5;
    fltr_add_param.fltr_priority = DS_IPFLTR_PRIORITY_FCFS;
    fltr_add_param.is_validated = TRUE;
    fltr_add_param.fw_inst_ptr = ds_fwk_get_inst_ptr_from_index(pdn_mgr_p->fw_index);
    qos_spec->tx.fltr_template.fltr_handle = ds_ipfltr_add(&fltr_add_param, 
                                                           &ds_errno);
    if(qos_spec->tx.fltr_template.fltr_handle == DS_IPFLTR_INVALID_HANDLE)
    {
      DS_ERROR_LOG_1("TX filter addition failed with err =d", ds_errno);
      ds_flow_delete(flow_context);
      return FALSE;
    }
  }

  /*-------------------------------------------------------------------------
    Save RX filter spec in FWK if Rx flow spec is specified
  -------------------------------------------------------------------------*/
  if (qos_spec->field_mask & QOS_MASK_RX_FLOW)
  {
    qos_spec->rx.fltr_template.fltr_handle = 
    	         ds_fwk_rx_qos_fltr_add(ds_fwk_get_inst_ptr_from_index(
    	                                pdn_mgr_p->fw_index),
    	                                qos_spec,
    	                                NULL,
                                                           &ds_errno);
    if(qos_spec->rx.fltr_template.fltr_handle == DS_IPFLTR_INVALID_HANDLE)
    {
      DS_ERROR_LOG_1("RX filter addition failed with err =d", ds_errno);

      if(qos_spec->tx.fltr_template.fltr_handle != DS_IPFLTR_INVALID_HANDLE)
      {
        fltr_delete_param.fw_inst_ptr = ds_fwk_get_inst_ptr_from_index(
        	                                               pdn_mgr_p->fw_index);
        fltr_delete_param.client_id = FLTR_CLIENT_QOS_OUTPUT;
        fltr_delete_param.fi_handle = qos_spec->tx.fltr_template.fltr_handle;
        ds_ipfltr_delete(&fltr_delete_param, &ds_errno);
      }
      
      ds_flow_delete(flow_context);      
      return FALSE;
    }
  }  

  if(inst_p->flow->default_flow_ptr == flow_context)
  {
    // No further action for default flow
    return TRUE;
  }

  if(inst_p->flow->num_sec_flows == DS_MAX_SEC_FLOWS)
  {
    DS_ERROR_LOG_1("Max flows =d already present", DS_MAX_SEC_FLOWS);

    if(qos_spec->tx.fltr_template.fltr_handle != DS_IPFLTR_INVALID_HANDLE)
    {
      fltr_delete_param.fw_inst_ptr = ds_fwk_get_inst_ptr_from_index(
                                                       pdn_mgr_p->fw_index);
      fltr_delete_param.client_id = FLTR_CLIENT_QOS_OUTPUT;
      fltr_delete_param.fi_handle = qos_spec->tx.fltr_template.fltr_handle;
      ds_ipfltr_delete(&fltr_delete_param, &ds_errno);
    }

    if(qos_spec->rx.fltr_template.fltr_handle != DS_IPFLTR_INVALID_HANDLE)
    {
      ds_fwk_rx_qos_fltr_delete(ds_fwk_get_inst_ptr_from_index(pdn_mgr_p->fw_index),
                                qos_spec->rx.fltr_template.fltr_handle);
    }
    
    ds_flow_delete(flow_context);      
    return FALSE;
  }
  
  /*-------------------------------------------------------------------------
    Insert in to the list of secondary flows. Insertion sort is used to keep
    the list sorted

    Algorithm
      1. Always increment instance when a flow is added. Don't slide it back
         when a flow is deleted.
      2. Make sure that "flow_inst" is already used up. If it is, increment
         "flow_inst" and start searching again.
      3. Handle wrap around by resetting "flow_inst" to 1 and search from
         begining of list
  -------------------------------------------------------------------------*/
  tmp_flow_ptr = list_peek_front(&(inst_p->flow->sec_flow_list));
  prev_flow_ptr = NULL;
  flow_cookie = inst_p->flow->next_cookie;

  /*-------------------------------------------------------------------------
    This won't run in to infinite loop since DS_MAX_SEC_FLOWS is <=
    DS_MAX_SEC_FLOW_INST, i.e there is a hole in the instance space
  -------------------------------------------------------------------------*/
  while(tmp_flow_ptr != NULL &&
        tmp_flow_ptr->cookie <= flow_cookie)
  {
    if(tmp_flow_ptr->cookie == flow_cookie)
    {
      flow_cookie++;
      if (flow_cookie > DS_MAX_SEC_FLOW_INST)
      {
        flow_cookie = DS_MIN_SEC_FLOW_INST;
        tmp_flow_ptr = list_peek_front(&(inst_p->flow->sec_flow_list));
        prev_flow_ptr = NULL;
      }
    }
    else
    {
      prev_flow_ptr = tmp_flow_ptr;
      tmp_flow_ptr = list_peek_next(&(inst_p->flow->sec_flow_list),
                                    &(tmp_flow_ptr->link));
    }
  }

  if (prev_flow_ptr == NULL)
  {
    list_push_front(&(inst_p->flow->sec_flow_list),
                    &(flow_context->link));
  }
  else
  {
    list_push_after(&(inst_p->flow->sec_flow_list),
                    &(flow_context->link),
                    &(prev_flow_ptr->link));
  }

  flow_context->cookie = flow_cookie;
  if (inst_p->flow->next_cookie == DS_MAX_SEC_FLOW_INST)
  {
    inst_p->flow->next_cookie = DS_MIN_SEC_FLOW_INST;
  }
  else
  {
    inst_p->flow->next_cookie++;
  }

  inst_p->flow->num_sec_flows++;  

  // Bind bearer with the flow
  flow_context->bearer_ptr = bearer_mgr_p;
  
  // Check NIQ requests - need to check if FWK will be doing this, QOS avail event

  // Log packet


  DS_MSG1(MSG_LEGACY_HIGH, "Flow 0x%x created successfully !", flow_context);  
  return TRUE;
}/* ds_flow_create_flow_context */

/*===========================================================================
FUNCTION DS_FLOW_VALIDATE_QOS_MODIFY_SPEC()

DESCRIPTION
  This function validates the modified QOS spec. If modified, flow spec and
  filter spec are validated in each direction. 

PARAMETERS
  flow_context
  qos_spec_type

RETURN VALUE
   boolean - TRUE if validation succeeds, FALSE otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean ds_flow_validate_qos_modify_spec
(
  ds_flow_type                     *flow_context_p,
  const qos_spec_type              *qos_spec_ptr
)
{
  qos_spec_field_mask_type    field_mask;
  qos_spec_field_mask_type    qos_modify_mask;  
  boolean                     is_qos_spec_valid = FALSE;
  ds_local_err_val_e_type     local_err = DS_LOCAL_ERR_VAL_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(!qos_spec_ptr || !ds_flow_is_valid(flow_context_p))
  {
    local_err = DS_LOCAL_ERR_VAL_0;
    goto func_exit;
  }

  /*-------------------------------------------------------------------------
    Validate that QOS_MODIFY is well formed. Rules are
      1. Some operation must be performed on atleast one of four blocks,
         i.e Tx flow, Tx fltr, Rx flow, and Rx fltr
      2. Reserved bits in qos_modify_mask must not be turned on
      3. If Tx flow is deleted, ensure that it was requested earlier. Also
         ensure that Tx min flow and Tx auxiliary flows are not specified.
         Also ensure that Rx flow either was requested earlier or is
         requested now
      4. If Tx fltrs are modified, make sure that Tx flow was requested
         earlier
      5. Rules 3, and 4 apply to Rx flow and Rx fltr also
      6. Both Tx flow and Rx flow can't be deleted
  -------------------------------------------------------------------------*/

  if (qos_spec_ptr->field_mask == QOS_MASK_INVALID)
  {
    local_err = DS_LOCAL_ERR_VAL_1;
    goto func_exit;
  }

  if (qos_spec_ptr->field_mask & ~QOS_MODIFY_MASK_ALL)
  {
    local_err = DS_LOCAL_ERR_VAL_2;
    goto func_exit;
  }

  field_mask = flow_context_p->qos_info_ptr->field_mask;

  /*-------------------------------------------------------------------------
    Validate Rule 3 for Tx flow spec
  -------------------------------------------------------------------------*/
  if (qos_spec_ptr->field_mask & QOS_MASK_TX_FLOW)
  {
    if (qos_spec_ptr->tx.flow_template.req_flow.field_mask ==
          IPFLOW_MASK_NONE)   /* if Tx flow is deleted */
    {
      if (!(field_mask & QOS_MASK_TX_FLOW))
      {
        local_err = DS_LOCAL_ERR_VAL_3;
        goto func_exit;
      }

      if (qos_spec_ptr->field_mask &
            (QOS_MASK_TX_MIN_FLOW | QOS_MASK_TX_AUXILIARY_FLOWS))
      {
        local_err = DS_LOCAL_ERR_VAL_4;
        goto func_exit;
      }

      /*---------------------------------------------------------------------
        A QOS spec must have flow spec in at least one direction. So flag
        error if Rx flow is not requested now and wasn't requested earlier.
        Also flag error if Rx flow is also deleted.
      ---------------------------------------------------------------------*/
      if ((!(field_mask & QOS_MASK_RX_FLOW) &&
           !(qos_spec_ptr->field_mask & QOS_MASK_RX_FLOW)) ||
           ((qos_spec_ptr->field_mask & QOS_MASK_RX_FLOW) &&
            (qos_spec_ptr->rx.flow_template.req_flow.field_mask ==
                IPFLOW_MASK_NONE)))
      {
        local_err = DS_LOCAL_ERR_VAL_5;
        goto func_exit;
      }

      if (qos_spec_ptr->field_mask & QOS_MODIFY_MASK_TX_FLTR_MODIFY)
      {
        local_err = DS_LOCAL_ERR_VAL_6;
        goto func_exit;
      }
    } /* if Tx flow is deleted */
  } /* Some operation is performed on Tx flow spec */

  /*-------------------------------------------------------------------------
    Validate Rule 4 for Tx fltr spec
  -------------------------------------------------------------------------*/
  if (qos_spec_ptr->field_mask & QOS_MODIFY_MASK_TX_FLTR_MODIFY)
  {
    if (!(field_mask & QOS_MASK_TX_FLOW))
    {
      local_err = DS_LOCAL_ERR_VAL_7;
      goto func_exit;
    }
  }

  /*-------------------------------------------------------------------------
    Validate Rule 3 for Rx flow spec
  -------------------------------------------------------------------------*/
  if (qos_spec_ptr->field_mask & QOS_MASK_RX_FLOW)
  {
    if (qos_spec_ptr->rx.flow_template.req_flow.field_mask ==
          IPFLOW_MASK_NONE)   /* if Rx flow is deleted */
    {
      if (!(field_mask & QOS_MASK_RX_FLOW))
      {
        local_err = DS_LOCAL_ERR_VAL_8;
        goto func_exit;
      }

      if (qos_spec_ptr->field_mask &
            (QOS_MASK_RX_MIN_FLOW | QOS_MASK_RX_AUXILIARY_FLOWS))
      {
        local_err = DS_LOCAL_ERR_VAL_9;
        goto func_exit;
      }

      /*---------------------------------------------------------------------
        A QOS spec must have flow spec in at least one direction. So flag
        error if Tx flow is not requested now and wasn't requested earlier.
        Also flag error if Tx flow is also deleted.
      ---------------------------------------------------------------------*/
      if ((!(field_mask & QOS_MASK_TX_FLOW) &&
           !(qos_spec_ptr->field_mask & QOS_MASK_TX_FLOW)) ||
           ((qos_spec_ptr->field_mask & QOS_MASK_TX_FLOW) &&
            (qos_spec_ptr->tx.flow_template.req_flow.field_mask ==
                IPFLOW_MASK_NONE)))
      {
        local_err = DS_LOCAL_ERR_VAL_10;
        goto func_exit;
      }

      if (qos_spec_ptr->field_mask & QOS_MODIFY_MASK_RX_FLTR_MODIFY)
      {
        local_err = DS_LOCAL_ERR_VAL_11;
        goto func_exit;
      }
    } /* if Rx flow is deleted */
  } /* Some operation is performed on Rx flow spec */

  /*-------------------------------------------------------------------------
    Validate Rule 4 for Rx fltr spec
  -------------------------------------------------------------------------*/
  if (qos_spec_ptr->field_mask & QOS_MODIFY_MASK_RX_FLTR_MODIFY)
  {
    if (!(field_mask & QOS_MASK_RX_FLOW))
    {
      local_err = DS_LOCAL_ERR_VAL_12;
      goto func_exit;
    }
  }

  qos_modify_mask = qos_spec_ptr->field_mask;
  is_qos_spec_valid = TRUE;
   
   /*-------------------------------------------------------------------------
     Validate Rx QOS spec, if it is added/modified. Setting QOS_MASK_RX_FLOW
     and field_mask to 0 indicates that app doesn't want Rx QOS any more
   -------------------------------------------------------------------------*/
   if (qos_modify_mask & QOS_MASK_RX_FLOW)
   {
     if (qos_spec_ptr->rx.flow_template.req_flow.field_mask != IPFLOW_MASK_NONE)
     {
       /*---------------------------------------------------------------------
         Validate Rx required flow spec
       ---------------------------------------------------------------------*/
       is_qos_spec_valid &= ds_flow_validate_qos_flow_param(
                             (ip_flow_type *)&qos_spec_ptr->rx.flow_template.req_flow,
                                   QOS_MASK_RX_FLOW);
   
       /*---------------------------------------------------------------------
         Validate Rx minimum required flow spec
       ---------------------------------------------------------------------*/
       if (qos_modify_mask & QOS_MASK_RX_MIN_FLOW)
       {
         is_qos_spec_valid &= ds_flow_validate_qos_flow_param(
                               (ip_flow_type *)&qos_spec_ptr->rx.flow_template.min_req_flow,
                                   QOS_MASK_RX_MIN_FLOW);
       }
     }
   }
   else /* if RX_FLOW is not specified */
   {
     /*-----------------------------------------------------------------------
       Validate that RX_MIN_FLOW or RX_AUXILIARY_FLOWS is not specified. It is
       mandatory to specify required flow spec if app wants to use QOS in a
       direction
     -----------------------------------------------------------------------*/
     if (qos_modify_mask & QOS_MASK_RX_MIN_FLOW)
     {
       local_err = DS_LOCAL_ERR_VAL_13;
       is_qos_spec_valid = FALSE;
       goto func_exit;
     }   
   }
   
   /*-------------------------------------------------------------------------
     Validate Rx fltr spec if it is either newly added or modified
   -------------------------------------------------------------------------*/
   if (((qos_modify_mask & QOS_MASK_RX_FLOW) &&
        !(field_mask & QOS_MASK_RX_FLOW)) ||
       (qos_modify_mask & QOS_MODIFY_MASK_RX_FLTR_MODIFY))
   {
     if(ds_flow_validate_fltr_spec(&qos_spec_ptr->rx.fltr_template,
                                   QOS_MASK_RX_FLOW) == FALSE)
     {
       local_err = DS_LOCAL_ERR_VAL_14;
       is_qos_spec_valid = FALSE;
       goto func_exit;
     }
   }
   
   /*-------------------------------------------------------------------------
     Validate Tx QOS spec, if it is added/modified. Setting QOS_MASK_TX_FLOW
     and field_mask to 0 indicates that app doesn't want Tx QOS any more
   -------------------------------------------------------------------------*/
   if (qos_modify_mask & QOS_MASK_TX_FLOW)
   {
     if (qos_spec_ptr->tx.flow_template.req_flow.field_mask != IPFLOW_MASK_NONE)
     {
       /*---------------------------------------------------------------------
         Validate Tx required flow spec
       ---------------------------------------------------------------------*/
       is_qos_spec_valid &= ds_flow_validate_qos_flow_param(
                             (ip_flow_type *)&qos_spec_ptr->tx.flow_template.req_flow,
                                   QOS_MASK_TX_FLOW);
      
       /*---------------------------------------------------------------------
         Validate Tx minimum required flow spec
       ---------------------------------------------------------------------*/
       if (qos_modify_mask & QOS_MASK_TX_MIN_FLOW)
       {
         is_qos_spec_valid &= ds_flow_validate_qos_flow_param(
                               (ip_flow_type *)&qos_spec_ptr->tx.flow_template.min_req_flow,
                                   QOS_MASK_TX_MIN_FLOW);
       }   
     } /* if Tx flow spec is not deleted */
   }
   else /* if TX_FLOW is not specified */
   {
     /*-----------------------------------------------------------------------
       Validate that TX_MIN_FLOW or TX_AUXILIARY_FLOWS is not specified. It is
       mandatory to specify required flow spec if app wants to use QOS in a
       direction
     -----------------------------------------------------------------------*/
     if (qos_modify_mask & QOS_MASK_TX_MIN_FLOW)
     {
       local_err = DS_LOCAL_ERR_VAL_15;
       is_qos_spec_valid = FALSE;
       goto func_exit;
     }   
   }
   
   /*-------------------------------------------------------------------------
     Validate Tx fltr spec if it is either newly added or modified
   -------------------------------------------------------------------------*/
   if (((qos_modify_mask & QOS_MASK_TX_FLOW) &&
        !(field_mask & QOS_MASK_TX_FLOW)) ||
       (qos_modify_mask & QOS_MODIFY_MASK_TX_FLTR_MODIFY))
   {
     if(ds_flow_validate_fltr_spec(&qos_spec_ptr->tx.fltr_template,
                                   QOS_MASK_TX_FLOW) == FALSE)
     {
       local_err = DS_LOCAL_ERR_VAL_16;
       is_qos_spec_valid = FALSE;
       goto func_exit;
     }
   }
   
func_exit:  
  if(local_err != DS_LOCAL_ERR_VAL_NONE)
  {
    DS_ERROR_LOG_2("ds_flow_validate_qos_modify_spec failed with err =d "
                   "is_qos_spec_valid: =d",
                   local_err, is_qos_spec_valid);
  }

  if (is_qos_spec_valid == FALSE)
  {
    return FALSE;
  }

  return TRUE;
} /* ds_flow_validate_qos_modify_spec() */

/*===========================================================================
FUNCTION DS_FLOW_MODIFY_FLOW_ACCEPTED

DESCRIPTION
  Indicates that QOS_MODIFY/PRIMARY_QOS_MODIFY is accepted by network.
  If it is QOS_MODIFY, migrates all state associated with QOS_MODIFY to
  ps_flow so that ps_flow represents QOS granted by network and cleans old QOS.

PARAMETERS
  iface_ptr : iface to which flow_ptr belongs
  flow_ptr  : ptr to flow
  info_code : reason for accepting QOS_MODIFY
  ps_errno  : specific error code in case operation fails

RETURN VALUE
   TRUE : on success
  FALSE : on failure

DEPENDENCIES
  None

SIDE EFFECTS
  Previously granted QOS is not valid anymore and FLOW_MODIFY_ACCEPTED_EV is
  posted
===========================================================================*/
boolean ds_flow_modify_flow_accepted
(
  ds_flow_type                     * flow_ptr
)
{
  ds_pdn_mgr_inst_type_s      *inst_p = NULL;
  ds_flow_type                *tmp_flow_ptr = NULL;
  ds_ipfltr_delete_param_type  fltr_delete_param = {0};
  int16                        ds_errno = DS_ENOERR;  
  qos_spec_field_mask_type     qos_modify_mask; /* QOS mask in QOS_MODIFY */
  qos_spec_field_mask_type     field_mask;      /* QOS mask in existing QOS*/
  ds_pdn_event_info_u_type     event_info;
  ds_ipfltr_ctrl_param_type    fltr_ctrl_param = {0};
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!ds_flow_is_valid(flow_ptr))
  {
    return FALSE;
  }

  inst_p = flow_ptr->inst_ptr;
  if (!inst_p)
  {
    return FALSE;
  }

  memset(&event_info,0,sizeof(ds_pdn_event_info_u_type));
  /*-----------------------------------------------------------------------
    Make sure that this secondary flow belongs to this iface
  -----------------------------------------------------------------------*/
  tmp_flow_ptr = list_peek_front(&(inst_p->flow->sec_flow_list));
  while (tmp_flow_ptr != NULL && tmp_flow_ptr != flow_ptr)
  {
    tmp_flow_ptr = list_peek_next(&(inst_p->flow->sec_flow_list),
                                  &(tmp_flow_ptr->link));
  }

  if (tmp_flow_ptr == NULL)
  {
    return FALSE;
  }

  DS_MSG1(MSG_LEGACY_HIGH,"ds_flow_modify_flow_accepted(): "
          "QOS MODIFY ACCEPTED on ps_flow, 0x%p", flow_ptr);

  field_mask = flow_ptr->qos_info_ptr->field_mask;
  qos_modify_mask = flow_ptr->qos_modify_info_ptr->field_mask;
  
  /*-----------------------------------------------------------------------
    Delete filters specified in old QOS if app changed/deleted filter spec
  -----------------------------------------------------------------------*/
  if((qos_modify_mask & QOS_MASK_TX_FLOW &&
      flow_ptr->qos_modify_info_ptr->tx.flow_template.req_flow.field_mask == 
                                                            IPFLOW_MASK_NONE) ||
     (qos_modify_mask & QOS_MODIFY_MASK_TX_FLTR_MODIFY))
  {
    fltr_delete_param.fw_inst_ptr = ds_fwk_get_inst_ptr_from_index(
                                     flow_ptr->bearer_ptr->pdn_mgr_ptr->fw_index);
    fltr_delete_param.client_id = FLTR_CLIENT_QOS_OUTPUT;
    fltr_delete_param.fi_handle = 
                          flow_ptr->qos_info_ptr->tx.fltr_template.fltr_handle;
    ds_ipfltr_delete(&fltr_delete_param, &ds_errno);

    flow_ptr->qos_info_ptr->tx.fltr_template.fltr_handle = 0;
  }

  if((qos_modify_mask & QOS_MASK_RX_FLOW &&
      flow_ptr->qos_modify_info_ptr->rx.flow_template.req_flow.field_mask == 
                                                            IPFLOW_MASK_NONE) ||
     (qos_modify_mask & QOS_MODIFY_MASK_RX_FLTR_MODIFY))
  {
    ds_fwk_rx_qos_fltr_delete(ds_fwk_get_inst_ptr_from_index(
                              flow_ptr->bearer_ptr->pdn_mgr_ptr->fw_index),
                              flow_ptr->qos_info_ptr->rx.fltr_template.fltr_handle);

    flow_ptr->qos_info_ptr->rx.fltr_template.fltr_handle = 0;
  }

  /*-----------------------------------------------------------------------
    Update Tx fltr handle if Tx fltr spec is either newly added or modified.
    Tx fltr spec specified in QOS_MODIFY was added in disabled state. Since
    network accepted QOS_MODIFY, enable these filters
  -----------------------------------------------------------------------*/
  if((qos_modify_mask & QOS_MASK_TX_FLOW &&
      !(field_mask & QOS_MASK_TX_FLOW)) ||
     (qos_modify_mask & QOS_MODIFY_MASK_TX_FLTR_MODIFY))
  {
    DS_ASSERT(flow_ptr->qos_info_ptr->tx.fltr_template.fltr_handle == 0);

    flow_ptr->qos_info_ptr->tx.fltr_template.num_filters = 
                   flow_ptr->qos_modify_info_ptr->tx.fltr_template.num_filters;
    flow_ptr->qos_info_ptr->tx.fltr_template.fltr_handle = 
                   flow_ptr->qos_modify_info_ptr->tx.fltr_template.fltr_handle;

    memscpy(flow_ptr->qos_info_ptr->tx.fltr_template.list_ptr,
            sizeof(ip_filter_type)*MAX_PACKET_FILTERS,
            flow_ptr->qos_modify_info_ptr->tx.fltr_template.list_ptr,
            sizeof(ip_filter_type)*MAX_PACKET_FILTERS);

    // Enable filter
    fltr_ctrl_param.ds_fwk_inst_ptr = ds_fwk_get_inst_ptr_from_index(
                                  flow_ptr->bearer_ptr->pdn_mgr_ptr->fw_index);
    fltr_ctrl_param.enable = TRUE;
    fltr_ctrl_param.client_id = FLTR_CLIENT_QOS_OUTPUT;
    fltr_ctrl_param.fi_handle = 
                      flow_ptr->qos_info_ptr->tx.fltr_template.fltr_handle;
     
    if(ds_ipfltr_control(&fltr_ctrl_param, &ds_errno) == -1)
    {
      DS_ERROR_LOG_0("flow filtering control failed");
    }
  }

   /*-----------------------------------------------------------------------
      Migrate Rx fltr spec from qos_modify_info_ptr to qos_info_ptr if it is
      either newly added or modified
    -----------------------------------------------------------------------*/
  if((qos_modify_mask & QOS_MASK_RX_FLOW &&
      !(field_mask & QOS_MASK_RX_FLOW)) ||
     (qos_modify_mask & QOS_MODIFY_MASK_RX_FLTR_MODIFY))
  {
    DS_ASSERT(flow_ptr->qos_info_ptr->rx.fltr_template.fltr_handle == 0);

    flow_ptr->qos_info_ptr->rx.fltr_template.num_filters = 
                   flow_ptr->qos_modify_info_ptr->rx.fltr_template.num_filters;
    flow_ptr->qos_info_ptr->rx.fltr_template.fltr_handle = 
                   flow_ptr->qos_modify_info_ptr->rx.fltr_template.fltr_handle;
    
    memscpy(flow_ptr->qos_info_ptr->rx.fltr_template.list_ptr,
            sizeof(ip_filter_type)*MAX_PACKET_FILTERS,
            flow_ptr->qos_modify_info_ptr->rx.fltr_template.list_ptr,
            sizeof(ip_filter_type)*MAX_PACKET_FILTERS);
  }

  /*-----------------------------------------------------------------------
    Update Rx flow spec if it is specified in QOS_MODIFY
  -----------------------------------------------------------------------*/
  if (qos_modify_mask & QOS_MASK_RX_FLOW)
  {   
    /*---------------------------------------------------------------------
      Unset QOS field mask. Need this to handle deletion/modification of Rx
      flow spec correctly
    ---------------------------------------------------------------------*/
    flow_ptr->qos_info_ptr->field_mask &= 
                                   ~(QOS_MASK_RX_FLOW | QOS_MASK_RX_MIN_FLOW |
                                    QOS_MASK_RX_AUXILIARY_FLOWS);

    /*---------------------------------------------------------------------
      If app is not deleting Rx flow spec, update Rx flow spec
    ---------------------------------------------------------------------*/
    if (!((qos_modify_mask & QOS_MASK_RX_FLOW) &&
          flow_ptr->qos_modify_info_ptr->rx.flow_template.req_flow.field_mask == 
                                                            IPFLOW_MASK_NONE))
    {
      flow_ptr->qos_info_ptr->field_mask |= QOS_MASK_RX_FLOW;
      flow_ptr->qos_info_ptr->rx.flow_template.req_flow = 
                       flow_ptr->qos_modify_info_ptr->rx.flow_template.req_flow;

      if (qos_modify_mask & QOS_MASK_RX_MIN_FLOW)
      {
        flow_ptr->qos_info_ptr->field_mask |= QOS_MASK_RX_MIN_FLOW;
        flow_ptr->qos_info_ptr->rx.flow_template.min_req_flow =
                   flow_ptr->qos_modify_info_ptr->rx.flow_template.min_req_flow;
      }
    } /* if Rx flow is not deleted */
  } /* if Rx flow spec is requested in QOS_MODIFY */

  /*-----------------------------------------------------------------------
    Update Tx flow spec if it is specified in QOS_MODIFY
  -----------------------------------------------------------------------*/
  if (qos_modify_mask & QOS_MASK_TX_FLOW)
  {
    /*---------------------------------------------------------------------
      Unset QOS field mask. Need this to handle deletion/modification of Tx
      flow spec correctly
    ---------------------------------------------------------------------*/
    flow_ptr->qos_info_ptr->field_mask &= 
                                   ~(QOS_MASK_TX_FLOW | QOS_MASK_TX_MIN_FLOW |
                                     QOS_MASK_TX_AUXILIARY_FLOWS);

    /*---------------------------------------------------------------------
      If app is not deleting Tx flow spec, update Tx flow spec
    ---------------------------------------------------------------------*/
    if (!((qos_modify_mask & QOS_MASK_TX_FLOW) &&
          flow_ptr->qos_modify_info_ptr->tx.flow_template.req_flow.field_mask == 
                                                            IPFLOW_MASK_NONE))
    {
      flow_ptr->qos_info_ptr->field_mask |= QOS_MASK_TX_FLOW;
      flow_ptr->qos_info_ptr->tx.flow_template.req_flow = 
                       flow_ptr->qos_modify_info_ptr->tx.flow_template.req_flow;

      if (qos_modify_mask & QOS_MASK_TX_MIN_FLOW)
      {
        flow_ptr->qos_info_ptr->field_mask |= QOS_MASK_TX_MIN_FLOW;
        flow_ptr->qos_info_ptr->tx.flow_template.min_req_flow = 
                   flow_ptr->qos_modify_info_ptr->tx.flow_template.min_req_flow;
      }
    } /* if Tx flow spec is not deleted */
  } /* if Tx flow spec is requested in QOS_MODIFY */

  /*-----------------------------------------------------------------------
    Reset qos_modify_info_ptr and free it
  -----------------------------------------------------------------------*/
  flow_ptr->qos_modify_info_ptr->field_mask = QOS_MASK_INVALID;
  ds_flow_delete_qos_modify_spec(flow_ptr);

  // FLOW modified event  
  event_info.net_initiated_qos_info.qos_handle = 
                               (ds_net_initiated_qos_req_handle_type)flow_ptr;
  ds_fwk_notify_events(flow_ptr->bearer_ptr->pdn_mgr_ptr->fw_index,
                       ds_pdn_mgr_convert_pdn_inst_to_fw_ip_type(inst_p->type),
                       PDN_FLOW_MODIFIED_EV,
                       event_info);

  return TRUE;
} /* ds_flow_modify_flow_accepted() */

/*===========================================================================
FUNCTION DS_FLOW_MODIFY_FLOW_CONTEXT

DESCRIPTION
  This function processes the QOS_MODIFY spec and stores it in the flow
  control block for later use. Both flow spec and filter spec
  are processed and validated for each direction. If Rx and/or Tx flow specs are
  modified, they are stored as is after making sure all specified parameters
  are correct. Tx filter spec is validated and stored in decreasing order
  of fltr precedence. Tx filters are disabled until network comes back with
  a response.

PARAMETERS
  iface_ptr         : corresponding iface on which QOS is requested
  flow_ptr          : flow to be modified
  qos_modify_params : structure containing qos_modified_spec and all the
                      information needed to process it
 
RETURN VALUE
   boolean - TRUE if modification passes, FALSE otherwise

DEPENDENCIES
  None.

SIDE EFFECTS
  None
===========================================================================*/
boolean ds_flow_modify_flow_context
(
 ds_flow_type               *flow_context,
 qos_spec_type              *qos_spec
)
{
  ds_ipfltr_add_param_type     fltr_add_param = {0};
  ds_ipfltr_delete_param_type  fltr_delete_param = {0};
  boolean                      is_rx_fltr_modified = FALSE;
  int16                        ds_errno = DS_ENOERR;
  
  if(!ds_flow_is_valid(flow_context) || !qos_spec)
  {
    return FALSE;
  }

  if(flow_context->qos_modify_info_ptr != NULL)
  {
    DS_ERROR_LOG_0("Modify already in progress for this flow.");
    return FALSE;
  }

  DS_MSG1(MSG_LEGACY_HIGH, "ds_flow_modify_flow_context for flow 0x%x",
          flow_context);

  // Validate QoS params
  if(ds_flow_validate_qos_modify_spec(flow_context, qos_spec) == FALSE)
  {
    return FALSE;
  }

  // Store QoS modify spec  
  DS_SYSTEM_HEAP_MEM_ALLOC(flow_context->qos_modify_info_ptr,
                           sizeof(qos_spec_type),qos_spec_type*);
  if(flow_context->qos_modify_info_ptr == NULL)
  {
    return FALSE;
  }
  
  memset(flow_context->qos_modify_info_ptr, 0, sizeof(qos_spec_type));
  memscpy(flow_context->qos_modify_info_ptr, sizeof(qos_spec_type),
          qos_spec, sizeof(qos_spec_type));

  // Copy the RX filters
  DS_SYSTEM_HEAP_MEM_ALLOC(
       flow_context->qos_modify_info_ptr->rx.fltr_template.list_ptr,
       sizeof(ip_filter_type)*MAX_PACKET_FILTERS,
       ip_filter_type*);
  memset(flow_context->qos_modify_info_ptr->rx.fltr_template.list_ptr, 0,
  	      sizeof(ip_filter_type)*MAX_PACKET_FILTERS);  
  memscpy(flow_context->qos_modify_info_ptr->rx.fltr_template.list_ptr,
  	       sizeof(ip_filter_type)*MAX_PACKET_FILTERS,
  	       qos_spec->rx.fltr_template.list_ptr,
  	       sizeof(ip_filter_type)*MAX_PACKET_FILTERS);

  // Copy the TX filters
  DS_SYSTEM_HEAP_MEM_ALLOC(
       flow_context->qos_modify_info_ptr->tx.fltr_template.list_ptr,
       sizeof(ip_filter_type)*MAX_PACKET_FILTERS,
       ip_filter_type*);
  memset(flow_context->qos_modify_info_ptr->tx.fltr_template.list_ptr, 0,
  	      sizeof(ip_filter_type)*MAX_PACKET_FILTERS);	
  memscpy(flow_context->qos_modify_info_ptr->tx.fltr_template.list_ptr,
  	       sizeof(ip_filter_type)*MAX_PACKET_FILTERS,
  	       qos_spec->tx.fltr_template.list_ptr,
  	       sizeof(ip_filter_type)*MAX_PACKET_FILTERS);

  /*-------------------------------------------------------------------------
    Save TX filter spec in FWK if Tx flow spec is specified
  -------------------------------------------------------------------------*/
  if (((qos_spec->field_mask & QOS_MASK_TX_FLOW) &&
       !(flow_context->qos_info_ptr->field_mask & QOS_MASK_TX_FLOW)) ||
      (qos_spec->field_mask & QOS_MODIFY_MASK_TX_FLTR_MODIFY))
  {
    fltr_add_param.client_id = FLTR_CLIENT_QOS_OUTPUT;
    fltr_add_param.fi_ptr_arr = qos_spec->tx.fltr_template.list_ptr;    
    fltr_add_param.num_filters = qos_spec->tx.fltr_template.num_filters;
    fltr_add_param.fi_result = (ds_ipfltr_result_type)flow_context;
    fltr_add_param.subset_id = 5;
    fltr_add_param.fltr_priority = DS_IPFLTR_PRIORITY_FCFS;
    fltr_add_param.is_validated = TRUE;
    fltr_add_param.fw_inst_ptr = ds_fwk_get_inst_ptr_from_index(
                                     flow_context->bearer_ptr->pdn_mgr_ptr->fw_index);
    flow_context->qos_modify_info_ptr->tx.fltr_template.fltr_handle = 
    	                                        ds_ipfltr_add(&fltr_add_param, 
                                                           &ds_errno);
    if(flow_context->qos_modify_info_ptr->tx.fltr_template.fltr_handle == 
    	                                             DS_IPFLTR_INVALID_HANDLE)
    {
      DS_ERROR_LOG_1("TX filter addition failed with err =d", ds_errno);
      ds_flow_delete_qos_modify_spec(flow_context);
      return FALSE;
    }
  }

  /*-------------------------------------------------------------------------
    Save RX filter spec in FWK if Rx flow spec is specified
  -------------------------------------------------------------------------*/
  if((qos_spec->field_mask & QOS_MASK_RX_FLOW) &&
     !(flow_context->qos_info_ptr->field_mask & QOS_MASK_RX_FLOW))
  {
    flow_context->qos_modify_info_ptr->rx.fltr_template.fltr_handle = 
              ds_fwk_rx_qos_fltr_add(ds_fwk_get_inst_ptr_from_index(
                                     flow_context->bearer_ptr->pdn_mgr_ptr->fw_index),
                                     qos_spec,
                                     NULL,
                                     &ds_errno);
    is_rx_fltr_modified = TRUE;
  }  
  else if(qos_spec->field_mask & QOS_MODIFY_MASK_RX_FLTR_MODIFY)
  {
    flow_context->qos_modify_info_ptr->rx.fltr_template.fltr_handle = 
         ds_fwk_rx_qos_fltr_modify(ds_fwk_get_inst_ptr_from_index(
                           flow_context->bearer_ptr->pdn_mgr_ptr->fw_index),
                           qos_spec,
                           flow_context->qos_info_ptr->rx.fltr_template.fltr_handle,
                           NULL,
                           &ds_errno);
    is_rx_fltr_modified = TRUE;
  }

  if(is_rx_fltr_modified &&
     flow_context->qos_modify_info_ptr->rx.fltr_template.fltr_handle == 
                                                          DS_IPFLTR_INVALID_HANDLE)
  {
    DS_ERROR_LOG_1("RX filter addition failed with err =d", ds_errno);
  
    if(flow_context->qos_modify_info_ptr->tx.fltr_template.fltr_handle != 
    	                                                     DS_IPFLTR_INVALID_HANDLE)
    {
      fltr_delete_param.fw_inst_ptr = ds_fwk_get_inst_ptr_from_index(
                                     flow_context->bearer_ptr->pdn_mgr_ptr->fw_index);
      fltr_delete_param.client_id = FLTR_CLIENT_QOS_OUTPUT;
      fltr_delete_param.fi_handle = 
      	            flow_context->qos_modify_info_ptr->tx.fltr_template.fltr_handle;
      ds_ipfltr_delete(&fltr_delete_param, &ds_errno);
    }
    
    ds_flow_delete_qos_modify_spec(flow_context);      
    return FALSE;
  }

  // Log packet


  DS_MSG1(MSG_LEGACY_HIGH, "Flow 0x%x modified successfully !", flow_context);  
  return TRUE;
}/* ds_flow_modify_flow_context */

/*===========================================================================
FUNCTION DS_FLOW_UPDATE_FILTER_IDENTIFIER_BIT_MASK

DESCRIPTION
  Update requested filter identifier bit mask.

PARAMETERS
 flow_context_p      - Pointer to flow context.
 set_identifier      - If true, set the bit else clear the bit.

DEPENDENCIES
  None.

RETURN VALUE
 boolean              - Indicates true or false.
  
SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_flow_update_filter_identifier_bit_mask
( 
  ds_flow_type     *flow_context_p,
  boolean           set_identifier
)
{
  qos_spec_type             *orig_spec = NULL;  
  ip_filter_type            *fltr = NULL; 
  int                        cnt = 0;          
  uint16                     identifier = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  if(!ds_flow_is_valid(flow_context_p))
  {
    return FALSE;                 
  }                                           

  orig_spec = flow_context_p->qos_info_ptr;

  /*------------------------------------------------------------------------
     Loop thru all filters and update the filter identifier bit mask for RX
  -------------------------------------------------------------------------*/
  for(cnt =0; cnt<orig_spec->rx.fltr_template.num_filters; cnt++)
  {
    fltr = &(orig_spec->rx.fltr_template.list_ptr[cnt]);
    /*---------------------------------------------------------------------
      NW Assigned filter id is in the last 4 bits.
    ---------------------------------------------------------------------*/
    identifier = (fltr->ipfltr_aux_info.fi_id)&(0xF);
    
    if(set_identifier)
    {
      /*-------------------------------------------------------------------
        Set the Requested Filter Identifier for Rx
      -------------------------------------------------------------------*/
      ds_pdn_tft_set_filter_identifier_bit(identifier,
                                           &(flow_context_p->fi_identifier));     
    }
    else
    {
      /*-------------------------------------------------------------------
        Clear the Filter Identifier for Rx
      -------------------------------------------------------------------*/
      ds_pdn_tft_clear_filter_identifier_bit(identifier,
                                             &(flow_context_p->fi_identifier));  
    }
  }/* RX FOR Loop*/

  /*------------------------------------------------------------------------
     Loop thru all filters and update the filter identifier bit mask for TX
  -------------------------------------------------------------------------*/
  for(cnt =0; cnt<orig_spec->tx.fltr_template.num_filters; cnt++)
  {
    fltr = &(orig_spec->tx.fltr_template.list_ptr[cnt]);
    /*---------------------------------------------------------------------
      NW Assigned filter id is in the last 4 bits.
    ---------------------------------------------------------------------*/
    identifier = (fltr->ipfltr_aux_info.fi_id&(0xF));
    
    if(set_identifier)
    {
      /*-------------------------------------------------------------------
        Set the Requested Filter Identifier for Tx
      -------------------------------------------------------------------*/
      ds_pdn_tft_set_filter_identifier_bit(identifier,
                                           &(flow_context_p->fi_identifier));     
    }
    else
    {
      /*-------------------------------------------------------------------
        Clear the Filter Identifier for Tx
      -------------------------------------------------------------------*/
      ds_pdn_tft_clear_filter_identifier_bit(identifier,
                                             &(flow_context_p->fi_identifier));  
    }
  }/* FOR TX  Filters */

  return TRUE; 
}/* ds_flow_update_filter_identifier_bit_mask */

/*===========================================================================
FUNCTION DS_FLOW_DELETE_FILTER_FROM_FLOW

DESCRIPTION
  Removes the filter from the passed ps qos spec.

PARAMETERS
  flow_context_p   - Flow Context information.
  filter_id        - Filter to be removed.
  app_qos_spec    - filter spec from which the filter has to be deleted.
  modified_qos_spec  - filter spec which has the modified filter changes
  
DEPENDENCIES
  None.

RETURN VALUE
  boolean          - Returns true if filters are retrieved
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_delete_filter_from_flow
( 
  ds_flow_type        *flow_context_p,
  uint32               filter_id,
  qos_spec_type       *app_qos_spec,
  qos_spec_type       *modified_qos_spec
)
{
  ip_filter_type           *fltr = NULL, *mod_fltr=NULL; 
  int                       cnt = 0, mod_cnt=0;          
  uint32                    identifier = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  /*------------------------------------------------------------------------
    Loop thru all filters and update the filter identifier bit mask for RX
  -------------------------------------------------------------------------*/
  for(cnt =0; cnt < app_qos_spec->rx.fltr_template.num_filters; cnt++)
  {
    fltr = &(app_qos_spec->rx.fltr_template.list_ptr[cnt]);
    mod_fltr = &(modified_qos_spec->rx.fltr_template.list_ptr[mod_cnt]);
    /*---------------------------------------------------------------------
      NW Assigned filter id is in the last 4 bits.
    ---------------------------------------------------------------------*/
    identifier = (1 << (fltr->ipfltr_aux_info.fi_id & (0xF)));
    if((identifier & filter_id) == identifier)
    {
      /* delete the filter from Qos spec */
      DS_MSG1(MSG_LEGACY_HIGH, "Deleting filter id %d from Rx Flow",identifier); 
    }
    else
    {
      memscpy(mod_fltr,sizeof(ip_filter_type), fltr, sizeof(ip_filter_type));
      mod_cnt++;
     
    }
  }/* RX FOR Loop*/
  /*---------------------------------------------------------------------
    Update the filter count
  ---------------------------------------------------------------------*/
  modified_qos_spec->rx.fltr_template.num_filters = mod_cnt;

  /*------------------------------------------------------------------------
    Loop thru all filters and update the filter identifier bit mask for TX
  -------------------------------------------------------------------------*/
  mod_cnt = 0;
  for(cnt =0; cnt<app_qos_spec->tx.fltr_template.num_filters; cnt++)
  {
    fltr = &(app_qos_spec->tx.fltr_template.list_ptr[cnt]);
    mod_fltr = &(modified_qos_spec->tx.fltr_template.list_ptr[mod_cnt]);
    /*---------------------------------------------------------------------
      NW Assigned filter id is in the last 4 bits.
    ---------------------------------------------------------------------*/
    identifier = (1 << (fltr->ipfltr_aux_info.fi_id & (0xF)));
    if((identifier & filter_id) == identifier)
    {
      /* delete the filter from Qos spec */
      DS_MSG1(MSG_LEGACY_HIGH, "Deleting filter id %d from Tx Flow",identifier);
    }
    else
    {
      memscpy(mod_fltr,sizeof(ip_filter_type), fltr, sizeof(ip_filter_type));
      mod_cnt++;
    }
  }/* TX FOR Loop*/
  
  /*---------------------------------------------------------------------
    Update the filter count
  ---------------------------------------------------------------------*/
  modified_qos_spec->tx.fltr_template.num_filters = mod_cnt;

  return TRUE;
}/* ds_flow_delete_filter_from_flow */

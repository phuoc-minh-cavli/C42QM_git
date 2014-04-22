/*===========================================================================
                      DS_BEARER_FLOW_MANAGER.C
DESCRIPTION
This file defines Bearer flow manager APIs.

Copyright (c) 2018 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/3gpp/pdnmgr/src/ds_bearer_flow_manager.c#2 $
  $DateTime: 2023/01/19 23:06:43 $$Author: pwbldsvc $

when           who    what, where, why
--------    ---    ----------------------------------------------------------
08/09/17    sd      First checkin
===========================================================================*/


#include "ds_pdn_defs.h"
#include "ds_bearer_flow_manager.h"
#include "ds_pdn_flow_manager.h"
#include "ds_bearer_manager.h"
#include "ds_pdn_hdlr.h"
#include "ds_pdn_tft.h"
#include "ds_pdn_nv_manager.h"

/*---------------------------------------------------------------------------
  kbps to bps multiplier, we need this to be uint64 as the product of this
  and some MACRO values needs to be uint64 type
---------------------------------------------------------------------------*/
uint32 ds_bearer_flow_kbps_to_bps_multiplier_32bit = 1000;
uint64 ds_bearer_flow_kbps_to_bps_multiplier_64bit = 1000;

/*----------------------------------------------------------------------------
  Bearer Flow Manager table. This table contains all the bearer flow references.
  This table is updated whenever a new bearer flow manager is created or deleted.
  Each flow manager context is tied to a bearer context
----------------------------------------------------------------------------*/
ds_bearer_flow_manager_s *ds_bearer_flow_mgr_tbl[DS_3GPP_MAX_BEARER_CONTEXT] = {NULL};



/*===========================================================================
FUNCTION ds_bearer_flow_manager_free_mem

DESCRIPTION
  This function frees dyn memory for bearer flow manager.
 
PARAMETERS
   bflow_manager_p
 
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
=============================================================================*/
void ds_bearer_flow_manager_free_mem
(
   ds_bearer_flow_manager_s *bflow_manager_p
)
{
/*---------------------------------------------------------------------------*/
  if(bflow_manager_p != NULL)
  {
    if(bflow_manager_p->eps_flow)
    {
      DS_SYSTEM_HEAP_MEM_FREE(bflow_manager_p->eps_flow);
    }

    if(bflow_manager_p->gprs_flow)
    {
      DS_SYSTEM_HEAP_MEM_FREE(bflow_manager_p->gprs_flow);
    }
    
    ds_bearer_flow_mgr_tbl[bflow_manager_p->index] = NULL;
    DS_SYSTEM_HEAP_MEM_FREE(bflow_manager_p);
  }
}

/*===========================================================================
FUNCTION DS_BEARER_FLOW_MANAGER_ALLOCATE_MEM

DESCRIPTION
  Allocate a new flow manager for this bearer.

PARAMETERS
 bearer_mgr_p     - Pointer to bearer context.

DEPENDENCIES
  None.

RETURN VALUE
 ds_bearer_flow_manager_s * - Pointer to empty flow manager.
  
SIDE EFFECTS
  None.
===========================================================================*/
ds_bearer_flow_manager_s* ds_bearer_flow_manager_allocate_mem
( 
  ds_bearer_mgr_s *bearer_mgr_p
)
{
  uint8                     alloc_index = 0;
  ds_bearer_flow_manager_s *bflow_manager_p = NULL;
  sys_sys_mode_e_type       mode;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for (alloc_index = 0; alloc_index < DS_3GPP_MAX_BEARER_CONTEXT; alloc_index++)
  {      
    if(NULL == ds_bearer_flow_mgr_tbl[alloc_index])
    {
      DS_SYSTEM_HEAP_MEM_ALLOC(bflow_manager_p,sizeof(ds_bearer_flow_manager_s),
	  	                        ds_bearer_flow_manager_s *);

      ds_bearer_flow_mgr_tbl[alloc_index] = bflow_manager_p;
      if(bflow_manager_p != NULL)
      {
        memset(bflow_manager_p, 0, sizeof(ds_bearer_flow_manager_s));
        bflow_manager_p->index = alloc_index;
      }
      else
      {
        return NULL;
      }

      mode = ds_pdn_hdlr_get_current_network_mode();
      if(mode == SYS_SYS_MODE_LTE)
      {
        DS_SYSTEM_HEAP_MEM_ALLOC(bflow_manager_p->eps_flow,
			                     sizeof(ds_eps_bearer_flow_manager_s),
			                     ds_eps_bearer_flow_manager_s*);
        if( NULL == bflow_manager_p->eps_flow )
        {
          DS_SYSTEM_HEAP_MEM_FREE(bflow_manager_p);
        	 return NULL;
        }
        
        memset(bflow_manager_p->eps_flow, 0, sizeof(ds_eps_bearer_flow_manager_s));
      }
      else if(mode == SYS_SYS_MODE_GSM)
      {
        DS_SYSTEM_HEAP_MEM_ALLOC(bflow_manager_p->gprs_flow,
		                         sizeof(ds_gprs_bearer_flow_manager_s),
								 ds_gprs_bearer_flow_manager_s*);
		
        if( NULL == bflow_manager_p->gprs_flow )
        {
          DS_SYSTEM_HEAP_MEM_FREE(bflow_manager_p);
          return NULL;
        }
        
        memset(bflow_manager_p->gprs_flow, 0, sizeof(ds_gprs_bearer_flow_manager_s));
      }

      /*---------------------------------------------------------------------
         Initialize Lists and map bearer context
        -----------------------------------------------------------------------*/
      bflow_manager_p->bearer_ptr = bearer_mgr_p;    
      list_init(&(bflow_manager_p->flow_context));
      break;
    }
  }

  if (alloc_index == DS_3GPP_MAX_BEARER_CONTEXT)
  {
    DS_ERROR_LOG_0("MAX Bearer Flow contexts have been allocated");
  }

  return bflow_manager_p;
} /* ds_bearer_flow_manager_allocate_mem */

/*===========================================================================
FUNCTION DS_BEARER_FLOW_MANAGER_RELEASE_FLOW

DESCRIPTION
  Releases/frees the bearer flow manager.
  
PARAMETERS
 bearer_mgr_p     - Pointer to bearer context.

DEPENDENCIES
  None.

RETURN VALUE
 None.
  
SIDE EFFECTS
  None.
===========================================================================*/
void ds_bearer_flow_manager_release_flow
( 
  ds_bearer_mgr_s *bearer_mgr_p
)
{
  uint8                     index = 0;
  ds_bearer_flow_manager_s *bflow_manager_p = NULL;
  ds_bearer_flow_manager_list_type   *next_flow_cntx = NULL;
  ds_bearer_flow_manager_list_type   *bflow_list_p = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for (index = 0; index < DS_3GPP_MAX_BEARER_CONTEXT; index++)
  {     
    bflow_manager_p = ds_bearer_flow_mgr_tbl[index];
    if( NULL !=  bflow_manager_p &&
    	   bearer_mgr_p == bflow_manager_p->bearer_ptr )
    {
      /*---------------------------------------------------------------------
             Empty the flow context list  - revisit later
          ---------------------------------------------------------------------*/          
      bflow_list_p = list_peek_front(&(bflow_manager_p->flow_context));
      while(bflow_list_p != NULL &&
      	     bflow_list_p->flow_context_p != NULL)
      {
        DS_MSG0(MSG_LEGACY_HIGH, "Flow context list is not empty");
        next_flow_cntx = list_peek_next(&(bflow_manager_p->flow_context),
                                        &(bflow_list_p->bflow_link));
        ds_flow_release_flow_context(bflow_list_p->flow_context_p); 
        
        list_pop_item(&(bflow_manager_p->flow_context),
                      &(bflow_list_p->bflow_link));
        DS_SYSTEM_HEAP_MEM_FREE(bflow_list_p);
        
        bflow_list_p = next_flow_cntx;
      }

      list_destroy(&(bflow_manager_p->flow_context));
      ds_bearer_flow_manager_free_mem(bflow_manager_p);
      break;
    }
  }

  return;
} /* ds_bearer_flow_manager_release_flow */


/*===========================================================================
FUNCTION ds_bearer_flow_manager_find_entry

DESCRIPTION
  Finds bearer flow manager entry corresponding to given bearer

PARAMETERS
  bearer_mgr_p - Bearer context pointer
 
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
ds_bearer_flow_manager_s* ds_bearer_flow_manager_find_entry
(
  ds_bearer_mgr_s *bearer_mgr_p
)
{
  ds_bearer_flow_manager_s   *bflow_manager_p = NULL;
  uint8                       index = 0;
/*-------------------------------------------------------------------------*/

  for(index = 0; index < DS_3GPP_MAX_BEARER_CONTEXT;index++)
  {
    bflow_manager_p = ds_bearer_flow_mgr_tbl[index];
    if((bflow_manager_p != NULL) &&
       (bflow_manager_p->bearer_ptr == bearer_mgr_p))
    {
      return bflow_manager_p;
    }
  }

  return NULL;
}

/*===========================================================================
FUNCTION DS_BEARER_FLOW_MANAGER_BIND_FLOW

DESCRIPTION
  Bind the flows associated with the bearer.

PARAMETERS
 bearer_mgr_p     - Pointer to Bearer information.

DEPENDENCIES
  None.

RETURN VALUE
 boolean              - Indicates true or false.
  
SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_bearer_flow_manager_bind_flow
( 
  ds_bearer_mgr_s *bearer_mgr_p
)
{
  ds_bearer_flow_manager_s            *bflow_manager_p = NULL;
  ds_bearer_flow_manager_list_type    *bflow_list_p  = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  bflow_manager_p = ds_bearer_flow_manager_find_entry(bearer_mgr_p);
  if(!bflow_manager_p)
  {
    return FALSE;
  } 
  /*---------------------------------------------------------------------
     Bind the flow on every flow context on the list
  ---------------------------------------------------------------------*/          
  bflow_list_p = list_peek_front(&(bflow_manager_p->flow_context));
  while(bflow_list_p != NULL &&
  	     bflow_list_p->flow_context_p != NULL)
  {
    DS_MSG1(MSG_LEGACY_HIGH, "Binding Flow context 0x:%x",
    	       bflow_list_p->flow_context_p);

    bflow_list_p->flow_context_p->bearer_ptr = bearer_mgr_p;
    
    ds_flow_context_activate_ind(bflow_list_p->flow_context_p);
    bflow_list_p = list_peek_next(&(bflow_manager_p->flow_context),
                                  &(bflow_list_p->bflow_link));
  } 
  
  return TRUE;
}/* ds_bearer_flow_manager_bind_flow */


/*===========================================================================
FUNCTION DS_BEARER_FLOW_MANAGER_FIND_FLOW_CONTEXT

DESCRIPTION
  Find the Flow context associated with the Qos.

PARAMETERS
 bearer_context_p     - Pointer to Bearer information.
 Flow_type               - Indicates V4 or V6 flow.

DEPENDENCIES
  None.

RETURN VALUE
 flow_context_p       - Pointer to flow context.
  
SIDE EFFECTS
  None.
===========================================================================*/
ds_flow_type* ds_bearer_flow_mgr_find_flow_context
( 
  ds_bearer_mgr_s *bearer_mgr_p,
  uint8            flow_type
)
{
  ds_bearer_flow_manager_s            *bflow_manager_p = NULL;
  ds_bearer_flow_manager_list_type    *bflow_list_p  = NULL;
  ds_flow_type                        *ret_flow_context_p = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  bflow_manager_p = ds_bearer_flow_manager_find_entry(bearer_mgr_p);
  if(!bflow_manager_p)
  {
    return NULL;
  } 
  
  /*---------------------------------------------------------------------
     Find FlowContext in the list
  ---------------------------------------------------------------------*/          
  bflow_list_p = list_peek_front(&(bflow_manager_p->flow_context));
  while(bflow_list_p && bflow_list_p->flow_context_p)
  {
    if((bflow_list_p->flow_context_p->flow_type & flow_type))
    {
      if(ret_flow_context_p == NULL)
      {
        ret_flow_context_p = bflow_list_p->flow_context_p;
      }
      else
      {
        DS_ERROR_LOG_0("More than one  flow context for bearer");
        break;
      }
    }
    bflow_list_p = list_peek_next(&(bflow_manager_p->flow_context),
    	                             &(bflow_list_p->bflow_link));
  }     
  
  if(ret_flow_context_p == NULL)
  {  
    DS_ERROR_LOG_0("No flow context found for given iface type");
  }
  
  return ret_flow_context_p;
}/* ds_bearer_flow_manager_find_flow_context */

/*===========================================================================
FUNCTION DS_BEARER_FLOW_MANAGER_ADD_FLOW_CONTEXT_TO_FLOW_MANAGER

DESCRIPTION
  Add a flow context to the bearer flow manager list.

PARAMETERS
 flow_context_s       - Pointer to Flow Context infomation.
 bearer_context_p     - Pointer to Bearer information.

DEPENDENCIES
  None.

RETURN VALUE
 boolean              - Indicates true or false.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_bearer_flow_manager_add_flow_context_to_flow_manager
( 
  ds_bearer_mgr_s          *bearer_context_p,
  ds_flow_type             *flow_context_p
)
{
  ds_bearer_flow_manager_s          *bflow_manager_p = NULL;
  ds_bearer_flow_manager_list_type  *bflow_list_p = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(!ds_bearer_mgr_validate_bearer_ptr(bearer_context_p) ||
     !ds_flow_is_valid(flow_context_p))
  {
    return FALSE;
  }

  bflow_manager_p = ds_bearer_flow_manager_find_entry(bearer_context_p);
  if(!bflow_manager_p)
  {
    return FALSE;
  }
  
  flow_context_p->bearer_flow_mgr_ptr = bflow_manager_p;
  
  // Check if flow context is already present in the list.
  bflow_list_p = list_peek_front(&(bflow_manager_p->flow_context));

  while(bflow_list_p != NULL &&
  	     bflow_list_p->flow_context_p != NULL)
  {
    DS_MSG1(MSG_LEGACY_HIGH, "Flow %x", bflow_list_p->flow_context_p);
    if(bflow_list_p->flow_context_p == flow_context_p)
    {
      DS_MSG2(MSG_LEGACY_ERROR,
              "Flow %x already present in bflow list for bearer %d", 
              bflow_list_p->flow_context_p, bearer_context_p->index);
      return FALSE;
    }
    bflow_list_p = list_peek_next(&(bflow_manager_p->flow_context),
                                  &(bflow_list_p->bflow_link));
  }

  DS_SYSTEM_HEAP_MEM_ALLOC(bflow_list_p, sizeof(ds_bearer_flow_manager_list_type),
  	                        ds_bearer_flow_manager_list_type *);
  if(bflow_list_p == NULL)
  {
    DS_MSG0(MSG_LEGACY_ERROR, "Memory allocation failed");
    return FALSE;
  }

  memset(bflow_list_p, 0, sizeof(ds_bearer_flow_manager_list_type));
  bflow_list_p->flow_context_p = flow_context_p;  	
  list_push_front(&(bflow_manager_p->flow_context),
                  &(bflow_list_p->bflow_link));

  DS_MSG3(MSG_LEGACY_HIGH, "Flow %x added to bearer %d bearer flow %d", 
          flow_context_p, bearer_context_p->index, bflow_manager_p);
  return TRUE;
} /* ds_bearer_flow_manager_add_flow_context_to_flow_manager */

/*===========================================================================
FUNCTION DS_BEARER_FLOW_MANAGER_REMOVE_FLOW_CONTEXT_FROM_FLOW_MANAGER

DESCRIPTION
  Remove a flow context from the bearer flow manager list.

PARAMETERS
 flow_context_s       - Pointer to Flow Context infomation.
 bearer_context_p     - Pointer to Bearer information.

DEPENDENCIES
  None.

RETURN VALUE
 boolean              - Indicates true or false.
  
SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_bearer_flow_manager_remove_flow_context_from_flow_manager
( 
  ds_bearer_mgr_s            *bearer_context_p,
  ds_flow_type               *flow_context_p
)
{
  ds_bearer_flow_manager_s          *bflow_manager_p = NULL;
  ds_bearer_flow_manager_list_type  *bflow_list_p = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  if(!ds_bearer_mgr_validate_bearer_ptr(bearer_context_p) || 
     !ds_flow_is_valid(flow_context_p))
  {
    return FALSE;
  }
  
  bflow_manager_p = ds_bearer_flow_manager_find_entry(bearer_context_p);
  if(!bflow_manager_p)
  {
    return FALSE;
  }  

  // Check if flow context is  present in the list.
  bflow_list_p = list_peek_front(&(bflow_manager_p->flow_context));
  while(bflow_list_p != NULL &&
  	     bflow_list_p->flow_context_p != NULL)
  {
    if(bflow_list_p->flow_context_p == flow_context_p)
    {
      break;
    }
    
    bflow_list_p = list_peek_next(&(bflow_manager_p->flow_context),
                                  &(bflow_list_p->bflow_link));
  }
  
  if(bflow_list_p == NULL ||
  	  bflow_list_p->flow_context_p == NULL)
  {
    DS_ERROR_LOG_0("Flow not present in bearer flow manager list");
    return FALSE;
  }
  
  list_pop_item(&(bflow_manager_p->flow_context),
                &(bflow_list_p->bflow_link));
  DS_SYSTEM_HEAP_MEM_FREE(bflow_list_p);
  
  DS_MSG3(MSG_LEGACY_HIGH, "Flow %x removed from bearer %d bearer flow %d", 
          flow_context_p, bearer_context_p->index, bflow_manager_p);
  return TRUE;
} /* ds_bearer_flow_manager_remove_flow_context_from_flow_manager */

/*===========================================================================
FUNCTION DS_BEARER_FLOW_MANAGER_GO_NULL

DESCRIPTION
  Post go Null on the flows associated with the bearer.

PARAMETERS
 bearer_mgr_p     - Pointer to Bearer information.

DEPENDENCIES
  None.

RETURN VALUE
 boolean              - Indicates true or false.
  
SIDE EFFECTS
  None.
===========================================================================*/
void ds_bearer_flow_manager_go_null
( 
  ds_bearer_mgr_s *bearer_mgr_p
)
{
  ds_pdn_mgr_s                     *pdn_mgr_p = NULL;
  ds_pdn_flow_manager_s            *pflow_manager_p = NULL;
  ds_bearer_flow_manager_s         *bflow_manager_p = NULL;
  ds_bearer_flow_manager_list_type *bflow_list_p  = NULL;
  ds_pdn_flow_manager_pending_list_type
                                   *pend_flow_list_p  = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  bflow_manager_p = ds_bearer_flow_manager_find_entry(bearer_mgr_p);
  if(!bflow_manager_p)
  {
    return;
  } 
  /*---------------------------------------------------------------------
     Go NUll indication on every flow context on the list
  ---------------------------------------------------------------------*/          
  bflow_list_p = list_peek_front(&(bflow_manager_p->flow_context));

  while(bflow_list_p && bflow_list_p->flow_context_p)
  {
    ds_flow_go_null_ind(bflow_list_p->flow_context_p); 
    bflow_list_p = list_peek_next(&(bflow_manager_p->flow_context),
                                  &(bflow_list_p->bflow_link));
  }     

  /*---------------------------------------------------------------------
     Go NULL indication on every flow context in the pending flow list.
     When modify is still pending with network but DS gets CALL_END from
     CM then before posting phys_link_gone_ind() during call teardown we
     need to clear all flows(includinf flows from pending flow list) for
     this bearer. 
  ---------------------------------------------------------------------*/          
  pdn_mgr_p = bearer_mgr_p->pdn_mgr_ptr;
  if(ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p))
  {
    pflow_manager_p = ds_pdn_flow_mgr_get_entry(pdn_mgr_p->index);
    if(pflow_manager_p)
    {
      pend_flow_list_p = list_peek_front(
                                        &(pflow_manager_p->pending_flow_cntx));
      while( pend_flow_list_p &&
             ds_flow_is_valid(pend_flow_list_p->flow_context_p))
      {
        if(pend_flow_list_p->flow_context_p->bearer_flow_mgr_ptr == 
                                                              bflow_manager_p)
        {
          DS_MSG1(MSG_LEGACY_HIGH,
                  "Found flow_context_ptr 0x%x in Pending list",
                  pend_flow_list_p->flow_context_p);
          
          list_pop_item(&(pflow_manager_p->pending_flow_cntx),
                        &(pend_flow_list_p->pflow_link));
          
          ds_flow_release_flow_context(pend_flow_list_p->flow_context_p);
          DS_SYSTEM_HEAP_MEM_FREE(pend_flow_list_p);
          
          pend_flow_list_p = list_peek_front(
                                        &(pflow_manager_p->pending_flow_cntx));
        }
        else
        {
          pend_flow_list_p = 
                          list_peek_next(&(pflow_manager_p->pending_flow_cntx),
                                         &(pend_flow_list_p->pflow_link));
        }
      }     
    }         
  }         
  
  return;
}/* ds_bearer_flow_manager_go_null */

/*===========================================================================
FUNCTION DS_BEARER_FLOW_MANAGER_DELETE_FLOW_CONTEXT

DESCRIPTION
  Delete a flow context from a bearer context.

PARAMETERS
  pdn_context_p           -  PDN information
  bearer_context_p        -  Bearer information
  flow_context_p          -  Flow context information.
  

DEPENDENCIES
  None.

RETURN VALUE
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_bearer_flow_mgr_delete_flow_context
(
  ds_bearer_mgr_s            *bearer_mgr_p,
  ds_flow_type               *flow_context_p
)
{
  ds_pdn_mgr_s               *pdn_mgr_p;
  /*---------------------------------------------------------------------*/

  if(!ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p) ||
     !ds_flow_is_valid(flow_context_p))
  {
    return;
  }
  
  pdn_mgr_p = bearer_mgr_p->pdn_mgr_ptr;

  /*---------------------------------------------------------------------
     Update the Filter precedence bit mask
   ---------------------------------------------------------------------*/
  ds_pdn_flow_manager_update_precedence_bit_mask(
                                pdn_mgr_p, flow_context_p, FALSE, FALSE, FALSE);

  /*---------------------------------------------------------------------
     Update the Filter Identifier bit mask
   ---------------------------------------------------------------------*/
  ds_bearer_flow_manager_update_filter_identifier_bit_mask(
                                           bearer_mgr_p, flow_context_p, FALSE);      
  ds_flow_release_flow_context(flow_context_p);      
  return;
} /*ds_bearer_flow_manager_delete_flow_context */

/*===========================================================================
FUNCTION DS_BEARER_FLOW_MANAGER_UPDATE_PREC_FOR_ACTIVE_BEARERS

DESCRIPTION
  In case of back to back precedence conflicts, the conflicting precedence
  bit for an active bearer can be cleared as part of regular deactivation.
  This function ensures to set the precedence bits for all the active bearers
  of thhis PDN except the one passed as a parameter, for which clean up is 
  in progress.

PARAMETERS
  pdn_context_p           -  PDN information
  bearer_context_p        -  Bearer information

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void ds_bearer_flow_manager_update_prec_for_active_bearers
(
  ds_pdn_mgr_s                *pdn_context_p,
  ds_bearer_mgr_s             *bearer_context_p
)
{
  ds_bearer_flow_manager_list_type *bflow_list_p = NULL;
  ds_bearer_flow_manager_s         *bflow_manager_p = NULL;
  uint8                             index = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!ds_pdn_mgr_validate_pdn_ptr(pdn_context_p) ||
      !ds_bearer_mgr_validate_bearer_ptr(bearer_context_p))
  {
    return;
  }
  
  for (index = 0; index < DS_3GPP_MAX_BEARER_CONTEXT; index++)
  {
    bflow_manager_p = ds_bearer_flow_mgr_tbl[index];    
    
    if( !(bflow_manager_p) ||
        (bflow_manager_p->pdn_ptr != pdn_context_p) ||
        (bflow_manager_p->bearer_ptr == bearer_context_p))
    {
      continue;
    }

    bflow_list_p = list_peek_front(&(bflow_manager_p->flow_context));
    
    while(bflow_list_p && bflow_list_p->flow_context_p)
    {
      ds_pdn_flow_manager_update_precedence_bit_mask(pdn_context_p, 
                                                     bflow_list_p->flow_context_p,
                                                     FALSE, TRUE, FALSE);                          
      bflow_list_p = list_peek_next(&(bflow_manager_p->flow_context),
                                    &(bflow_list_p->bflow_link));;
    }
  }
  
  return;
}

/*===========================================================================
FUNCTION DS_BEARER_FLOW_MANAGER_DELETE_ALL_FLOWS

DESCRIPTION
  Delets all flows associated with the bearer.

PARAMETERS
 bearer_mgr_p     - Pointer to Bearer information.

DEPENDENCIES
  None.

RETURN VALUE
 None.
  
SIDE EFFECTS
  None.
===========================================================================*/
void ds_bearer_flow_manager_delete_all_flows
( 
  ds_bearer_mgr_s *bearer_mgr_p
)
{
  ds_bearer_flow_manager_s         *bflow_manager_p = NULL;
  ds_bearer_flow_manager_list_type *bflow_list_p  = NULL;
  ds_bearer_flow_manager_list_type *next_flow_context_p  = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  bflow_manager_p = ds_bearer_flow_manager_find_entry(bearer_mgr_p);
  if(!bflow_manager_p)
  {
    return;
  } 
  /*---------------------------------------------------------------------
     Go NUll indication on every flow context on the list
  ---------------------------------------------------------------------*/          
  bflow_list_p = list_peek_front(&(bflow_manager_p->flow_context));

  while(bflow_list_p && bflow_list_p->flow_context_p)
  {
    next_flow_context_p = list_peek_next(&(bflow_manager_p->flow_context),
                                         &(bflow_list_p->bflow_link));
    list_pop_item(&(bflow_manager_p->flow_context),
                  &(bflow_list_p->bflow_link));
    
    DS_MSG3(MSG_LEGACY_HIGH, "Flow %x removed from bearer %d bearer flow %d", 
            bflow_list_p->flow_context_p, bearer_mgr_p->index, bflow_manager_p);

    ds_bearer_flow_mgr_delete_flow_context(bearer_mgr_p, 
                                           bflow_list_p->flow_context_p);

    DS_SYSTEM_HEAP_MEM_FREE(bflow_list_p);
    
    bflow_list_p = next_flow_context_p;
  }     

  /* Called to set back the precedence bits for active bearers of this PDN
     in case of precedence conflict */
  ds_bearer_flow_manager_update_prec_for_active_bearers(
                                                     bearer_mgr_p->pdn_mgr_ptr, 
                                                     bearer_mgr_p);         
  return;
}/* ds_bearer_flow_manager_delete_all_flows */

/*===========================================================================
FUNCTION DS_BEARER_FLOW_MANAGER_CONVERT_CAUSE_CODE

DESCRIPTION
  Convert the generic cause codes to specific cause code.

PARAMETERS
  mode_info_ptr      -  CM passed mode information.
  ds_int_cause_ptr   -  DS generic cause  

DEPENDENCIES
  None.

RETURN VALUE
  Returns True if the cause code conversion is successful.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_bearer_flow_manager_convert_cause_code
(
  esm_cause_T                                  *esm_cause,
  ds_bearer_flow_internal_cause_code_enum_type *icause_ptr
)
{
  if((esm_cause == NULL) || (icause_ptr == NULL))
  {
    return FALSE;
  }

  esm_cause->valid = TRUE;  
  switch(*icause_ptr)
  {
    case DS_BEARER_FLOW_INTERNAL_INSUFFICIENT_RESOURCES:
      esm_cause->esm_cause = NAS_ESM_INSUFFICIENT_RESOURCES;
      break;

    case DS_BEARER_FLOW_INTERNAL_FEATURE_NOT_SUPPORTED:
      esm_cause->esm_cause = NAS_ESM_FEATURE_NOT_SUPPORTED;
      break;

    case DS_BEARER_FLOW_INTERNAL_QOS_NOT_ACCEPTED :
      esm_cause->esm_cause = NAS_ESM_EPS_QOS_NOT_ACCEPTED;
      break;

    case DS_BEARER_FLOW_INTERNAL_SEMANTIC_ERR_IN_TFT: 
      esm_cause->esm_cause = NAS_ESM_SEMANTIC_ERR_IN_TFT_OPERATION;
      break;

    case DS_BEARER_FLOW_INTERNAL_SYNTACTICAL_ERR_IN_TFT:
      esm_cause->esm_cause = NAS_ESM_SYNTACTICAL_ERR_IN_TFT_OPERATION;
      break;

    case DS_BEARER_FLOW_INTERNAL_SEMANTIC_ERR_IN_FILTER:
      esm_cause->esm_cause = NAS_ESM_SYMANTIC_ERR_IN_PKT_FILTER;
      break;

    case DS_BEARER_FLOW_INTERNAL_SYNTACTICAL_ERR_IN_FILTER:
      esm_cause->esm_cause = NAS_ESM_SYNTACTICAL_ERR_IN_PKT_FILTER;
      break;

    case DS_BEARER_FLOW_INTERNAL_BEARER_WITH_NO_TFT_ALREADY_PRESENT:
      esm_cause->esm_cause = NAS_ESM_EPS_BEARER_CONT_WO_TFT_ALREADY_ACT;
      break;

    case DS_BEARER_FLOW_INTERNAL_INVALID_MANDATORY_INFO:
      esm_cause->esm_cause = NAS_ESM_INVALID_MANDATORY_INFORMATION;
      break;

    case DS_BEARER_FLOW_INTERNAL_ACTIVATION_REJECTED:
      esm_cause->esm_cause = NAS_ESM_ACTIVATION_REJECTED;
      break;

    case DS_BEARER_FLOW_INTERNAL_SERV_OPT_NOT_SUPPORTED:
      esm_cause->esm_cause = NAS_ESM_SERV_OPT_NOT_SUPPORTED;
      break;
      
    default:
      esm_cause->valid = FALSE;
      return FALSE;
  }/* switch*/
  
  return TRUE;
}/* ds_bearer_flow_manager_convert_cause_code*/

/*===========================================================================
FUNCTION DS_BEARER_FLOW_MANAGER_CONVERT_NW_BIT_RATE

DESCRIPTION
  Converts the NW passed bit rate to uint64.

PARAMETERS
  input         -  network octet to be converted

DEPENDENCIES
  None.

RETURN VALUE
uint64          - return converted bit rate
  
SIDE EFFECTS
  None.
===========================================================================*/
uint64 ds_bearer_flow_manager_convert_nw_bit_rate
(
  byte input
)
{
  uint64 converted_rate = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(input == 0)
  {
    return converted_rate;
  }
  
  if(input <= DS_EPS_1KB_BITRATE_END_RANGE)
  {
    converted_rate = DS_EPS_1KB_BITRATE_MINIMUM_VALUE+
		       (input - DS_EPS_1KB_BITRATE_START_RANGE);   
  }
  else if(input <= DS_EPS_8KB_BITRATE_END_RANGE)
  {
    converted_rate = DS_EPS_8KB_BITRATE_MINIMUM_VALUE+
		(input - DS_EPS_8KB_BITRATE_START_RANGE)* DS_EPS_8KB_BITRATE_MULTIPLE;   
  }
  else if(input <= DS_EPS_64KB_BITRATE_END_RANGE)
  {
    converted_rate = DS_EPS_64KB_BITRATE_MINIMUM_VALUE+
		(input - DS_EPS_64KB_BITRATE_START_RANGE)* DS_EPS_64KB_BITRATE_MULTIPLE;   
  }
  
  // Convert NW rate (kbps) to App rate (bps)
  converted_rate *= ds_bearer_flow_kbps_to_bps_multiplier_64bit;
  DS_MSG1(MSG_LEGACY_HIGH, "converted rate returned is 0x%x",converted_rate);
  return converted_rate;
} /* ds_bearer_flow_manager_convert_nw_bit_rate */

/*===========================================================================
FUNCTION DS_BEARER_FLOW_MANAGER_CONVERT_EXT_NW_BIT_RATE

DESCRIPTION
  Converts the NW passed extended bit rate to uint32.

PARAMETERS
  input         -  network octet to be converted

DEPENDENCIES
  None.

RETURN VALUE
uint32          - return converted bit rate
  
SIDE EFFECTS
  None.
===========================================================================*/
uint64 ds_bearer_flow_manager_convert_ext_nw_bit_rate
(
  byte input
)
{
  uint64 converted_rate = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(input == 0)
  {
   	return converted_rate;
  }
  if(input <= DS_EPS_100KB_BITRATE_END_RANGE)
  {
    converted_rate = DS_EPS_100KB_BITRATE_MINIMUM_VALUE+
		(input - DS_EPS_100KB_BITRATE_START_RANGE)*DS_EPS_100KB_BITRATE_MULTIPLE;   
  }
  else if(input <= DS_EPS_1MB_BITRATE_END_RANGE)
  {
    converted_rate = DS_EPS_1MB_BITRATE_MINIMUM_VALUE+
		(input - DS_EPS_1MB_BITRATE_START_RANGE)*DS_EPS_1MB_BITRATE_MULTIPLE;   
  }
  else if(input <= DS_EPS_2MB_BITRATE_END_RANGE)
  {
    converted_rate = DS_EPS_2MB_BITRATE_MINIMUM_VALUE+
		(input - DS_EPS_2MB_BITRATE_START_RANGE)* DS_EPS_2MB_BITRATE_MULTIPLE;   
  }
  else
  {
  	 converted_rate = DS_EPS_EXT_BITRATE_MAXIMUM_VALUE;
  }

  // Convert NW rate (kbps) to App rate (bps)
  converted_rate *= ds_bearer_flow_kbps_to_bps_multiplier_64bit;
  DS_MSG1(MSG_LEGACY_HIGH, "converted extended rate returned is 0x%x",
          converted_rate);
  return converted_rate;
} /* ds_bearer_flow_manager_convert_ext_nw_bit_rate */

/*===========================================================================
FUNCTION DS_BEARER_FLOW_MANAGER_CONVERT_EXT2_NW_BIT_RATE

DESCRIPTION
  Converts the NW passed extended bit rate to uint32.

PARAMETERS
  input         -  network octet to be converted

DEPENDENCIES
  None.

RETURN VALUE
uint32          - return converted bit rate
  
SIDE EFFECTS
  None.

===========================================================================*/
uint64 ds_bearer_flow_manager_convert_ext2_nw_bit_rate
(
  byte input
)
{
  uint64 converted_rate = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(input == 0)
  {
   	return converted_rate;
  }
  if(input <= DS_EPS_4MB_BITRATE_END_RANGE)
  {
    converted_rate = DS_EPS_4MB_BITRATE_MINIMUM_VALUE+
		(input - DS_EPS_4MB_BITRATE_START_RANGE)*DS_EPS_4MB_BITRATE_MULTIPLE;   
  }
  else if(input <= DS_EPS_10MB_BITRATE_END_RANGE)
  {
    converted_rate = DS_EPS_10MB_BITRATE_MINIMUM_VALUE+
		(input - DS_EPS_10MB_BITRATE_START_RANGE)*DS_EPS_10MB_BITRATE_MULTIPLE;   
  }
  else if(input <= DS_EPS_100MB_BITRATE_END_RANGE)
  {
    converted_rate = DS_EPS_100MB_BITRATE_MINIMUM_VALUE+
		(input - DS_EPS_100MB_BITRATE_START_RANGE)* DS_EPS_100MB_BITRATE_MULTIPLE;   
  }
  else
  {
   	converted_rate = DS_EPS_APP_BITRATE_MAXIMUM_VALUE;
  }

  // Convert NW rate (kbps) to App rate (bps)
  converted_rate *= ds_bearer_flow_kbps_to_bps_multiplier_64bit;
  DS_MSG1(MSG_LEGACY_HIGH, "converted extended-2 rate returned is 0x%x",
          converted_rate);
  return converted_rate;
} /* ds_bearer_flow_manager_convert_ext2_nw_bit_rate */

/*===========================================================================
FUNCTION DS_BEARER_FLOW_MANAGER_CONVERT_NW_FLOW_TO_APP_FLOW

DESCRIPTION
  Converts the passed Flow spec to App flow spec.

PARAMETERS
  nw_flow         -  NW flow to be converted
  app_flow        -  App flow to store the converted info.

DEPENDENCIES
  None.

RETURN VALUE
boolean          - true if conversion is succesfull 
                     - false if conversion is not succesfull
  
SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_bearer_flow_manager_convert_nw_flow_to_app_flow
(
  sdf_qos_T                                    *nw_flow,
  qos_spec_type                                *app_flow,
  ds_bearer_flow_internal_cause_code_enum_type *cause_code_p
)
{
  ip_flow_type            *app_rx_flow = NULL;
  ip_flow_type            *app_tx_flow = NULL;
  uint64                   rx_mbr = 0;
  uint64                   rx_gbr = 0;
  uint64                   tx_mbr = 0;
  uint64                   tx_gbr = 0;
  ds_local_err_val_e_type  local_err = DS_LOCAL_ERR_VAL_NONE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    
  if(cause_code_p == NULL)
  {
    local_err = DS_LOCAL_ERR_VAL_0;
    goto func_exit;
  }
  else
  {
    *cause_code_p = DS_BEARER_FLOW_INTERNAL_SEMANTIC_ERR_IN_FILTER;
  }

  if(nw_flow == NULL || app_flow == NULL)
  {
    local_err = DS_LOCAL_ERR_VAL_0;
    *cause_code_p = DS_BEARER_FLOW_INTERNAL_INSUFFICIENT_RESOURCES;
    goto func_exit;
  } 
  
  app_rx_flow = &(app_flow->rx.flow_template.req_flow);
  app_tx_flow = &(app_flow->tx.flow_template.req_flow);

  if((nw_flow->length == DS_EPS_QOS_QCI_LEN_FOR_DATA) ||
     (nw_flow->length == DS_EPS_QOS_QCI_LEN_FOR_EXT_DATA) ||
     (nw_flow->length == DS_EPS_QOS_QCI_LEN_FOR_EXT2_DATA))
  {
    app_rx_flow->data_rate.format_type = DATA_RATE_FORMAT_MIN_MAX_TYPE;
    rx_mbr = 
           ds_bearer_flow_manager_convert_nw_bit_rate(nw_flow->max_bit_rate_dl);
    rx_gbr = ds_bearer_flow_manager_convert_nw_bit_rate(nw_flow->gbr_dl);
    app_rx_flow->lte_params.lte_qci =(lte_qci_enum_type)nw_flow->qci;
    app_rx_flow->field_mask |=(int)(IPFLOW_MASK_DATA_RATE);
    app_rx_flow->field_mask |=(int)(IPFLOW_MASK_LTE_QCI); 


    app_tx_flow->data_rate.format_type = DATA_RATE_FORMAT_MIN_MAX_TYPE;
    tx_mbr = 
           ds_bearer_flow_manager_convert_nw_bit_rate(nw_flow->max_bit_rate_ul);
    tx_gbr = ds_bearer_flow_manager_convert_nw_bit_rate(nw_flow->gbr_ul);
    
    app_tx_flow->lte_params.lte_qci = (lte_qci_enum_type)nw_flow->qci;
    app_tx_flow->field_mask |=(int)(IPFLOW_MASK_DATA_RATE);
    app_tx_flow->field_mask |=(int)(IPFLOW_MASK_LTE_QCI);    
  } /*nw_flow->length == 11||7 */

  if((nw_flow->length == DS_EPS_QOS_QCI_LEN_FOR_EXT_DATA) ||
     (nw_flow->length == DS_EPS_QOS_QCI_LEN_FOR_EXT2_DATA))
  {
    if((nw_flow->max_bit_rate_dl_ex != 0) && 
       (rx_mbr == 
        DS_EPS_MAX_1_OCTET_BITRATE*ds_bearer_flow_kbps_to_bps_multiplier_64bit))
    {
      rx_mbr = ds_bearer_flow_manager_convert_ext_nw_bit_rate(
                                                   nw_flow->max_bit_rate_dl_ex);
    }

    if((nw_flow->gbr_dl_ex != 0) &&
       (rx_gbr == 
        DS_EPS_MAX_1_OCTET_BITRATE*ds_bearer_flow_kbps_to_bps_multiplier_64bit))
    {
      rx_gbr = ds_bearer_flow_manager_convert_ext_nw_bit_rate(
                                                            nw_flow->gbr_dl_ex);
    }

    if((nw_flow->max_bit_rate_ul_ex != 0) &&
       (tx_mbr == 
        DS_EPS_MAX_1_OCTET_BITRATE*ds_bearer_flow_kbps_to_bps_multiplier_64bit))
    {
      tx_mbr = ds_bearer_flow_manager_convert_ext_nw_bit_rate(
                                                   nw_flow->max_bit_rate_ul_ex);
    }

    if((nw_flow->gbr_ul_ex != 0) &&
       (tx_gbr ==
        DS_EPS_MAX_1_OCTET_BITRATE*ds_bearer_flow_kbps_to_bps_multiplier_64bit))
    {
      tx_gbr = ds_bearer_flow_manager_convert_ext_nw_bit_rate(
                                                            nw_flow->gbr_ul_ex);
    }
  }/* nw_flow->length == 13|9 */

  if(nw_flow->length == DS_EPS_QOS_QCI_LEN_FOR_EXT2_DATA)
  {
    if((nw_flow->max_bit_rate_dl_ex2 != 0) && 
       (rx_mbr == DS_EPS_EXT_BITRATE_MAXIMUM_VALUE*
                                   ds_bearer_flow_kbps_to_bps_multiplier_64bit))
    {
      rx_mbr = ds_bearer_flow_manager_convert_ext2_nw_bit_rate(
                                                  nw_flow->max_bit_rate_dl_ex2);
    }

    if((nw_flow->gbr_dl_ex2 != 0) &&
       (rx_gbr == DS_EPS_EXT_BITRATE_MAXIMUM_VALUE*
                                   ds_bearer_flow_kbps_to_bps_multiplier_64bit))
    {
      rx_gbr = ds_bearer_flow_manager_convert_ext2_nw_bit_rate(
                                                           nw_flow->gbr_dl_ex2);
    }

    if((nw_flow->max_bit_rate_ul_ex2 != 0) &&
       (tx_mbr == DS_EPS_EXT_BITRATE_MAXIMUM_VALUE*
                                   ds_bearer_flow_kbps_to_bps_multiplier_64bit))
    {
      tx_mbr = ds_bearer_flow_manager_convert_ext2_nw_bit_rate(
                                                  nw_flow->max_bit_rate_ul_ex2);
    }

    if((nw_flow->gbr_ul_ex2 != 0) &&
       (tx_gbr == DS_EPS_EXT_BITRATE_MAXIMUM_VALUE*
                                   ds_bearer_flow_kbps_to_bps_multiplier_64bit))
    {
      tx_gbr = ds_bearer_flow_manager_convert_ext2_nw_bit_rate(
                                                           nw_flow->gbr_ul_ex2);
    }
  }/* nw_flow->length == 13 */


  /* min and max dl rates are zero,qos mbr,gbr are valid, return false , 
    if nw_flow->length == DS_EPS_QOS_QCI_MIN_LEN_FOR_DATA. this indicates 
    only qci is specified */  
  if((tx_mbr == 0) && 
     (rx_mbr == 0) &&
     (nw_flow->length != DS_EPS_QOS_QCI_MIN_LEN_FOR_DATA) )
  {
    local_err = DS_LOCAL_ERR_VAL_1;
    *cause_code_p = DS_BEARER_FLOW_INTERNAL_INSUFFICIENT_RESOURCES;
    goto func_exit;
  }

  /* Return failure if GBR is great than MBR for gbr qci bearer*/
  if(((((lte_qci_enum_type)nw_flow->qci >=LTE_QCI_1) &&
        ((lte_qci_enum_type)nw_flow->qci <=LTE_QCI_4)) ||
      (((lte_qci_enum_type)nw_flow->qci ==LTE_QCI_65) ||
       ((lte_qci_enum_type)nw_flow->qci <=LTE_QCI_66))) &&
     ((tx_gbr > tx_mbr)|| (rx_gbr > rx_mbr)))
  {
    local_err = DS_LOCAL_ERR_VAL_2;
    *cause_code_p = DS_BEARER_FLOW_INTERNAL_INSUFFICIENT_RESOURCES;
    goto func_exit;
  }  

  /*
  If 3gpp release 11 is supported on the UE send 64bit values to DS. DS will send both 32bit and 64bit 
  format to app. If release 11 is not supported then if the value is more than 256mbps (pre relase 11
  maximum). 3gpp will only send 256mbps to DS. Otherwise we would send whatever we get from network. 
  Either way, it would be in 32bit variables.
  */
  if(ds_pdn_nv_manager_is_rel_11_or_above_supported()) 
  {
     app_rx_flow->data_rate.format_type = DATA_RATE_FORMAT_MIN_MAX_EX_TYPE;

     /*Update application rate if network provides new data rate*/
     if (rx_mbr != 0)
     {
       app_rx_flow->data_rate.format.min_max_ex.max_rate = rx_mbr;
     } 

     if (rx_gbr != 0)
     {
       app_rx_flow->data_rate.format.min_max_ex.guaranteed_rate = rx_gbr;
     }

     app_tx_flow->data_rate.format_type = DATA_RATE_FORMAT_MIN_MAX_EX_TYPE;

     if(tx_mbr != 0)
     {
       app_tx_flow->data_rate.format.min_max_ex.max_rate = tx_mbr;
     }

     if(tx_gbr != 0)
     {
       app_tx_flow->data_rate.format.min_max_ex.guaranteed_rate = tx_gbr;
     }

     DS_MSG2(MSG_LEGACY_HIGH, "MAX UL and MAX DL bit rates are %d  %d",
             app_tx_flow->data_rate.format.min_max_ex.max_rate,
             app_rx_flow->data_rate.format.min_max_ex.max_rate );

     /* min and max dl rates are zero, validation only for dedicated bearer
        activation */ 
    if((app_rx_flow->data_rate.format.min_max_ex.max_rate == 0) && 
       (app_tx_flow->data_rate.format.min_max_ex.max_rate== 0))
    {
      local_err = DS_LOCAL_ERR_VAL_3;
      *cause_code_p = DS_BEARER_FLOW_INTERNAL_INSUFFICIENT_RESOURCES;
      goto func_exit;
    }
  }
  else
  {
    app_rx_flow->data_rate.format_type = DATA_RATE_FORMAT_MIN_MAX_TYPE;
    app_tx_flow->data_rate.format_type = DATA_RATE_FORMAT_MIN_MAX_TYPE;

   /*Update application rate if network provides new data rate*/
    if (rx_mbr > DS_EPS_EXT_BITRATE_MAXIMUM_VALUE*
                                    ds_bearer_flow_kbps_to_bps_multiplier_64bit) 
    {
      app_rx_flow->data_rate.format.min_max.max_rate = 
                                  DS_EPS_EXT_BITRATE_MAXIMUM_VALUE*
                                    ds_bearer_flow_kbps_to_bps_multiplier_32bit;
    }
    else
    {
      if (rx_gbr != 0)
      {
        app_rx_flow->data_rate.format.min_max.max_rate = (uint32)rx_mbr;
      }
    }
    
    if (rx_gbr > DS_EPS_EXT_BITRATE_MAXIMUM_VALUE*
                                    ds_bearer_flow_kbps_to_bps_multiplier_64bit) 
    {
      app_rx_flow->data_rate.format.min_max.guaranteed_rate = 
                                  DS_EPS_EXT_BITRATE_MAXIMUM_VALUE*
                                    ds_bearer_flow_kbps_to_bps_multiplier_32bit;
    }
    else
    {
      if(rx_gbr != 0)
      {
        app_rx_flow->data_rate.format.min_max.guaranteed_rate = (uint32)rx_gbr;
      }
    }
    
    if (tx_mbr > DS_EPS_EXT_BITRATE_MAXIMUM_VALUE*
                                    ds_bearer_flow_kbps_to_bps_multiplier_64bit) 
    {
       app_tx_flow->data_rate.format.min_max.max_rate = 
                                  DS_EPS_EXT_BITRATE_MAXIMUM_VALUE*
                                    ds_bearer_flow_kbps_to_bps_multiplier_32bit;
    }
    else
    {
      if(tx_mbr != 0)
      { 
        app_tx_flow->data_rate.format.min_max.max_rate = (uint32)tx_mbr;
      }
    }
    
    if (tx_gbr > DS_EPS_EXT_BITRATE_MAXIMUM_VALUE*
                                    ds_bearer_flow_kbps_to_bps_multiplier_64bit) 
    {
       app_tx_flow->data_rate.format.min_max.guaranteed_rate = 
                                  DS_EPS_EXT_BITRATE_MAXIMUM_VALUE*
                                    ds_bearer_flow_kbps_to_bps_multiplier_32bit;
    }
    else
    {
      if(tx_gbr != 0)
      { 
        app_tx_flow->data_rate.format.min_max.guaranteed_rate = (uint32)tx_gbr;
      }
    }

    DS_MSG2(MSG_LEGACY_HIGH, "MAX UL and MAX DL bit rates are %d  %d",
            app_tx_flow->data_rate.format.min_max.max_rate,
            app_rx_flow->data_rate.format.min_max.max_rate );

    /* min and max dl rates are zero, validation only for dedicated bearer
       activation */  
    if((app_rx_flow->data_rate.format.min_max.max_rate == 0) && 
       (app_tx_flow->data_rate.format.min_max.max_rate== 0))
    {
      local_err = DS_LOCAL_ERR_VAL_4;
      *cause_code_p = DS_BEARER_FLOW_INTERNAL_INSUFFICIENT_RESOURCES;
      goto func_exit;
    }
  }

func_exit:
  if(local_err != DS_LOCAL_ERR_VAL_NONE)
  {
    DS_ERROR_LOG_1("Convert NW Flow failed with error =d", local_err);
    return FALSE;
  }
  
  return TRUE;
} /* ds_bearer_flow_manager_convert_nw_flow_to_app_flow */

/*===========================================================================
FUNCTION  DS_BEARER_FLOW_MANAGER_CONVERT_NW_SDF_QOS_TO_APP

DESCRIPTION
  Converts the SDF QOS NW passed to App data rate.

PARAMETERS
nw_qos          -  SDF qos to be converted
app_qos         -  Application qos structure

DEPENDENCIES
  None.

RETURN VALUE
boolean         - return TRUE or FALSE
  
SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_bearer_flow_manager_convert_nw_sdf_qos_to_app
(
  ds_bearer_flow_manager_s                     *bflow_manager_p,
  sdf_qos_T                                    *nw_qos,
  qos_spec_type                                *app_qos,
  ds_bearer_flow_internal_cause_code_enum_type *cause_code_p
)
{
  ip_flow_type           *app_rx_flow = NULL;
  ip_flow_type           *app_tx_flow = NULL;
  byte                    qci_prev = 0;
  boolean                 rel12_qci= ds_pdn_nv_manager_get_rel12_qci_support();
  ds_local_err_val_e_type local_err = DS_LOCAL_ERR_VAL_NONE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(cause_code_p == NULL)
  {
    local_err = DS_LOCAL_ERR_VAL_0;
    goto func_exit;
  }
  else
  {
    *cause_code_p = DS_BEARER_FLOW_INTERNAL_SEMANTIC_ERR_IN_FILTER;
  }

  if(nw_qos == NULL || app_qos == NULL || bflow_manager_p == NULL)
  {
    local_err = DS_LOCAL_ERR_VAL_1;
    goto func_exit;
  }

  qci_prev = bflow_manager_p->rx_flow.lte_params.lte_qci;
  DS_MSG1(MSG_LEGACY_HIGH, "Received qos with QCI value:%d", nw_qos->qci);
  
  /*------------------------------------------------------------------------
    Convert LTE QOS values to PS-Specific Qos value
  -------------------------------------------------------------------------*/
  if((nw_qos->qci >(byte)(LTE_QCI_9)) && 
     (nw_qos->qci <=127) && 
     (!rel12_qci))
  {
    local_err = DS_LOCAL_ERR_VAL_2;
    goto func_exit;
  }
  
  /* QCI VALUE : 1to 4, 65,66 */
  if((nw_qos->qci >=(byte)(LTE_QCI_1) && 
       nw_qos->qci <=(byte)(LTE_QCI_4))||
     ((nw_qos->qci ==(byte)(LTE_QCI_65)) || 
      (nw_qos->qci ==(byte)(LTE_QCI_66))))
  {
    app_rx_flow = &(app_qos->rx.flow_template.req_flow);    
    if((qci_prev >=(byte)(LTE_QCI_5) &&
        qci_prev <=(byte)(LTE_QCI_9)) ||
       (qci_prev ==(byte)(LTE_QCI_69) ||
        qci_prev ==(byte)(LTE_QCI_70)))

    {
      local_err = DS_LOCAL_ERR_VAL_3;
      goto func_exit;
    }

    if(nw_qos->length!=DS_EPS_QOS_QCI_MIN_LEN_FOR_DATA &&
       nw_qos->length!=DS_EPS_QOS_QCI_LEN_FOR_DATA &&
       nw_qos->length!=DS_EPS_QOS_QCI_LEN_FOR_EXT_DATA &&
       nw_qos->length!=DS_EPS_QOS_QCI_LEN_FOR_EXT2_DATA)
    {
      local_err = DS_LOCAL_ERR_VAL_4;
      goto func_exit;      
    }  
    
    /*----------------------------------------------------------------------
      RX/TX FLOW SPEC: Convert Max and Guaranteed Data Rates (bps)
    -----------------------------------------------------------------------*/
    if(ds_bearer_flow_manager_convert_nw_flow_to_app_flow(nw_qos,
                                                          app_qos,
                                                         cause_code_p) == FALSE)
    {
      local_err = DS_LOCAL_ERR_VAL_5;
      goto func_exit;    
    }
  } /* GBR QCI */  
  else if((nw_qos->qci >= 0x80 && nw_qos->qci <= 0xFE) ||
          (nw_qos->qci >= 0x0A && nw_qos->qci <= 0x40) ||
          (nw_qos->qci >= 0x43 && nw_qos->qci <= 0x44) ||
          (nw_qos->qci >= 0x47 && nw_qos->qci <= 0x7F))
  {

    /*QCI VALUES 10 to 64,67 to 68, 71 to 127,128 to 255 */
    lte_qci_enum_type converted_qci = LTE_QCI_5;

    app_rx_flow = &(app_qos->rx.flow_template.req_flow);
    app_tx_flow = &(app_qos->tx.flow_template.req_flow);

    if(ds_bearer_flow_manager_convert_nw_flow_to_app_flow(nw_qos,
                                                          app_qos,
                                                         cause_code_p) == FALSE)
    {
      //There is no set data rate associated with this bearer. Use non-GBR QCI.
      //Pick the worst QCI since there is no way of mapping to the correct
      //QCI without knowing more information.
      converted_qci = LTE_QCI_9;
      if ((qci_prev >=(byte)(LTE_QCI_1) &&
           qci_prev <=(byte)(LTE_QCI_4)) ||
          (qci_prev ==(byte)(LTE_QCI_65) ||
           qci_prev ==(byte)(LTE_QCI_66)))

      {
        local_err = DS_LOCAL_ERR_VAL_6;
        goto func_exit;
      }
    }
    else 
    {
      //There is set data rate. Use GBR QCI.
      converted_qci = LTE_QCI_4;
      if((qci_prev >=(byte)(LTE_QCI_5) &&
          qci_prev <=(byte)(LTE_QCI_9)) ||
         (qci_prev ==(byte)(LTE_QCI_69) ||
          qci_prev ==(byte)(LTE_QCI_70)))

      {
        local_err = DS_LOCAL_ERR_VAL_7;
        goto func_exit;
      }
    }

    app_rx_flow->lte_params.lte_qci = converted_qci;
    app_rx_flow->field_mask |= (int)IPFLOW_MASK_LTE_QCI;
    app_tx_flow->lte_params.lte_qci = converted_qci;
    app_tx_flow->field_mask |= (int)IPFLOW_MASK_LTE_QCI;
  }  
  else if((nw_qos->qci >=(byte)(LTE_QCI_5) && 
            nw_qos->qci <=(byte)(LTE_QCI_9))||
          ((nw_qos->qci ==(byte)(LTE_QCI_69)) || 
           (nw_qos->qci ==(byte)(LTE_QCI_70))))
  {
    /*QCI VALUES 5 to 9, 69, 70 */
    app_rx_flow = &(app_qos->rx.flow_template.req_flow); 
    if((qci_prev >=(byte)(LTE_QCI_1) &&
        qci_prev <=(byte)(LTE_QCI_4)) ||
       (qci_prev ==(byte)(LTE_QCI_65) ||
        qci_prev ==(byte)(LTE_QCI_66)))

    {
      local_err = DS_LOCAL_ERR_VAL_8;
      goto func_exit;
    }

    app_rx_flow = &(app_qos->rx.flow_template.req_flow);
    app_rx_flow->lte_params.lte_qci =(lte_qci_enum_type)nw_qos->qci;
    app_rx_flow->field_mask|=(int)IPFLOW_MASK_LTE_QCI;

    app_tx_flow = &(app_qos->tx.flow_template.req_flow);
    app_tx_flow->lte_params.lte_qci =(lte_qci_enum_type)nw_qos->qci;
    app_tx_flow->field_mask|=(int)IPFLOW_MASK_LTE_QCI;
  } 
  else
  {
    /*QCI VALUES 0 and 255*/
    local_err = DS_LOCAL_ERR_VAL_9;
    goto func_exit;
  }
  
func_exit:
  if(local_err != DS_LOCAL_ERR_VAL_NONE)
  {
    DS_ERROR_LOG_1("Convert SDF QoS failed with error =d", local_err);
    return FALSE;
  }
  
  return TRUE;
}

/*===========================================================================
FUNCTION DS_BEARER_FLOW_MANAGER_CONVERT_DATA_RATE

DESCRIPTION
  Convert the NW FLOW to APP Flow. Invoke the mode specific handlers to 
  convert the value.

PARAMETERS
  bearer_context_p -  Bearer information
  mode_info_p      -  mode specific call info. 
  app_qos_p        -  Pointer to App Qos structure.

DEPENDENCIES
  None.

RETURN VALUE
boolean - return DS3G_FAILURE or DS3G_SUCCESS
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean  ds_bearer_flow_manager_convert_data_rate
(
 ds_bearer_mgr_s                              *bearer_mgr_p,
 sdf_qos_T                                    *sdf_qos,
 qos_T                                        *neg_qos,
 qos_spec_type                                *app_qos,
 ds_bearer_flow_internal_cause_code_enum_type *icause_code
)
{
  ds_bearer_flow_manager_s *bflow_manager_p = NULL;
  /*-----------------------------------------------------------------------*/
  
  bflow_manager_p = ds_bearer_flow_manager_find_entry(bearer_mgr_p);

  if(icause_code == NULL)
  {
    return FALSE;
  }
  else
  {
    *icause_code = DS_BEARER_FLOW_INTERNAL_INSUFFICIENT_RESOURCES;
  }
  
  if(bflow_manager_p == NULL || sdf_qos == NULL || app_qos == NULL)
  {
    return FALSE;
  }

  if(ds_bearer_flow_manager_convert_nw_sdf_qos_to_app(bflow_manager_p,
                                                      sdf_qos,
                                                      app_qos,
                                                      icause_code)== FALSE)
  {
    DS_ERROR_LOG_0("Data Rate conversion Failure");
    return FALSE;
  }

  if(bflow_manager_p->eps_flow == NULL)
  {
    return FALSE;
  }

  /*----------------------------------------------------------------------
     Store the Data rate in Mode specific context
  -----------------------------------------------------------------------*/
  memscpy((void*)&bflow_manager_p->eps_flow->sdf_qos, sizeof(sdf_qos_T),
          (void*)sdf_qos, sizeof(sdf_qos_T));

  //If the QCI was converted internally; store the new QCI value.
  if (app_qos->rx.flow_template.req_flow.field_mask & IPFLOW_MASK_LTE_QCI)      
  {
    bflow_manager_p->eps_flow->sdf_qos.qci = 
                         	app_qos->rx.flow_template.req_flow.lte_params.lte_qci;
  }
  else if(app_qos->tx.flow_template.req_flow.field_mask & IPFLOW_MASK_LTE_QCI)      
  {
    bflow_manager_p->eps_flow->sdf_qos.qci = 
                         	app_qos->tx.flow_template.req_flow.lte_params.lte_qci;
  } 
  
  /* Mode independent*/
  memscpy((void *)&(bflow_manager_p->rx_flow), sizeof(ip_flow_type),
          (void *)&(app_qos->rx.flow_template.req_flow), sizeof(ip_flow_type));         

  memscpy((void *)&(bflow_manager_p->tx_flow), sizeof(ip_flow_type),
          (void *)&(app_qos->tx.flow_template.req_flow), sizeof(ip_flow_type)); 
  
  /* store umts qos set */
  if(neg_qos != NULL &&
     neg_qos->valid == TRUE)
  {
    memscpy(&(bflow_manager_p->neg_qos), sizeof(qos_T),
            neg_qos, sizeof(qos_T));
  }

  return TRUE;  
}

/*===========================================================================
FUNCTION DS_BEARER_FLOW_MANAGER_UPDATE_FILTER_IDENTIFIER_BIT_MASK

DESCRIPTION
  Update filter identifier bit mask for all the filters in the flow context.

PARAMETERS
 bearer_context_p    - Pointer to bearer Context.
 flow_context_p      - Pointer to flow context.
 set_identifier      - If true, set the bit else clear the bit.

DEPENDENCIES
  None.

RETURN VALUE
 boolean              - Indicates true or false.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_bearer_flow_manager_update_filter_identifier_bit_mask
( 
  ds_bearer_mgr_s  *bearer_context_p,
  ds_flow_type     *flow_context_p,
  boolean           set_identifier
)
{
  ds_bearer_flow_manager_s  *bflow_manager_p = NULL;
  qos_spec_type             *orig_spec = NULL;  
  ip_filter_type            *fltr = NULL; 
  int                        cnt = 0;          
  uint16                     identifier = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  if(!ds_bearer_mgr_validate_bearer_ptr(bearer_context_p) || 
     !ds_flow_is_valid(flow_context_p))
  {
    return FALSE;
  }

  bflow_manager_p = ds_bearer_flow_manager_find_entry(bearer_context_p);
  if(!bflow_manager_p)
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
    identifier =(fltr->ipfltr_aux_info.fi_id &(0xF));

    if(ds_pdn_tft_is_filter_identifier_requested(
                          identifier, &(flow_context_p->ignore_fi_id)) == TRUE)
    {
      continue;
    }
    
    if(set_identifier)
    {
      /*-------------------------------------------------------------------
        Set the Requested FI Identifer for Rx
      -------------------------------------------------------------------*/
      ds_pdn_tft_set_filter_identifier_bit(
                                identifier, &(bflow_manager_p->fi_identifier));     
    }
    else
    {
      /*-------------------------------------------------------------------
        Clear the FI Identifier for Rx
      -------------------------------------------------------------------*/
      ds_pdn_tft_clear_filter_identifier_bit(
                                identifier, &(bflow_manager_p->fi_identifier));  
    }
  }/* RX FOR Loop*/
  
  /*------------------------------------------------------------------------
     Loop thru all filters and update the filter identifier bit mask for TX
  -------------------------------------------------------------------------*/
  for(cnt =0; cnt<orig_spec->tx.fltr_template.num_filters; cnt++)
  {
    fltr = &(orig_spec->tx.fltr_template.list_ptr[cnt]);
    identifier = (fltr->ipfltr_aux_info.fi_id & 0xf);
    
    if(ds_pdn_tft_is_filter_identifier_requested(
                          identifier, &(flow_context_p->ignore_fi_id)) == TRUE)
    {
      continue;
    }

    if(set_identifier)
    {
      /*-------------------------------------------------------------------
        Set the Requested FI Identifier for Tx
      -------------------------------------------------------------------*/
      ds_pdn_tft_set_filter_identifier_bit(
                                identifier, &(bflow_manager_p->fi_identifier));     
    }
    else
    {
      /*-------------------------------------------------------------------
        Clear the FI Identifier for Tx
      -------------------------------------------------------------------*/
      ds_pdn_tft_clear_filter_identifier_bit(
                                identifier, &(bflow_manager_p->fi_identifier));  
    }
  }/* FOR TX  Filters */

  return TRUE; 
}/* ds_bearer_flow_manager_update_filter_identifier_bit_mask*/

/*===========================================================================
FUNCTION DS_BEARER_FLOW_MANAGER_CHECK_SDF_ID

DESCRIPTION
  Handle the bearer modification response.

PARAMETERS
  sdf_id - Flow identifier

DEPENDENCIES
  None.
    
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
===========================================================================*/
void ds_bearer_flow_manager_check_sdf_id
(
  word   sdf_id
)
{
  ds_pdn_mgr_s               *pdn_context_p = NULL;
  ds_flow_type               *flow_context_p = NULL;
  ds_bearer_flow_manager_s   *bflow_manager_p = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  /*------------------------------------------------------------------------
    Check if SDF ID is valid.
  -------------------------------------------------------------------------*/
  if(sdf_id != 0xFFFF)
  {
    /*---------------------------------------------------------------------
      Check if any pending request matches the SDF_ID.
    ---------------------------------------------------------------------*/
    flow_context_p = ds_pdn_flow_manager_find_pending_flow_context(
                                  &pdn_context_p, sdf_id);
    if(!ds_pdn_mgr_validate_pdn_ptr(pdn_context_p) ||
       !flow_context_p)
    {
      DS_ERROR_LOG_1("Could not find the flow context matching SDF ID:=x",
                     sdf_id);
      return; 
    } 

    if(flow_context_p->state != DS_FLOW_STATE_DOWN)
    {
      DS_ERROR_LOG_1("Received SDF ID in an unknown state: =d",
                       flow_context_p->state);
      return;
    }
    
    /*---------------------------------------------------------------------
      Found the pending flow context. Remove the pending flow context.
    ---------------------------------------------------------------------*/
    ds_pdn_flow_manager_update_precedence_bit_mask(
                   pdn_context_p,flow_context_p,FALSE,FALSE,FALSE);
    
    /*---------------------------------------------------------------------
      Update the bearer filter identifier mask.
      and unbind the flow.
    ---------------------------------------------------------------------*/
    bflow_manager_p = flow_context_p->bearer_flow_mgr_ptr;
    if(bflow_manager_p)
    {
      ds_bearer_flow_manager_update_filter_identifier_bit_mask(
                      bflow_manager_p->bearer_ptr, flow_context_p,FALSE);
      ds_flow_go_null_ind(flow_context_p);
    }

    ds_pdn_flow_manager_remove_pending_flow_context(pdn_context_p,
                                                    flow_context_p);
    ds_flow_release_flow_context(flow_context_p);
    return;
  } /* SDF_ID != 0xFFFF */ 
  
  DS_ERROR_LOG_1("Invalid SDF ID :=x",sdf_id); 
  return;
}/* ds_bearer_flow_manager_check_sdf_id */

/*===========================================================================
FUNCTION DS_BEARER_FLOW_MANAGER_INST_DOWN_HDLR

DESCRIPTION
  The instance has gone out of use. Check for an UE initiated bearers and
  clean them up. Update the TFT information that needs to be sent out to 
  the network.

PARAMETERS
  pdn_context_p            -  Pointer to PDN context.
  bearer_context_p         -  Pointer to Bearer context.
  flow_type                -  IP Type.

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_bearer_flow_manager_inst_down_hdlr
(
  ds_pdn_mgr_s                *pdn_context_p,
  ds_bearer_mgr_s             *bearer_context_p,
  uint8                        flow_type
)
{
  ds_flow_type                      *flow_context_p  = NULL;
  ds_flow_type                      *old_flow_context_p = NULL;
  ds_bearer_flow_manager_s          *bflow_manager_p = NULL;
  ds_bearer_flow_manager_list_type  *bflow_list_p = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  bflow_manager_p = ds_bearer_flow_manager_find_entry(bearer_context_p);
  if(!bflow_manager_p)
  {
    return;
  }
    
  /*---------------------------------------------------------------------
    Step through all the flow contexts
  ---------------------------------------------------------------------*/  
  bflow_list_p = list_peek_front(&(bflow_manager_p->flow_context));
  while(bflow_list_p != NULL &&
  	     ds_flow_is_valid(bflow_list_p->flow_context_p))
  {
    flow_context_p = bflow_list_p->flow_context_p;
    old_flow_context_p = NULL;
    
    if(flow_context_p->flow_type & flow_type)
    {
      if(flow_context_p->state != DS_FLOW_STATE_UP)
      {
        DS_ERROR_LOG_0("Flow context is not in up state");
      }
      else
      {
        old_flow_context_p = flow_context_p;
      }
    } /* if flowtype matches*/
    
    bflow_list_p = list_peek_next(&(bflow_manager_p->flow_context),
                                  &(bflow_list_p->bflow_link));
    if(old_flow_context_p)
    {
    
      DS_MSG1(MSG_LEGACY_HIGH, "Del flow context 0x%x", old_flow_context_p);
    
      /*-------------------------------------------------------------------
        Remove the flow context from bearer flow manager.
      ---------------------------------------------------------------------*/
      if(ds_bearer_flow_manager_remove_flow_context_from_flow_manager(
                                                  bflow_manager_p->bearer_ptr,
                                                  old_flow_context_p) == FALSE)
      {
        DS_ERROR_LOG_0("Error in removing the pending context");
      }
      
      /*---------------------------------------------------------------------
        Delete the flow context
      ---------------------------------------------------------------------*/
      ds_flow_release_flow_context(old_flow_context_p);
      old_flow_context_p = NULL;
    } /* old_flow_conext_p != NULL */
  }/* while loop */

  return;
}/*ds_bearer_flow_manager_inst_down_hdlr */


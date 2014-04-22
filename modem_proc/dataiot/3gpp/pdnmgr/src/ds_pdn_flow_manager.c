/*===========================================================================
                      DS_PDN_FLOW_MANGER.C
DESCRIPTION
This file define PDN flow manager APIs.

Copyright (c) 2018 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/3gpp/pdnmgr/src/ds_pdn_flow_manager.c#2 $
  $DateTime: 2023/01/19 23:06:43 $$Author: pwbldsvc $

when           who    what, where, why
--------    ---    ----------------------------------------------------------
08/09/17    sd      First checkin
===========================================================================*/

#include "ds_pdn_defs.h"
#include "ds_pdn_flow_manager.h"
#include "ds_pdn_manager.h"
#include "ds_bearer_manager.h"
#include "ds_bearer_flow_manager.h"
#include "ds_pdn_tft.h"
#include "ds_3gpp_pdn_throttle_sm.h"
#include "ds_pdn_nv_manager.h"
#include "ds_pdn_cp_data_hdlr.h"
#include "ds_pdn_apn_rate_ctrl.h"
#include "ds_flow.h"

/*----------------------------------------------------------------------------
  PDN Flow Manager table. This table contains all the flow managers. This table
  is updated whenever a new flow manager is created or deleted.
  Each flow manager context is tied to a PDN context
----------------------------------------------------------------------------*/

ds_pdn_flow_manager_s *ds_pdn_flow_manager_tbl[DS_3GPP_MAX_PDN_CONTEXT] = {NULL};




/*===========================================================================
FUNCTION DS_PDN_FLOW_MANAGER_ALLOCATE_MEM

DESCRIPTION
  Allocate a new flow manager for this PDN.

PARAMETERS
 ds_pdn_mgr_s     - Pointer to PDN context information.

DEPENDENCIES
  None.

RETURN VALUE
 ds_pdn_flow_manager_s * - Pointer to empty flow manager.
  
SIDE EFFECTS
  None.
===========================================================================*/
ds_pdn_flow_manager_s * ds_pdn_flow_manager_allocate_mem
( 
  ds_pdn_mgr_s *pdn_mgr_p
)
{
  ds_pdn_flow_manager_s  *flow_manager_p = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  flow_manager_p = ds_pdn_flow_manager_tbl[pdn_mgr_p->index];
  
  if(NULL != flow_manager_p)
  {
    DS_SYSTEM_HEAP_MEM_FREE(flow_manager_p);
  }
  DS_SYSTEM_HEAP_MEM_ALLOC(flow_manager_p, sizeof(ds_pdn_flow_manager_s),
  	                       ds_pdn_flow_manager_s*);
  ds_pdn_flow_manager_tbl[pdn_mgr_p->index] = flow_manager_p;
  
  if(NULL == flow_manager_p)
  {
   	return NULL;
  }

  memset(flow_manager_p, 0, sizeof(ds_pdn_flow_manager_s));
  
  flow_manager_p->pdn_mgr_p = pdn_mgr_p;  
  list_init(&(flow_manager_p->bearer_flow_mgr));   
  list_init(&(flow_manager_p->pending_flow_cntx));   

  return flow_manager_p;
} /* ds_pdn_flow_manager_allocate_mem */

/*===========================================================================
FUNCTION ds_pdn_flow_mgr_free_mem

DESCRIPTION
  This function allocates dyn memory.
 
PARAMETERS
 ds_pdn_flow_manager_s - PDN flow mgr ptr 
 
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
=============================================================================*/
void ds_pdn_flow_mgr_free_mem
(
  ds_pdn_flow_manager_s *flow_manager_p   
)
{
  uint8 index = 0;  
  ds_bearer_flow_manager_s   *bearer_flow_mgr_p = NULL;
  ds_bearer_flow_manager_s   *next_bflow_mgr = NULL; 
  ds_pdn_flow_manager_pending_list_type
                             *pend_flow_list_p = NULL;
  ds_pdn_flow_manager_pending_list_type
                             *pend_next_pflow_cntxt = NULL;
  
  if( flow_manager_p != NULL )
  {
    index = flow_manager_p->pdn_mgr_p->index;
    DS_MSG1(MSG_LEGACY_HIGH, 
    	       "Releasing the flow manager associated with PDN %d", index);
    
    /*---------------------------------------------------------------------
      Empty the bearer flow manager list
    ---------------------------------------------------------------------*/          
    bearer_flow_mgr_p = list_peek_front(&(flow_manager_p->bearer_flow_mgr));
    while(bearer_flow_mgr_p != NULL)
    {
      next_bflow_mgr = list_peek_next(&(flow_manager_p->bearer_flow_mgr),
      	                               &(bearer_flow_mgr_p->link));
      list_pop_item(&(flow_manager_p->bearer_flow_mgr),
                    &(bearer_flow_mgr_p->link));
      bearer_flow_mgr_p = next_bflow_mgr;
    }

    /*---------------------------------------------------------------------
        Empty the pending flow context list
      ---------------------------------------------------------------------*/          
    pend_flow_list_p = list_peek_front(&(flow_manager_p->pending_flow_cntx));
    while( pend_flow_list_p && 
           ds_flow_is_valid(pend_flow_list_p->flow_context_p) )
    {
      pend_next_pflow_cntxt = 
                         list_peek_next(&(flow_manager_p->pending_flow_cntx),
                                        &(pend_flow_list_p->pflow_link));
      list_pop_item(&(flow_manager_p->pending_flow_cntx),
                    &(pend_flow_list_p->pflow_link));
      DS_SYSTEM_HEAP_MEM_FREE(pend_flow_list_p);
      
      pend_flow_list_p = pend_next_pflow_cntxt;
    }    

    list_destroy(&(flow_manager_p->bearer_flow_mgr));
    list_destroy(&(flow_manager_p->pending_flow_cntx));
    
    ds_pdn_flow_manager_tbl[index] = NULL;    
    DS_SYSTEM_HEAP_MEM_FREE(flow_manager_p);
  }

  return;
}

/*===========================================================================
FUNCTION DS_PDN_FLOW_MGR_RELEASE_FLOW

DESCRIPTION
  Release the flow manager associated with the PDN connection.

PARAMETERS
  pdn_context_p  - Pointer to the PDN context.

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
===========================================================================*/
void ds_flow_mgr_release_flow_manager
(
  ds_pdn_mgr_s *pdn_mgr_p
)
{
  if(ds_pdn_flow_manager_tbl[pdn_mgr_p->index])
  {
    ds_pdn_flow_mgr_free_mem(ds_pdn_flow_manager_tbl[pdn_mgr_p->index]);
  }
}/* ds_flow_manager_release_flow_manager */

/*===========================================================================
FUNCTION DS_PDN_FLOW_MGR_GET_ENTRY

DESCRIPTION
  Return PDN flow manager pointer

PARAMETERS
 uint8     - Index for which pointer is required.

DEPENDENCIES
  None.

RETURN VALUE
 ds_pdn_flow_manager_s * - Pointer to PDN flow manager.
  
SIDE EFFECTS
  None.
===========================================================================*/
ds_pdn_flow_manager_s * ds_pdn_flow_mgr_get_entry
( 
  uint8   index
)
{
  if(index < DS_3GPP_MAX_PDN_CONTEXT)
  {
    return ds_pdn_flow_manager_tbl[index];
  }

  return NULL;
}

/*===========================================================================
FUNCTION DS_PDN_FLOW_MGR_ADD_BEARER_TO_PDN_FLOW

DESCRIPTION
  Add a bearer context to the flow manager list.

PARAMETERS
 pdn_context_s        - Pointer to PDN Context.
 bearer_context_p     - Pointer to Bearer.

DEPENDENCIES
  None.

RETURN VALUE
 boolean              - Indicates true or false.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_flow_mgr_add_bearer_to_pdn_flow
( 
  ds_pdn_mgr_s    *pdn_mgr_p,
  ds_bearer_mgr_s *bearer_mgr_p  
)
{
  ds_bearer_flow_manager_s *bflow_manager_p = NULL,*temp_bflow_manager_p = NULL; 
  ds_pdn_flow_manager_s    *pflow_manager_p = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  pflow_manager_p = ds_pdn_flow_manager_tbl[pdn_mgr_p->index];
  bflow_manager_p = ds_bearer_flow_manager_find_entry(bearer_mgr_p);
  
  if( NULL == pflow_manager_p || NULL == bflow_manager_p )
  {
    return FALSE;
  }

  // Check to see if the bearer is already part of the list.
  temp_bflow_manager_p = list_peek_front(&(pflow_manager_p->bearer_flow_mgr));
  while(temp_bflow_manager_p)
  {
    if(temp_bflow_manager_p == bflow_manager_p)
    {
      DS_ERROR_LOG_0("Bearer already present in flow manager list");
      return TRUE;
    }

    temp_bflow_manager_p = list_peek_next(&(pflow_manager_p->bearer_flow_mgr),
                                          &(temp_bflow_manager_p->link));
  }

  list_push_front(&(pflow_manager_p->bearer_flow_mgr),
                  &(bflow_manager_p->link));
  bflow_manager_p->pdn_ptr = pdn_mgr_p;
  return TRUE;
} /* ds_pdn_flow_mgr_add_bearer_to_pdn_flow */

/*===========================================================================
FUNCTION DS_PDN_FLOW_MANAGER_REMOVE_BEARER_FROM_PDN_FLOW

DESCRIPTION
  Remove a bearer from the PDN flow manager list.

PARAMETERS
 pdn_context_s       - Pointer to PDN Contex.
 bearer_context_p     - Pointer to Bearer.

DEPENDENCIES
  None.

RETURN VALUE
 boolean              - Indicates true or false.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_flow_manager_remove_bearer_from_pdn_flow
( 
  ds_pdn_mgr_s    *pdn_mgr_p,
  ds_bearer_mgr_s *bearer_mgr_p
)
{
  ds_bearer_flow_manager_s *bflow_manager_p = NULL,*tbflow_manager_p = NULL;
  ds_pdn_flow_manager_s    *pflow_manager_p = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  pflow_manager_p = ds_pdn_flow_manager_tbl[pdn_mgr_p->index];
  bflow_manager_p = ds_bearer_flow_manager_find_entry(bearer_mgr_p);

  if( NULL == pflow_manager_p ||
  	   NULL == bflow_manager_p )
  {
    return FALSE;
  }
  
  // Check to see if the bearer is part of the list.
  tbflow_manager_p = list_peek_front(&(pflow_manager_p->bearer_flow_mgr));
  while(tbflow_manager_p != NULL)
  {
    if(tbflow_manager_p == bflow_manager_p)
    {
      break;
    }
    tbflow_manager_p = list_peek_next(&(pflow_manager_p->bearer_flow_mgr),
    	                                 &(tbflow_manager_p->link));
  }
  
  if(tbflow_manager_p == NULL)
  {
    DS_ERROR_LOG_0("Bearer not present in flow manager list");
    return FALSE;  
  }
  
  list_pop_item(&(pflow_manager_p->bearer_flow_mgr),
                &(bflow_manager_p->link));
  return TRUE;
} /* ds_pdn_flow_manager_remove_bearer_from_pdn_flow */

/*===========================================================================
FUNCTION DS_PDN_FLOW_MANAGER_ADD_PENDING_FLOW_CONTEXT

DESCRIPTION
  Add a flow context to the pending flow context list.

PARAMETERS
 pdn_context_s        - Pointer to PDN Context.
 flow_context_p       - Pointer to Flow Context.

DEPENDENCIES
  None.

RETURN VALUE
 boolean              - Indicates true or false.
  
SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_pdn_flow_manager_add_pending_flow_context
( 
  ds_pdn_mgr_s   *pdn_context_p,
  ds_flow_type   *flow_context_p
  
)
{
  ds_pdn_flow_manager_s    *flow_manager_p = NULL;
  ds_pdn_flow_manager_pending_list_type
                           *pend_flow_list_p = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  if(!ds_pdn_mgr_validate_pdn_ptr(pdn_context_p) || 
     !ds_flow_is_valid(flow_context_p))
  {
    return FALSE;
  }

  flow_manager_p = ds_pdn_flow_mgr_get_entry(pdn_context_p->index);
  if(!flow_manager_p)
  {
    return FALSE;
  }

  // Check if flow context is already present in the list.
  pend_flow_list_p = list_peek_front(&(flow_manager_p->pending_flow_cntx));
  while(pend_flow_list_p &&
        ds_flow_is_valid(pend_flow_list_p->flow_context_p))
  {
    if(pend_flow_list_p->flow_context_p == flow_context_p)
    {
      DS_ERROR_LOG_0("Flow already present in pdn context list");
      return FALSE;
    }
    
    pend_flow_list_p = list_peek_next(&(flow_manager_p->pending_flow_cntx),
                                      &(pend_flow_list_p->pflow_link));
  }
  
  
  DS_SYSTEM_HEAP_MEM_ALLOC(pend_flow_list_p, 
                           sizeof(ds_pdn_flow_manager_pending_list_type),
                           ds_pdn_flow_manager_pending_list_type *);
  if (pend_flow_list_p == NULL)
  {
    DS_MSG0(MSG_LEGACY_ERROR, "Cannot allocate memory for pend_flow_list_p");
    return FALSE;
  }

  memset(pend_flow_list_p, 0, sizeof(ds_pdn_flow_manager_pending_list_type));
  pend_flow_list_p->flow_context_p = flow_context_p;  	
  
  list_push_front(&(flow_manager_p->pending_flow_cntx),
                  &(pend_flow_list_p->pflow_link));
  return TRUE;
} /* ds_pdn_flow_manager_add_pending_flow_context */

/*===========================================================================
FUNCTION DS_PDN_FLOW_MANAGER_FIND_PENDING_FLOW_CONTEXT_FOR_PDN

DESCRIPTION
  Return a pending flow context based on SDF_ID.

PARAMETERS
 pdn_context_s       - Pointer to PDN Contex.
 sdf_id              -  SDF ID to find.

DEPENDENCIES
  None.

RETURN VALUE
 ds_flow_type * - Return valid pointer if flow context exists. Else NULL.
  
SIDE EFFECTS
  None.

===========================================================================*/
ds_flow_type* ds_pdn_flow_manager_find_pending_flow_context_for_pdn
( 
  ds_pdn_mgr_s      *pdn_context_p,
  uint16             sdf_id
)
{
  ds_pdn_flow_manager_s        *flow_manager_p = NULL;
  ds_pdn_flow_manager_pending_list_type
                               *pend_flow_context_p = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  if(!ds_pdn_mgr_validate_pdn_ptr(pdn_context_p))
  {
    return NULL;
  }
  
  flow_manager_p = ds_pdn_flow_mgr_get_entry(pdn_context_p->index);
  if(!flow_manager_p)
  {
    return NULL;
  }
  
  /*---------------------------------------------------------------------
    Find the pending flow context list
  ---------------------------------------------------------------------*/          
  pend_flow_context_p = list_peek_front(&(flow_manager_p->pending_flow_cntx));
  while(pend_flow_context_p &&
        ds_flow_is_valid(pend_flow_context_p->flow_context_p))
  {
    if(pend_flow_context_p->flow_context_p->sdf_id == sdf_id)
    {
      DS_MSG1(MSG_LEGACY_HIGH, "Found flow_context for SDF_ID 0x%x", sdf_id);
      return pend_flow_context_p->flow_context_p;
    }
    
    pend_flow_context_p = list_peek_next(&(flow_manager_p->pending_flow_cntx),
                                         &(pend_flow_context_p->pflow_link));
  }
  
  return NULL;
}/* ds_pdn_flow_manager_find_pending_flow_context_for_pdn */

/*===========================================================================
FUNCTION DS_PDN_FLOW_MANAGER_FIND_PENDING_FLOW_CONTEXT

DESCRIPTION
  Return a pending flow context based on SDF_ID.

PARAMETERS
 pdn_cntx_p       -  PDN pointer - Output pointer
 sdf_id              -  SDF ID to find.

DEPENDENCIES
  None.

RETURN VALUE
 ds_flow_type * - Return valid pointer if flow context exists. Else NULL.
  
SIDE EFFECTS
  None.
===========================================================================*/
ds_flow_type* ds_pdn_flow_manager_find_pending_flow_context
( 
  ds_pdn_mgr_s   **pdn_cntx_p,
  uint16           sdf_id
)
{
  ds_pdn_flow_manager_s    *flow_manager_p = NULL;
  ds_flow_type             *flow_context_p = NULL;
  ds_pdn_mgr_s             *pdn_context_p  = NULL;  
  int                       i = 0;  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  for (i =0; i < DS_3GPP_MAX_PDN_CONTEXT; i++)
  {
    flow_manager_p = ds_pdn_flow_manager_tbl[i];
    if(flow_manager_p)
    {
      pdn_context_p = flow_manager_p->pdn_mgr_p; 
      if(!ds_pdn_mgr_validate_pdn_ptr(pdn_context_p))
      {
        return NULL;
      }
      
      flow_context_p = ds_pdn_flow_manager_find_pending_flow_context_for_pdn(
                                   pdn_context_p,
                                   sdf_id);
      if(flow_context_p)
      {
        *pdn_cntx_p = pdn_context_p;
        return flow_context_p;
      }
    }
  }
  
  return NULL;
}/* ds_pdn_flow_manager_find_pending_flow_context*/

/*===========================================================================
FUNCTION DS_PDN_FLOW_MANAGER_REMOVE_PENDING_FLOW_CONTEXT

DESCRIPTION
  Remove a pending flow context from the flow manager list.

PARAMETERS
 pdn_context_s       - Pointer to PDN Contex.
 flow_context_p     - Pointer to Bearer.

DEPENDENCIES
  None.

RETURN VALUE
 boolean              - Indicates true or false.
  
SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_pdn_flow_manager_remove_pending_flow_context
( 
  ds_pdn_mgr_s    *pdn_context_p,
  ds_flow_type    *flow_context_p
)
{
  ds_pdn_flow_manager_s        *flow_manager_p = NULL;
  ds_pdn_flow_manager_pending_list_type
                               *pend_flow_context_p = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  if(!ds_flow_is_valid(flow_context_p) ||
     !ds_pdn_mgr_validate_pdn_ptr(pdn_context_p))
  {
    return FALSE;
  }

  flow_manager_p = ds_pdn_flow_mgr_get_entry(pdn_context_p->index);
  if(!flow_manager_p)
  {
    return FALSE;
  }
  
  // Check if flow context is present in the list.
  pend_flow_context_p = list_peek_front(&(flow_manager_p->pending_flow_cntx));
  while(pend_flow_context_p &&
        ds_flow_is_valid(pend_flow_context_p->flow_context_p))
  {
    if(pend_flow_context_p->flow_context_p == flow_context_p)
    {
      break;
    }
    
    pend_flow_context_p = list_peek_next(&(flow_manager_p->pending_flow_cntx),
                                         &(pend_flow_context_p->pflow_link));
  }
  
  if(pend_flow_context_p == NULL)
  {
    DS_ERROR_LOG_0("Flow not present in pending flow context list");
    return FALSE;
  }
  
  list_pop_item(&(flow_manager_p->pending_flow_cntx),
                &(pend_flow_context_p->pflow_link));
  DS_SYSTEM_HEAP_MEM_FREE(pend_flow_context_p);
  return TRUE;
} /* ds_pdn_flow_manager_remove_pending_flow_context*/

/*===========================================================================
FUNCTION      ds_pdn_flow_mgr_decode_bcm

DESCRIPTION
  This function decodes the bearer control mode info

PARAMETERS
  Protocol info ptr
  Protocol length
  PDN index
  
DEPENDENCIES  None.

RETURN VALUE  TRUE - IF BCM decoded successfully
                       FALSE - otherwise

SIDE EFFECTS  None.
===========================================================================*/
boolean ds_pdn_flow_mgr_decode_bcm
(
  const byte       *protocol_pkt_ptr, /* Pointer to the buffer for bcm info */
  int               protocol_pkt_len,  /* Length of the bcm container */
  uint8             index
)
{
  ds_pdn_flow_manager_s *flow_manager_p;

  if(protocol_pkt_len != 1)
  {
    return FALSE;
  }

  flow_manager_p = ds_pdn_flow_manager_tbl[index];
  if(!flow_manager_p)
  {
    return FALSE;
  }
  
  switch(*protocol_pkt_ptr)
  {
    case 0x1:
      flow_manager_p->bcm_info = DS_3GPP_BCM_UE_ONLY;
      break;
    case 0x2:
      flow_manager_p->bcm_info = DS_3GPP_BCM_UE_NW;
      break;
    default:
      break;
  }

  DS_MSG1(MSG_LEGACY_HIGH,"ds_pdn_flow_mgr_decode_bcm = %d",
  	       flow_manager_p->bcm_info);
  return TRUE;
}/* ds_pdn_flow_mgr_decode_bcm */

/*===========================================================================
FUNCTION ds_pdn_flow_mgr_update_uplink_flow_capability 

DESCRIPTION
  This function is used to enable or disable uplink flow based on 
  flow direction (This is based on GBR values granted by NW) 

PARAMETERS
 bearer_mgr_p: Bearer for which data path is to be setup
 boolean:  to Enable or Disable flow
 
DEPENDENCIES
  None.

RETURN VALUE
None
  
SIDE EFFECTS
  None.
===========================================================================*/
void ds_pdn_flow_mgr_update_uplink_flow_capability
(
  ds_bearer_mgr_s *bearer_mgr_p,
  boolean          ul_flow_enable
)
{
  ds_pdn_mgr_s       *pdn_mgr_p = NULL;
  ds_flow_type       *v4_flow_cntx_p = NULL;
  ds_flow_type       *v6_flow_cntx_p = NULL;
  /*-----------------------------------------------------  */
  
  pdn_mgr_p = bearer_mgr_p->pdn_mgr_ptr;
  if(!ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p))
  {
    return;
  }
  
  /* Get V4 &V6flow context pointers */
  v4_flow_cntx_p = ds_bearer_flow_mgr_find_flow_context(bearer_mgr_p,
                                                        DS_FLOW_V4_FLOW_TYPE);  
  v6_flow_cntx_p = ds_bearer_flow_mgr_find_flow_context(bearer_mgr_p,
                                                        DS_FLOW_V6_FLOW_TYPE);
    
  if (ul_flow_enable)
  {
    DS_MSG0(MSG_LEGACY_HIGH, 
    	       "Enabling bearer flow and resetting flow Capability");

    /* Enable bearer flow */
    ds_bearer_mgr_enable_tx_flow(bearer_mgr_p, DS_BEARER_CONTROL_MASK);
	
    /* Check validity of flow_ptrs  reset the flow capabiltiy */
    if (v4_flow_cntx_p)
    {
      ds_flow_reset_capability(v4_flow_cntx_p, 
                               DS_FLOW_CAPABILITY_DATA_DISALLOWED);
    }
  
    if (v6_flow_cntx_p)
    {
      ds_flow_reset_capability(v6_flow_cntx_p, 
                               DS_FLOW_CAPABILITY_DATA_DISALLOWED);
    }
  }
  else
  {
    DS_MSG0(MSG_LEGACY_HIGH, 
    	       "Disabling Phys_link flow and setting PS flow Capability");
    
    /* Disable bearer flow */
    ds_bearer_mgr_disable_tx_flow(bearer_mgr_p, DS_BEARER_CONTROL_MASK);
	 
     /* Check validity of flow_ptrs  set the flow capabiltiy */
    if (v4_flow_cntx_p)
    {
      ds_flow_set_capability(v4_flow_cntx_p, 
                             DS_FLOW_CAPABILITY_DATA_DISALLOWED);
    }
  
    if (v6_flow_cntx_p)
    {
      ds_flow_set_capability(v6_flow_cntx_p, 
                             DS_FLOW_CAPABILITY_DATA_DISALLOWED);
    }
  }
  
  return;  
}/* ds_pdn_flow_mgr_update_uplink_flow_capability */

/*===========================================================================
FUNCTION DS_PDN_FLOW_MANAGER_UPDATE_APN_AMBR

DESCRIPTION
  Update the APN AMBR with the network request.

PARAMETERS
  apn_ambr_ptr    -  NW passed AMBR info.
  pdn_context_p   -  PDN context pointer

DEPENDENCIES
  None.

RETURN VALUE
  Returns True if successfully updated.
  
SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_pdn_flow_manager_update_apn_ambr
(
  apn_ambr_T           *apn_ambr,
  ds_pdn_mgr_s         *pdn_mgr_p
)
{
  ds_pdn_flow_manager_s        *pflow_manager_p = NULL;
  /*-----------------------------------------------------  */

  if(apn_ambr != NULL &&
     apn_ambr->valid == TRUE)
  {
    pflow_manager_p = ds_pdn_flow_mgr_get_entry(pdn_mgr_p->index);
    if(!pflow_manager_p)
    {
      return FALSE;
    }
  
    memscpy(&pflow_manager_p->eps_pdn_ambr, sizeof(apn_ambr_T),
            apn_ambr, sizeof(apn_ambr_T));
    return TRUE;
  }

  return FALSE;
}/* ds_pdn_flow_manager_update_apn_ambr */

/*===========================================================================
 FUNCTION DS_PDN_FLOW_MANAGER_CALCULATE_PREFIMASK

DESCRIPTION
  This functions calculates ipv6 prefix mask given the prefix length

PARAMETERS
 address_mask       -   Pointer to the ipv6 address array. 
 prefix length      -   ipv6 prefix length


DEPENDENCIES
  None.

RETURN VALUE
 None
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_flow_manger_calcuate_prefixmask
(
  uint8      address_mask[16],
  uint8      prefix_len
)
{
  uint8      offset = 0;
  uint8      remainder = 0;
  uint8      num = 0;
  uint8      index = 0;


  offset = prefix_len/8;
  remainder = prefix_len % 8;

  /* Example prefix length */
  /* 64-> ffff:ffff:ffff:ffff:0:0:0:0)
  /* most significant bit stored in index 0 */
  
  for(index =0; index < 16;index++)
  {
    address_mask[index] = 0xFF;
  }

  for (index = (remainder?(offset+1):offset); index < 16; index++)
  {
    address_mask[index] = 0;
  }
  
  while (num < remainder)
  {
    address_mask[offset] &= ~(1 << num);
    num++;
  }
}

/*===========================================================================
FUNCTION DS_PDN_FLOW_MANAGER_CONVERT_NW_FILTER_TO_APP_FILTER

DESCRIPTION
  Converts the NW passed filters spec to App filter spec.

PARAMETERS
 nw_filter            -  network filter to be converted
 ip_filter            -  App filter to store the converted info.
 filter_id            -  filter_id passed by NW
 direction            -  direction in which the filter should be applied.
 flow_context_p       -  Pointer to the flow context

DEPENDENCIES
  None.

RETURN VALUE
boolean          - returns if conversion is successfull
  
SIDE EFFECTS
  None.

===========================================================================*/
/*lint -save -e655 Bitwise operation on compatible enums */
boolean ds_pdn_flow_manager_convert_nw_filter_to_app_filter
(
  packet_filter_type_T                           *nw_filter,
  ip_filter_type                                 *ip_filter,
  byte                                            filter_id,
  ds_pdn_tft_pkt_filter_direction_enum_type       direction,
  ds_pdn_mgr_instance_type_e                      ip_type,
  ds_flow_type                                   *flow_context_p,
  ds_bearer_flow_internal_cause_code_enum_type   *cause_code_p
)
{
  int                               inc = 0;
  byte                              filter_length = 0;
  byte                             *filter_contents = NULL;
  ds_pdn_tft_comp_type_id_enum_type filter_type = DS_PDN_TFT_EMPTY_TYPE;
  uint32                            ip_address = 0;
  uint32                            subnet_address = 0;
  byte                              protocol_identifier =0;
  uint16                            port_number = 0;
  uint16                            port_range = 0;
  byte                              temp_prefix = 0;
  int                               ip6_inc =0;
  ds_pdn_mgr_s                     *pdn_ptr  = NULL;
  ps_ip_addr_type                   ip_addr;
  ps_ip_addr_type                   ipv6_addr_mask;
  ds_pdn_mgr_inst_type_s           *inst_p  = NULL;
  uint32                            esp_spi = 0; uint32 flow_label =0;
  boolean                           prefix_len_found = FALSE;
  ds_pdn_tft_filter_mask_enum_type  filter_mask = DS_PDN_TFT_FILTER_NONE;
#ifdef FEATURE_DATA_REL11
  ds_pdn_tft_filter_mask_enum_type  local_address_mask = DS_PDN_TFT_FILTER_NONE;
#endif /* FEATURE_DATA_REL11 */
  ds_local_err_val_e_type           local_err = DS_LOCAL_ERR_VAL_NONE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_MSG2(MSG_LEGACY_HIGH, "Convert NW filters ID=%d and direction is %d",
          filter_id, (int)direction);
  
  if(cause_code_p == NULL || nw_filter == NULL || ip_filter == NULL)
  {
    local_err = DS_LOCAL_ERR_VAL_0;
    *cause_code_p = DS_BEARER_FLOW_INTERNAL_INVALID_MANDATORY_INFO;
    goto func_exit;
  }

  filter_length = nw_filter->content_length;
  filter_contents = (nw_filter->filter_contents);
 
  if((filter_length >= MAX_FILTER_CONTENT_LENGTH) ||
     (filter_length == 0))
  { 
    local_err = DS_LOCAL_ERR_VAL_1;
    *cause_code_p = DS_BEARER_FLOW_INTERNAL_SYNTACTICAL_ERR_IN_FILTER;
    goto func_exit;
  }
 
  /*----------------------------------------------------------------------
     Copy the filter ID and filter precedence
     Filter ID should be unique.
  -----------------------------------------------------------------------*/   
  if(flow_context_p != NULL)
  {
    ip_filter->ipfltr_aux_info.fi_id=
                       ((flow_context_p->sdf_id & 0xFF) << 8) | filter_id;
  }
  else
  {
    ip_filter->ipfltr_aux_info.fi_id = filter_id;
  }
  
  ip_filter->ipfltr_aux_info.fi_precedence = nw_filter->eval_precedence;

  /*----------------------------------------------------------------------
     Initialize the Filter Version
   -----------------------------------------------------------------------*/   
  ip_filter->ip_vsn = ds_pdn_mgr_convert_pdn_inst_to_fw_ip_type(ip_type);

  while(inc < filter_length)
  {
    filter_type =(ds_pdn_tft_comp_type_id_enum_type)filter_contents[inc];
    if( !ds_pdn_nv_manager_is_rel_11_or_above_supported())
    {
      if((filter_type == DS_PDN_TFT_IPV4_LOCAL_ADDR)||
         (filter_type == DS_PDN_TFT_IPV6_REM_PRE_ADDR) ||
         (filter_type == DS_PDN_TFT_IPV6_LOCAL_PRE_ADDR))
      {
        local_err = DS_LOCAL_ERR_VAL_2;
        *cause_code_p = DS_BEARER_FLOW_INTERNAL_SYNTACTICAL_ERR_IN_FILTER;
        goto func_exit;
      }
    }
    
    inc++;
    switch(filter_type)
    {
      case DS_PDN_TFT_IPV4_ADDR:
        /* check if the two remote ip address are received */
        if( ((uint32)filter_mask & (uint32)DS_PDN_TFT_FILTER_IP_ADDR_TYPE)
#ifdef FEATURE_DATA_REL11
            && (filter_type == DS_PDN_TFT_IPV4_ADDR)
#endif /* FEATURE_DATA_REL11 */
          )
        {
          local_err = DS_LOCAL_ERR_VAL_3;
          *cause_code_p = DS_BEARER_FLOW_INTERNAL_SEMANTIC_ERR_IN_FILTER;
          goto func_exit;
        }
        else
#ifdef FEATURE_DATA_REL11
        if (filter_type == DS_PDN_TFT_IPV4_ADDR)
#endif /* FEATURE_DATA_REL11 */
        {
          filter_mask =(ds_pdn_tft_filter_mask_enum_type)
               ((uint32)filter_mask |(uint32)DS_PDN_TFT_FILTER_IP_ADDR_TYPE);
        }

#ifdef FEATURE_DATA_REL11
      case DS_PDN_TFT_IPV4_LOCAL_ADDR:
        /* check if the two local ip address are received */
        if((((uint32)local_address_mask & 
                          (uint32)DS_PDN_TFT_FILTER_IP_LOCAL_ADDR_TYPE) &&
             filter_type == DS_PDN_TFT_IPV4_LOCAL_ADDR ))
        {
          local_err = DS_LOCAL_ERR_VAL_4;
          *cause_code_p = DS_BEARER_FLOW_INTERNAL_SEMANTIC_ERR_IN_FILTER;
          goto func_exit;
        }

        /* set the local address mask if local address is received */
        if(filter_type == DS_PDN_TFT_IPV4_LOCAL_ADDR)
        {
          local_address_mask =(ds_pdn_tft_filter_mask_enum_type)
            ((uint32)local_address_mask  | 
             (uint32)DS_PDN_TFT_FILTER_IP_LOCAL_ADDR_TYPE);
          DS_MSG1(MSG_LEGACY_HIGH, "local addresss mask %d",
                  (uint32)local_address_mask);
        }
#endif /* FEATURE_DATA_REL11 */
        
        /* checking if the filter length meets the ipv4 networking standards */
        if(filter_length < (inc+8))
        {
          local_err = DS_LOCAL_ERR_VAL_5;
          *cause_code_p = DS_BEARER_FLOW_INTERNAL_SYNTACTICAL_ERR_IN_FILTER;
          goto func_exit;
        }
        else
        {
          /*-----------------------------------------------------------------
            Overwrite the IP Version.
          -----------------------------------------------------------------*/  
          if(ip_filter->ip_vsn != IP_V4)
          {
            memset(&(ip_filter->ip_hdr),0, sizeof(ip_filter->ip_hdr));
            memset(&(ip_filter->next_prot_hdr),0, sizeof(ip_filter->next_prot_hdr));
            filter_mask = DS_PDN_TFT_FILTER_NONE;
#ifdef FEATURE_DATA_REL11
            local_address_mask = DS_PDN_TFT_FILTER_NONE;
#endif
            inc = 0;
            ip_filter->ip_vsn = IPV4_ADDR;
            DS_MSG3(MSG_LEGACY_HIGH,
                    "Resetting Ip version: %d for filter type:%d",
	                   (int)ip_filter->ip_vsn,(int)filter_type,0); 
            break;
          }
          ip_address =(uint32)(filter_contents[inc++] << 24);
          ip_address |=(uint32)(filter_contents[inc++] <<16);
          ip_address |=(uint32)(filter_contents[inc++] <<8);
          ip_address |=(uint32)(filter_contents[inc++]);
      
          subnet_address =(uint32)(filter_contents[inc++] << 24);
          subnet_address |=(uint32)(filter_contents[inc++] <<16);
          subnet_address |=(uint32)(filter_contents[inc++] <<8);
          subnet_address |=(uint32)(filter_contents[inc++]);
          if(direction == DS_PDN_TFT_DOWNLINK)
          {
#ifdef FEATURE_DATA_REL11
            if(filter_type == DS_PDN_TFT_IPV4_ADDR)
#endif /* FEATURE_DATA_REL11 */
            {
              ip_filter->ip_hdr.v4.src.addr.ps_s_addr = ps_ntohl(ip_address);
              ip_filter->ip_hdr.v4.src.subnet_mask.ps_s_addr =
                                                       ps_ntohl(subnet_address);
              ip_filter->ip_hdr.v4.field_mask |=(int)IPFLTR_MASK_IP4_SRC_ADDR;
              DS_MSG3(MSG_LEGACY_HIGH,
                      "Recvd downlink IPv4 Address remote address:0x%x"
                      "and subnet mask 0x%x, setting downlink field mask to %d",
                      ip_filter->ip_hdr.v4.src.addr.ps_s_addr,
                      ip_filter->ip_hdr.v4.src.subnet_mask.ps_s_addr, 
                      ip_filter->ip_hdr.v4.field_mask);
            }
#ifdef FEATURE_DATA_REL11
           /*------------------------------------------------------------------ 
              if the direction is downlink and received filter type is local 
              address then update destination parameters and set the destination 
              field mask.
              (Destination)(local) UE <--downlink---( Source )(remote) Server 
            ------------------------------------------------------------------*/
            if(filter_type == DS_PDN_TFT_IPV4_LOCAL_ADDR)
            {
              ip_filter->ip_hdr.v4.dst.addr.ps_s_addr = ps_ntohl(ip_address);
              ip_filter->ip_hdr.v4.dst.subnet_mask.ps_s_addr =
                                                       ps_ntohl(subnet_address);
              ip_filter->ip_hdr.v4.field_mask |=(int)IPFLTR_MASK_IP4_DST_ADDR;
              DS_MSG3(MSG_LEGACY_HIGH,
                      "Recvd downlink IPv4 Address local address:0x%x"
                      "and subnet mask 0x%x, setting downlink field mask to %d",
                      ip_filter->ip_hdr.v4.dst.addr.ps_s_addr,
                      ip_filter->ip_hdr.v4.dst.subnet_mask.ps_s_addr,
                      ip_filter->ip_hdr.v4.field_mask);
            }
#endif /* FEATURE_DATA_REL11 */
          }
          if(direction == DS_PDN_TFT_UPLINK)
          {
#ifdef FEATURE_DATA_REL11
            if(filter_type == DS_PDN_TFT_IPV4_ADDR)
#endif /* FEATURE_DATA_REL11 */
            {
              ip_filter->ip_hdr.v4.dst.addr.ps_s_addr =ps_ntohl(ip_address);
              ip_filter->ip_hdr.v4.dst.subnet_mask.ps_s_addr = ps_ntohl(subnet_address);
              ip_filter->ip_hdr.v4.field_mask |=(int)IPFLTR_MASK_IP4_DST_ADDR;
              DS_MSG2(MSG_LEGACY_HIGH, 
                      "Recvd uplink IPv4 Address remote address:0x%x"
                      "and subnet mask 0x%x",
                      ip_filter->ip_hdr.v4.dst.addr.ps_s_addr,
                      ip_filter->ip_hdr.v4.dst.subnet_mask.ps_s_addr);
            }
#ifdef FEATURE_DATA_REL11
            /*----------------------------------------------------------------
            if the direction is uplink and received filter type is local 
            address then update source parameters and set the source 
            field mask 
            (Source)(local) UE --uplink-->( Destination )(remote) Server 
             ----------------------------------------------------------------*/
            if(filter_type == DS_PDN_TFT_IPV4_LOCAL_ADDR)
            {
              ip_filter->ip_hdr.v4.src.addr.ps_s_addr =ps_ntohl(ip_address);
              ip_filter->ip_hdr.v4.src.subnet_mask.ps_s_addr = ps_ntohl(subnet_address);
              ip_filter->ip_hdr.v4.field_mask |=(int)IPFLTR_MASK_IP4_SRC_ADDR;
              DS_MSG2(MSG_LEGACY_HIGH,
                      "Recvd uplink IPv4 Address local address:0x%x"
                      "and subnet mask 0x%x",
                      ip_filter->ip_hdr.v4.src.addr.ps_s_addr,
                      ip_filter->ip_hdr.v4.src.subnet_mask.ps_s_addr);
            }
#endif /* FEATURE_DATA_REL11 */
          }
          if(((ip_filter->ip_hdr.v4.field_mask &(int)IPFLTR_MASK_IP4_SRC_ADDR) 
             == IPFLTR_MASK_IP4_SRC_ADDR) && (direction == DS_PDN_TFT_UPLINK))
          {
            if(!flow_context_p)
            {
              if(flow_context_p->bearer_ptr != NULL &&
                 flow_context_p->bearer_ptr->pdn_mgr_ptr != NULL)
              {
                pdn_ptr = flow_context_p->bearer_ptr->pdn_mgr_ptr;
                /* getting pdn context and instance; this is because we install 
                                  same filter on both v4 and v6 flow contexts */
                inst_p = pdn_ptr->inst_ptr[DS_PDN_INSTANCE_IPV4];
                if(inst_p != NULL )
                {
                  memset(&ip_addr,0,sizeof(ps_ip_addr_type));
                  ds_fwk_get_ip_addr(ds_fwk_get_inst_ptr_from_index(
                                                          pdn_ptr->fw_index),
                                     IPV4_ADDR,
                                     &ip_addr);
                  if (ip_addr.type == IPV4_ADDR)
                  {
                    DS_MSG3(MSG_LEGACY_HIGH,
                            "Recvd uplink IPv4 Address local address:%d "
                            "and subnet mask %d, IPv4 addr : %d",
                            ip_filter->ip_hdr.v4.src.addr.ps_s_addr,
                            ip_filter->ip_hdr.v4.src.subnet_mask.ps_s_addr,
                            ip_addr.addr.v4.ps_s_addr);
                    /* most significant bit stored in index 0 */
                    if((ip_addr.addr.v4.ps_s_addr & 
                        ip_filter->ip_hdr.v4.src.subnet_mask.ps_s_addr) != 
                       (ip_filter->ip_hdr.v4.src.addr.ps_s_addr & 
                        ip_filter->ip_hdr.v4.src.subnet_mask.ps_s_addr))
                    {
                      local_err = DS_LOCAL_ERR_VAL_6;
                      *cause_code_p = 
                                 DS_BEARER_FLOW_INTERNAL_SEMANTIC_ERR_IN_FILTER;
                      goto func_exit;
                     }
                   }
                 }
               }
             }
          } 
        }/* else filter_length */
        break;
        
      case DS_PDN_TFT_IPV6_ADDR:
        /* check if the two remote ip address are received */
        if((uint32)filter_mask & (uint32)DS_PDN_TFT_FILTER_IP_ADDR_TYPE
#ifdef FEATURE_DATA_REL11
            && filter_type == DS_PDN_TFT_IPV6_ADDR
#endif /* FEATURE_DATA_REL11 */
          )
        {
          local_err = DS_LOCAL_ERR_VAL_7;
          *cause_code_p = DS_BEARER_FLOW_INTERNAL_SEMANTIC_ERR_IN_FILTER;
          goto func_exit;
        }
        else
#ifdef FEATURE_DATA_REL11
        if (filter_type == DS_PDN_TFT_IPV6_ADDR)
#endif /* FEATURE_DATA_REL11 */
        {
          filter_mask =(ds_pdn_tft_filter_mask_enum_type)
                   ((uint32)filter_mask|(uint32)DS_PDN_TFT_FILTER_IP_ADDR_TYPE);
        }

#ifdef FEATURE_DATA_REL11
      case DS_PDN_TFT_IPV6_REM_PRE_ADDR:
      case DS_PDN_TFT_IPV6_LOCAL_PRE_ADDR:
        /* check if the two local ip address/prefix type are received */
        if(((uint32)local_address_mask & 
            (uint32)DS_PDN_TFT_FILTER_IP_LOCAL_PREFIX_TYPE)
           && filter_type == DS_PDN_TFT_IPV6_LOCAL_PRE_ADDR )
        {
          local_err = DS_LOCAL_ERR_VAL_8;
          *cause_code_p = DS_BEARER_FLOW_INTERNAL_SEMANTIC_ERR_IN_FILTER;
          goto func_exit;
        }
        /* check if the two remote ip address /prefix type are received */
        if(((uint32)filter_mask & 
             (uint32)DS_PDN_TFT_FILTER_IP_REMOTE_PREFIX_TYPE)
             && filter_type == DS_PDN_TFT_IPV6_REM_PRE_ADDR)
        {
          local_err = DS_LOCAL_ERR_VAL_9;
          *cause_code_p = DS_BEARER_FLOW_INTERNAL_SEMANTIC_ERR_IN_FILTER;
          goto func_exit;
        }

       /*---------------------------------------------------------------------------
        Case 1: 
        Old address format received after new address format: Here we are checking if 
        previous component  received was remote/prefix typeand current component  is 
        remote address,if "yes" then we ignore remote address and continue with other 
        components else we continue with remote address 
        ---------------------------------------------------------------------------*/
        if(filter_type == DS_PDN_TFT_IPV6_ADDR)
        {
          if((uint32)filter_mask & 
             (uint32)DS_PDN_TFT_FILTER_IP_REMOTE_PREFIX_TYPE)
          {
            DS_MSG0(MSG_LEGACY_HIGH,
            "Received a filter of remote type DS_3GPP_TFT_IPV6_ADDR ignore"
            " since DS_3GPP_FILTER_IP_REMOTE_PREFIX_TYPE is already received");
            inc = inc+32;
            continue;
          }
        }

         /*----------------------------------------------------------------------------
            Case 2: New address format received after Old address format 
            Check 1:outer if checks if the current component ipv6 remote/prefix 
            address type.
            Check 2:internal if statement check if previously received component 
             was remote address or not
          ----------------------------------------------------------------------------*/
        if(filter_type == DS_PDN_TFT_IPV6_REM_PRE_ADDR)
        { 
          if((uint32)filter_mask & (uint32)DS_PDN_TFT_FILTER_IP_ADDR_TYPE)
          {
            /* -------------------------------------------------------------------------
             if both the checks are true which means we replace the remote address type 
             with remote/prefix type according to 3gpp spec and reset the field masks 
             --------------------------------------------------------------------------*/
            if(direction == DS_PDN_TFT_DOWNLINK )
            {
              memset(&(ip_filter->ip_hdr.v6.src),0, sizeof(ip_filter->ip_hdr.v6.src));
              ip_filter->ip_hdr.v6.field_mask ^= IPFLTR_MASK_IP6_SRC_ADDR;
            }
            if(direction == DS_PDN_TFT_UPLINK)
            {
              memset(&(ip_filter->ip_hdr.v6.dst),0, sizeof(ip_filter->ip_hdr.v6.dst));
              ip_filter->ip_hdr.v6.field_mask ^= IPFLTR_MASK_IP6_DST_ADDR;
            }
          }
        }

        /*---------------------------------------------------------------------------------
         set the filter mask to remote/prefix type and enable the received_prefix_from_network
         flag if the filter type is remote/prefix address type 
        ----------------------------------------------------------------------------------*/
        if(filter_type == DS_PDN_TFT_IPV6_REM_PRE_ADDR)
        {
          filter_mask =(ds_pdn_tft_filter_mask_enum_type)
                ((uint32)filter_mask|
                 (uint32)DS_PDN_TFT_FILTER_IP_REMOTE_PREFIX_TYPE);
        }
        /*---------------------------------------------------------------------------------
        set the filter mask to local /prefix type and enable the received_prefix_from_network
                   flag  if the filter type is local/prefix address type.
        ----------------------------------------------------------------------------------*/
        if(filter_type == DS_PDN_TFT_IPV6_LOCAL_PRE_ADDR)
        {
          local_address_mask = (ds_pdn_tft_filter_mask_enum_type)
                ((uint32)local_address_mask|
                 (uint32)DS_PDN_TFT_FILTER_IP_LOCAL_PREFIX_TYPE);
        }

        /* --------------------------------------------------------------------------------
           checks if the filter length meets the ipv6 networking standards 
           since in remote/local prefix type first 16 octets is address and 1 octet is prefix 
         ---------------------------------------------------------------------------------*/
        if(filter_type == DS_PDN_TFT_IPV6_REM_PRE_ADDR || 
           filter_type == DS_PDN_TFT_IPV6_LOCAL_PRE_ADDR)
        {
          if(filter_length < (inc+17))
          {
            local_err = DS_LOCAL_ERR_VAL_10;
            *cause_code_p = DS_BEARER_FLOW_INTERNAL_SYNTACTICAL_ERR_IN_FILTER;
            goto func_exit;
          }
        }
       
        /*----------------------------------------------------------------------------------
           checks if the filter length meets the ipv6 networking standards 
           since first 16 octets is address and next 16 octets is the prefix 
        -----------------------------------------------------------------------------------*/
        if(filter_type == DS_PDN_TFT_IPV6_ADDR)
#endif /* FEATURE_DATA_REL11 */
        {
          if(filter_length < (inc+32))
          {
            local_err = DS_LOCAL_ERR_VAL_11;
            *cause_code_p = DS_BEARER_FLOW_INTERNAL_SYNTACTICAL_ERR_IN_FILTER;
            goto func_exit;
          }
        }

        /*-----------------------------------------------------------------
          Overwrite the IP Version.
        -----------------------------------------------------------------*/         
        if(ip_filter->ip_vsn != IP_V6)
        {
          memset(&(ip_filter->ip_hdr),0, sizeof(ip_filter->ip_hdr));
          memset(&(ip_filter->next_prot_hdr),0, sizeof(ip_filter->next_prot_hdr));
          filter_mask = DS_PDN_TFT_FILTER_NONE;
#ifdef FEATURE_DATA_REL11
          local_address_mask = DS_PDN_TFT_FILTER_NONE;
#endif
          inc = 0;
          ip_filter->ip_vsn = IPV6_ADDR;
          DS_MSG2(MSG_LEGACY_HIGH, 
                  "Resetting Ip version: %d for filter type:%d",
	                 (int)ip_filter->ip_vsn,(int)filter_type); 
          break;
        }
        
        if(direction == DS_PDN_TFT_DOWNLINK)
        {
          for(ip6_inc=0;ip6_inc<16;ip6_inc++)
          {
#ifdef FEATURE_DATA_REL11
            /*------------------------------------------------------------------ 
            if the direction is downlink and received filter type is remote 
            address then update destination parameters and set the destination 
            field mask.
            (Destination)(local) UE <--downlink---( Source )(remote) Server 
            ------------------------------------------------------------------*/
            if( (filter_type == DS_PDN_TFT_IPV6_ADDR) ||
                (filter_type == DS_PDN_TFT_IPV6_REM_PRE_ADDR) )
#endif /* FEATURE_DATA_REL11 */
            {
              ip_filter->ip_hdr.v6.src.addr.in6_u.u6_addr8[ip6_inc] =
                          filter_contents[inc];
              inc++;
            }
#ifdef FEATURE_DATA_REL11
          /*------------------------------------------------------------------ 
          if the direction is downlink and received filter type is local 
          address then update destination parameters and set the destination 
          field mask.
          (Destination)(local) UE <--downlink---( Source )(remote) Server 
            ------------------------------------------------------------------*/
            if(filter_type == DS_PDN_TFT_IPV6_LOCAL_PRE_ADDR)
            {
              ip_filter->ip_hdr.v6.dst.addr.in6_u.u6_addr8[ip6_inc] =
                        filter_contents[inc];
              inc++;
            }
#endif /* FEATURE_DATA_REL11 */
          }

#ifdef FEATURE_DATA_REL11
     /*----------------------------------------------------------------------------
        check if we have received a prefix length from network 
        incase of remote address type we need to compute it 
        incase of remote/local prefix address type network provides us 
        with the ipv6 prefix (first 16 octets is address and 17th octet is prefix  
     -----------------------------------------------------------------------------*/ 
        if(filter_type == DS_PDN_TFT_IPV6_ADDR)
        {
#endif /* FEATURE_DATA_REL11 */
          prefix_len_found = FALSE;
          for(ip6_inc=0;ip6_inc<16;ip6_inc++)
          {
            if(prefix_len_found == FALSE)
            {
              if(filter_contents[inc] != 0xFF)
              {
                ip_filter->ip_hdr.v6.src.prefix_len = ip6_inc * 8;
                temp_prefix = (1 << 7);
                if (filter_contents[inc] != 0)
                {
                  while((filter_contents[inc] & temp_prefix) == temp_prefix)
                  {
                    temp_prefix = temp_prefix >> 1;
                    ip_filter->ip_hdr.v6.src.prefix_len++; 
                  } 
                  inc++; 
                }
                else
                {
                  inc++;
                }
                prefix_len_found = TRUE;
                DS_MSG1(MSG_LEGACY_HIGH, "prefix length %d",
                        ip_filter->ip_hdr.v6.src.prefix_len );
              }
              else
              {
                inc++;
              }
            }  
            else
            {
              inc++;
            }
          }/* for */
#ifdef FEATURE_DATA_REL11
          }
          else
          { 
            /* if the 17th octet is not null then update the coressponding src/dst contents */
            if(filter_contents[inc] != 0)
            {
              if(filter_type == DS_PDN_TFT_IPV6_REM_PRE_ADDR )
              {
                ip_filter->ip_hdr.v6.src.prefix_len = filter_contents[inc];
                inc++;
                DS_MSG1(MSG_LEGACY_HIGH,
                        "remote prefix length %d <- src",
                        ip_filter->ip_hdr.v6.src.prefix_len );
              }
              if(filter_type == DS_PDN_TFT_IPV6_LOCAL_PRE_ADDR)
              {
                ip_filter->ip_hdr.v6.dst.prefix_len = filter_contents[inc];
                inc++;
                DS_MSG1(MSG_LEGACY_HIGH,
                        "local prefix length %d <- dst",
                        ip_filter->ip_hdr.v6.dst.prefix_len );
              }
            }
           /* otherwise use the default value 128 to update */
            else
            {
              if(filter_type == DS_PDN_TFT_IPV6_REM_PRE_ADDR)
              {
                ip_filter->ip_hdr.v6.src.prefix_len = 128;
                inc++;
                DS_MSG1(MSG_LEGACY_HIGH,
                        "remote prefix length %d <- src",
                        ip_filter->ip_hdr.v6.src.prefix_len );
              }
              if(filter_type == DS_PDN_TFT_IPV6_LOCAL_PRE_ADDR)
              {
                ip_filter->ip_hdr.v6.dst.prefix_len = 128;
                inc++;
                DS_MSG1(MSG_LEGACY_HIGH, "local prefix length %d <- dst",
                        ip_filter->ip_hdr.v6.dst.prefix_len );
              }
            }
          }
#endif /* FEATURE_DATA_REL11 */
          if(prefix_len_found == FALSE && filter_type == DS_PDN_TFT_IPV6_ADDR )
          {
            ip_filter->ip_hdr.v6.src.prefix_len = 128;
          }
          /* set the field mask to src address(downlink) if remote address is received otherwise
                      set local address mask */
#ifdef FEATURE_DATA_REL11
          if(filter_type == DS_PDN_TFT_IPV6_REM_PRE_ADDR || 
             filter_type == DS_PDN_TFT_IPV6_ADDR)
          { 
#endif /* FEATURE_DATA_REL11 */
          ip_filter->ip_hdr.v6.field_mask |=(int)IPFLTR_MASK_IP6_SRC_ADDR;
#ifdef FEATURE_DATA_REL11
          }
          if(filter_type == DS_PDN_TFT_IPV6_LOCAL_PRE_ADDR)
          {
            ip_filter->ip_hdr.v6.field_mask |=(int)IPFLTR_MASK_IP6_DST_ADDR;
          }
#endif /* FEATURE_DATA_REL11 */
          DS_MSG0(MSG_LEGACY_HIGH,
                  "Received IPv6 Address and subnet mask for downlink");
        } /* DS_3GPP_TFT_DOWNLINK*/
        if(direction == DS_PDN_TFT_UPLINK)
        {
          for(ip6_inc=0;ip6_inc<16;ip6_inc++)
          {
#ifdef FEATURE_DATA_REL11
         /*------------------------------------------------------------------
         if the direction is uplink and received filter type is remote 
         address then update source parameters and set the source 
         field mask 
         (Source)(local) UE --uplink-->( Destination )(remote) Server 
         ----------------------------------------------------------------*/
            if(filter_type == DS_PDN_TFT_IPV6_ADDR ||
               filter_type == DS_PDN_TFT_IPV6_REM_PRE_ADDR)
#endif /* FEATURE_DATA_REL11 */
            {
              ip_filter->ip_hdr.v6.dst.addr.in6_u.u6_addr8[ip6_inc] =
                                             filter_contents[inc];
              inc++;
            }
#ifdef FEATURE_DATA_REL11
          /*------------------------------------------------------------------
          if the direction is uplink and received filter type is local 
          address then update source parameters and set the source 
          field mask 
          (Source)(local) UE --uplink-->( Destination )(remote) Server 
           ----------------------------------------------------------------*/
            if(filter_type == DS_PDN_TFT_IPV6_LOCAL_PRE_ADDR)
            {
              ip_filter->ip_hdr.v6.src.addr.in6_u.u6_addr8[ip6_inc] =
                                           filter_contents[inc];
              inc++;
            }
#endif /* FEATURE_DATA_REL11 */
          }

          prefix_len_found = FALSE;
#ifdef FEATURE_DATA_REL11
       /*------------------------------------------------------------------------
       check if we have received a prefix length from network 
       incase of remote address type we need to compute it 
       incase of remote/local prefix address type network provides us 
       with the ipv6 prefix (first 16 octets is address and 17th octet is prefix  
       -------------------------------------------------------------------------*/
          if(filter_type == DS_PDN_TFT_IPV6_ADDR)
          {
#endif /* FEATURE_DATA_REL11 */
          for(ip6_inc=0;ip6_inc<16;ip6_inc++)
          {
            if(prefix_len_found == FALSE)
            {
              if(filter_contents[inc] != 0xFF)
              {
                ip_filter->ip_hdr.v6.dst.prefix_len = ip6_inc * 8;
                temp_prefix = (1 << 7);
                if (filter_contents[inc] != 0)
                {
                  while((filter_contents[inc] & temp_prefix) != temp_prefix)
                  {
                    temp_prefix = temp_prefix >> 1;
                    ip_filter->ip_hdr.v6.dst.prefix_len++; 
                  } 
                  inc++; 
                }
                else
                {
                  inc++;
                }
                prefix_len_found = TRUE;
                DS_MSG1(MSG_LEGACY_HIGH, "prefix length %d",
                        ip_filter->ip_hdr.v6.dst.prefix_len );
              }
              else
              {
                inc++;
              }
            }/* prefix_len_found == FALSE*/  
            else
            {
              inc++;
            }
          }/* for */
#ifdef FEATURE_DATA_REL11
          }
          else
          {
            /* if the 17th octet is not null then update the coressponding src/dst contents */
            if(filter_contents[inc] != 0)
            {
              if(filter_type == DS_PDN_TFT_IPV6_REM_PRE_ADDR)
              {
                ip_filter->ip_hdr.v6.dst.prefix_len = filter_contents[inc];
                inc++;
                DS_MSG1(MSG_LEGACY_HIGH, "remote prefix length %d <- dst",
                        ip_filter->ip_hdr.v6.dst.prefix_len );
              }
              if(filter_type == DS_PDN_TFT_IPV6_LOCAL_PRE_ADDR)
              {
                ip_filter->ip_hdr.v6.src.prefix_len = filter_contents[inc];
                inc++;
                DS_MSG1(MSG_LEGACY_HIGH,
                        "local prefix length %d <- src",
                        ip_filter->ip_hdr.v6.src.prefix_len );
              }
            }
            /* otherwise use the default value 128 to update */
            else
            {
              if(filter_type == DS_PDN_TFT_IPV6_REM_PRE_ADDR)
              {
                ip_filter->ip_hdr.v6.dst.prefix_len = 128;
                inc++;
                DS_MSG1(MSG_LEGACY_HIGH,
                        "remote prefix length %d <- dst",
                        ip_filter->ip_hdr.v6.dst.prefix_len );
              }
              if(filter_type == DS_PDN_TFT_IPV6_LOCAL_PRE_ADDR)
              {
                ip_filter->ip_hdr.v6.src.prefix_len = 128;
                inc++;
                DS_MSG1(MSG_LEGACY_HIGH,
                        "local prefix length %d <- src",
                        ip_filter->ip_hdr.v6.src.prefix_len );
              }
            }
          }
#endif /* FEATURE_DATA_REL11 */
          if(prefix_len_found == FALSE && filter_type == DS_PDN_TFT_IPV6_ADDR)
          {
            ip_filter->ip_hdr.v6.dst.prefix_len = 128;
          }
#ifdef FEATURE_DATA_REL11
          /* set the field mask to dst address(uplink) if remote address is received otherwise 
                      set local address mask */
          if(filter_type == DS_PDN_TFT_IPV6_REM_PRE_ADDR ||
             filter_type == DS_PDN_TFT_IPV6_ADDR)
          {
#endif /* FEATURE_DATA_REL11 */
          ip_filter->ip_hdr.v6.field_mask |=(int)IPFLTR_MASK_IP6_DST_ADDR;
#ifdef FEATURE_DATA_REL11
          }
          if(filter_type == DS_PDN_TFT_IPV6_LOCAL_PRE_ADDR)
          {
            ip_filter->ip_hdr.v6.field_mask |=(int)IPFLTR_MASK_IP6_SRC_ADDR;
          }
#endif /* FEATURE_DATA_REL11 */
          DS_MSG0(MSG_LEGACY_HIGH, 
                  "Received IPv6 Address and subnet mask for uplink");
        }/* UPLINK */
        
        if(((ip_filter->ip_hdr.v6.field_mask &(int)IPFLTR_MASK_IP6_SRC_ADDR) ==
            IPFLTR_MASK_IP6_SRC_ADDR) && (direction == DS_PDN_TFT_UPLINK) )
        {
          if(!flow_context_p)
          {
            if(flow_context_p->bearer_ptr != NULL  &&
               flow_context_p->bearer_ptr->pdn_mgr_ptr != NULL)
            {
              pdn_ptr = flow_context_p->bearer_ptr->pdn_mgr_ptr;
              /* getting pdn context and instance; this is because we install 
                              same filter on both v4 and v6 flow contexts */
              inst_p = pdn_ptr->inst_ptr[DS_PDN_INSTANCE_IPV6];
              if(inst_p != NULL )
              {
                memset(&ip_addr,0,sizeof(ps_ip_addr_type));
                ds_fwk_get_ip_addr(ds_fwk_get_inst_ptr_from_index(
                                                        pdn_ptr->fw_index),
                                   IPV6_ADDR,
                                   &ip_addr);
                ipv6_addr_mask.type = IPV6_ADDR;
                /*pass the ipv6 address array */
                ds_pdn_flow_manger_calcuate_prefixmask(
                   ipv6_addr_mask.addr.v6.in6_u.u6_addr8,
                   ip_filter->ip_hdr.v6.src.prefix_len);
                /*utility to print ipv6 addresses */
                IPV6_ADDR_MSG(ipv6_addr_mask.addr.v6.in6_u.u6_addr64);
                IPV6_ADDR_MSG(ip_addr.addr.v6.in6_u.u6_addr64);
                IPV6_ADDR_MSG(ip_filter->ip_hdr.v6.src.addr.in6_u.u6_addr64);
                if (ip_addr.type == IPV6_ADDR)
                {
                  /*checking only prefix 
                    (most significant 64 bits) and mask*/
                  if((ip_addr.addr.v6.in6_u.u6_addr64[0] & 
                      ipv6_addr_mask.addr.v6.in6_u.u6_addr64[0]) != 
                     (ip_filter->ip_hdr.v6.src.addr.in6_u.u6_addr64[0] & 
                      ipv6_addr_mask.addr.v6.in6_u.u6_addr64[0]) )
                  {
                    local_err = DS_LOCAL_ERR_VAL_13;
                    *cause_code_p = 
                                 DS_BEARER_FLOW_INTERNAL_SEMANTIC_ERR_IN_FILTER;
                    goto func_exit;
                  }
                }
              }
            }
          }
        } 
        break;
        
      case DS_PDN_TFT_NEXT_HDR_TYPE:
        if((uint32)filter_mask & (uint32)DS_PDN_TFT_FILTER_NXT_HDR_TYPE)
        {
          local_err = DS_LOCAL_ERR_VAL_14;
          *cause_code_p = DS_BEARER_FLOW_INTERNAL_SEMANTIC_ERR_IN_FILTER;
          goto func_exit;
        }
        else
        {
          filter_mask =(ds_pdn_tft_filter_mask_enum_type)
            ((uint32)filter_mask|(uint32)DS_PDN_TFT_FILTER_NXT_HDR_TYPE);
        }
        if(filter_length < (inc + 1))
        {
          local_err = DS_LOCAL_ERR_VAL_15;
          *cause_code_p = DS_BEARER_FLOW_INTERNAL_SYNTACTICAL_ERR_IN_FILTER;
          goto func_exit;
        }
        else
        {
          protocol_identifier = filter_contents[inc];
          inc++;
          if(ip_filter->ip_vsn == IP_V4)
          {
            ip_filter->ip_hdr.v4.next_hdr_prot = protocol_identifier;
            ip_filter->ip_hdr.v4.field_mask |=
				                (int)IPFLTR_MASK_IP4_NEXT_HDR_PROT;
          }
          if(ip_filter->ip_vsn == IP_V6)
          {
            ip_filter->ip_hdr.v6.next_hdr_prot = protocol_identifier;
            ip_filter->ip_hdr.v6.field_mask |=
				                (int)IPFLTR_MASK_IP6_NEXT_HDR_PROT;
          }
          /* If the port information is sent before the next header*/
          if (((direction == DS_PDN_TFT_DOWNLINK) &&
               (filter_mask & DS_PDN_TFT_FILTER_REMOTE_PORT_TYPE)) ||
               ((direction == DS_PDN_TFT_UPLINK) &&
               (filter_mask & DS_PDN_TFT_FILTER_LOCAL_PORT_TYPE)))
          {
            DS_MSG0(MSG_LEGACY_HIGH, 
                    "Previously received SRC port information");
            if(protocol_identifier == (byte)PS_IPPROTO_TCP)
            {
              ip_filter->next_prot_hdr.tcp.field_mask |=
                                           (int)IPFLTR_MASK_TCP_SRC_PORT;
            }
            if(protocol_identifier == (byte)PS_IPPROTO_UDP)
            {
              ip_filter->next_prot_hdr.tcp.field_mask |=
                                          (int)IPFLTR_MASK_UDP_SRC_PORT;
            }
          }
          
          if (((direction == DS_PDN_TFT_DOWNLINK) &&
               (filter_mask & DS_PDN_TFT_FILTER_LOCAL_PORT_TYPE)) ||
               ((direction == DS_PDN_TFT_UPLINK) &&
               (filter_mask & DS_PDN_TFT_FILTER_REMOTE_PORT_TYPE)))
          {
            DS_MSG0(MSG_LEGACY_HIGH,
                    "Previously received DST port information");
            if(protocol_identifier == (byte)PS_IPPROTO_TCP)
            {
              ip_filter->next_prot_hdr.tcp.field_mask |=
                                           (int)IPFLTR_MASK_TCP_DST_PORT;
            }
            if(protocol_identifier == (byte)PS_IPPROTO_UDP)
            {
              ip_filter->next_prot_hdr.tcp.field_mask |=
                                          (int)IPFLTR_MASK_UDP_DST_PORT;
            }
          }
          DS_MSG1(MSG_LEGACY_HIGH, 
                  "Received %d as the next header",protocol_identifier);  
        }/* else valid data */
        break;
        
      case DS_PDN_TFT_SINGLE_LOCAL_PORT:
        if((uint32)filter_mask & (uint32)DS_PDN_TFT_FILTER_LOCAL_PORT_TYPE)
        {
          local_err = DS_LOCAL_ERR_VAL_16;
          *cause_code_p = DS_BEARER_FLOW_INTERNAL_SEMANTIC_ERR_IN_FILTER;
          goto func_exit;
        }
        else
        {
          filter_mask =(ds_pdn_tft_filter_mask_enum_type)
               ((uint32)filter_mask|(uint32)DS_PDN_TFT_FILTER_LOCAL_PORT_TYPE);
        }
        if(filter_length < (inc + 2))
        {
          local_err = DS_LOCAL_ERR_VAL_17;
          *cause_code_p = DS_BEARER_FLOW_INTERNAL_SYNTACTICAL_ERR_IN_FILTER;
          goto func_exit;
        }
        else
        {
          port_number =(uint16)(filter_contents[inc++] << 8);
          port_number |=(uint16)(filter_contents[inc++]);
          if(direction == DS_PDN_TFT_DOWNLINK)
          {
            if(protocol_identifier ==(byte)PS_IPPROTO_TCP)
            {
              ip_filter->next_prot_hdr.tcp.dst.port = ps_ntohs(port_number);
              ip_filter->next_prot_hdr.tcp.dst.range = 0;
              ip_filter->next_prot_hdr.tcp.field_mask |=
                                               (int)IPFLTR_MASK_TCP_DST_PORT;              
            }
            else if(protocol_identifier ==(byte)PS_IPPROTO_UDP)
            {
              ip_filter->next_prot_hdr.udp.dst.port = ps_ntohs(port_number);
              ip_filter->next_prot_hdr.udp.dst.range = 0;
              ip_filter->next_prot_hdr.udp.field_mask |=
                                             (int)IPFLTR_MASK_UDP_DST_PORT;
            }
            else
            {
              /*use either TCP or UDP as both are of same format and size*/
              ip_filter->next_prot_hdr.udp.dst.port = ps_ntohs(port_number);
              ip_filter->next_prot_hdr.udp.dst.range =0;
            }            
            DS_MSG2(MSG_LEGACY_HIGH,
                    "Received local port number 0x%x, protocol is 0x%x",
                    port_number,protocol_identifier); 
          }/* direction == DOWNLINK */
          if(direction == DS_PDN_TFT_UPLINK)
          {
            if(protocol_identifier ==(byte)PS_IPPROTO_TCP)
            {
              ip_filter->next_prot_hdr.tcp.src.port = ps_ntohs(port_number);
              ip_filter->next_prot_hdr.tcp.src.range =0;
              ip_filter->next_prot_hdr.tcp.field_mask |=
                                             (int)IPFLTR_MASK_TCP_SRC_PORT;
            }
            else if(protocol_identifier ==(byte)PS_IPPROTO_UDP)
            {
              ip_filter->next_prot_hdr.udp.src.port = ps_ntohs(port_number);
              ip_filter->next_prot_hdr.udp.src.range = 0;
              ip_filter->next_prot_hdr.udp.field_mask |=
                                           (int)IPFLTR_MASK_UDP_SRC_PORT;
            }
            else
            {
              /*use either TCP or UDP as both are of same format and size*/
              ip_filter->next_prot_hdr.udp.src.port = ps_ntohs(port_number);
              ip_filter->next_prot_hdr.udp.src.range =0;
            }            
            DS_MSG2(MSG_LEGACY_HIGH, 
                    "Received local port number 0x%x, prot is 0x%x",
                    port_number, protocol_identifier); 
          } /* direction == UPLINK */
        } /* else filter_length */
        break;
        
      case DS_PDN_TFT_LOCAL_PORT_RANGE:
        if((uint32)filter_mask & (uint32)DS_PDN_TFT_FILTER_LOCAL_PORT_TYPE)
        {
          local_err = DS_LOCAL_ERR_VAL_18;
          *cause_code_p = DS_BEARER_FLOW_INTERNAL_SEMANTIC_ERR_IN_FILTER;
          goto func_exit;
        }
        else
        {
          filter_mask =(ds_pdn_tft_filter_mask_enum_type)
             ((uint32)filter_mask|(uint32)DS_PDN_TFT_FILTER_LOCAL_PORT_TYPE);
        }
        if(filter_length < (inc + 4))
        {
          local_err = DS_LOCAL_ERR_VAL_19;
          *cause_code_p = DS_BEARER_FLOW_INTERNAL_SYNTACTICAL_ERR_IN_FILTER;
          goto func_exit;
        }
        else
        {
          port_number =(uint16)(filter_contents[inc++] << 8);
          port_number |=(uint16)(filter_contents[inc++]);

          port_range =(uint16)(filter_contents[inc++] << 8);
          port_range |=(uint16)(filter_contents[inc++]);

          if(direction == DS_PDN_TFT_DOWNLINK)
          {
            if(protocol_identifier ==(byte)PS_IPPROTO_TCP)
            { 
              if(!(ARE_PORT_PARAM_VALID(port_number,port_range)))
              {
                local_err = DS_LOCAL_ERR_VAL_20;
                *cause_code_p = DS_BEARER_FLOW_INTERNAL_SEMANTIC_ERR_IN_FILTER;
                goto func_exit;
              }              
              port_range = port_range - port_number;              
              ip_filter->next_prot_hdr.tcp.dst.port = ps_ntohs(port_number);
              ip_filter->next_prot_hdr.tcp.dst.range = (port_range);
              ip_filter->next_prot_hdr.tcp.field_mask |=
                                            (int)IPFLTR_MASK_TCP_DST_PORT;             
            } /* PROTO_TCP */
            else if(protocol_identifier ==(byte)PS_IPPROTO_UDP)
            {
              if(!(ARE_PORT_PARAM_VALID(port_number,port_range)))
              {
                local_err = DS_LOCAL_ERR_VAL_21;
                *cause_code_p = DS_BEARER_FLOW_INTERNAL_SEMANTIC_ERR_IN_FILTER;
                goto func_exit;
              }
              port_range = port_range - port_number; 
              ip_filter->next_prot_hdr.udp.dst.port = ps_ntohs(port_number);
              ip_filter->next_prot_hdr.udp.dst.range = (port_range);             
              ip_filter->next_prot_hdr.udp.field_mask |=
                                           (int)IPFLTR_MASK_UDP_DST_PORT;
            }/* PROTO_UDP */
            else
            {
              /*use either TCP or UDP as both are of same format and size*/
              if(!(ARE_PORT_PARAM_VALID(port_number,port_range)))
              {
                local_err = DS_LOCAL_ERR_VAL_22;
                *cause_code_p = DS_BEARER_FLOW_INTERNAL_SEMANTIC_ERR_IN_FILTER;
                goto func_exit;
              }
              port_range = port_range - port_number;
              ip_filter->next_prot_hdr.udp.dst.port = ps_ntohs(port_number);
              ip_filter->next_prot_hdr.udp.dst.range = (port_range);  
            }
            
            DS_MSG3(MSG_LEGACY_HIGH,
                    "Rec local port no 0x%x,range 0x%x for proto %x in downlink",
                    port_number, port_range, protocol_identifier); 
          }/* direction == DOWNLINK */
          if(direction == DS_PDN_TFT_UPLINK)
          {
            if(protocol_identifier ==(byte)PS_IPPROTO_TCP)
            {  
              if(!(ARE_PORT_PARAM_VALID(port_number,port_range)))
              {
                local_err = DS_LOCAL_ERR_VAL_23;
                *cause_code_p = DS_BEARER_FLOW_INTERNAL_SEMANTIC_ERR_IN_FILTER;
                goto func_exit;
              }
              port_range = port_range - port_number;
              ip_filter->next_prot_hdr.tcp.src.port = ps_ntohs(port_number);
              ip_filter->next_prot_hdr.tcp.src.range = (port_range);
              ip_filter->next_prot_hdr.tcp.field_mask |=
                                           (int)IPFLTR_MASK_TCP_SRC_PORT;            
            }/* PROTO_TCP */
            else if(protocol_identifier ==(byte)PS_IPPROTO_UDP)
            { 
              if(!(ARE_PORT_PARAM_VALID(port_number,port_range)))
              {
                local_err = DS_LOCAL_ERR_VAL_24;
                *cause_code_p = DS_BEARER_FLOW_INTERNAL_SEMANTIC_ERR_IN_FILTER;
                goto func_exit;
              }
              port_range = port_range - port_number;
              ip_filter->next_prot_hdr.udp.src.port = ps_ntohs(port_number);
              ip_filter->next_prot_hdr.udp.src.range = (port_range);
              ip_filter->next_prot_hdr.udp.field_mask |=
                                           (int)IPFLTR_MASK_UDP_SRC_PORT;             
            } /* PROTO_UDP */
            else
            {
              /*use either TCP or UDP as both are of same format and size*/
              if(!(ARE_PORT_PARAM_VALID(port_number,port_range)))
              {
                local_err = DS_LOCAL_ERR_VAL_25;
                *cause_code_p = DS_BEARER_FLOW_INTERNAL_SEMANTIC_ERR_IN_FILTER;
                goto func_exit;
              }
              port_range = port_range - port_number;
              ip_filter->next_prot_hdr.udp.src.port = ps_ntohs(port_number);
              ip_filter->next_prot_hdr.udp.src.range = (port_range);                            
            }/* else */

            DS_MSG3(MSG_LEGACY_HIGH,
                    "Rec local port no 0x%x,range 0x%x for proto %x in uplink",
                    port_number, port_range, protocol_identifier); 
          } /* direction == UPLINK */
        }/* else filter_length*/ 
        break;
        
      case DS_PDN_TFT_SINGLE_REMOTE_PORT:
        if((uint32)filter_mask & (uint32)DS_PDN_TFT_FILTER_REMOTE_PORT_TYPE)
        {
          local_err = DS_LOCAL_ERR_VAL_26;
          *cause_code_p = DS_BEARER_FLOW_INTERNAL_SEMANTIC_ERR_IN_FILTER;
          goto func_exit;
        }
        else
        {
          filter_mask =(ds_pdn_tft_filter_mask_enum_type)
             ((uint32)filter_mask|(uint32)DS_PDN_TFT_FILTER_REMOTE_PORT_TYPE);
        }
        if(filter_length < (inc + 2))
        {
          local_err = DS_LOCAL_ERR_VAL_27;
          *cause_code_p = DS_BEARER_FLOW_INTERNAL_SYNTACTICAL_ERR_IN_FILTER;
          goto func_exit;
        }
        else
        {
          port_number =(uint16)(filter_contents[inc++] << 8);
          port_number |=(uint16)(filter_contents[inc++]);
          if(direction == DS_PDN_TFT_DOWNLINK)
          {
            if(protocol_identifier ==(byte)PS_IPPROTO_TCP)
            {
              ip_filter->next_prot_hdr.tcp.src.port = ps_ntohs(port_number);
              ip_filter->next_prot_hdr.tcp.src.range = 0;
              ip_filter->next_prot_hdr.tcp.field_mask |=
                                             (int)IPFLTR_MASK_TCP_SRC_PORT;
            }
            else if(protocol_identifier ==(byte)PS_IPPROTO_UDP)
            {
              ip_filter->next_prot_hdr.udp.src.port = ps_ntohs(port_number);
              ip_filter->next_prot_hdr.udp.src.range = 0;
              ip_filter->next_prot_hdr.udp.field_mask |=
                                              (int)IPFLTR_MASK_UDP_SRC_PORT;
            }
            else
            {
              /*use either TCP or UDP as both are of same format and size*/
              ip_filter->next_prot_hdr.udp.src.port = ps_ntohs(port_number);
              ip_filter->next_prot_hdr.udp.src.range = 0;
            }            
            DS_MSG2(MSG_LEGACY_HIGH,
                   "Received remote port no:%d, but protocol is:%d in downlink",
                    port_number,protocol_identifier); 
          }/* direction == DOWNLINK */
          if(direction == DS_PDN_TFT_UPLINK)
          {
            if(protocol_identifier ==(byte)PS_IPPROTO_TCP)
            {
              ip_filter->next_prot_hdr.tcp.dst.port = ps_ntohs(port_number);
              ip_filter->next_prot_hdr.tcp.dst.range =0;
              ip_filter->next_prot_hdr.tcp.field_mask |=
                                           (int)IPFLTR_MASK_TCP_DST_PORT;
            }
            else if(protocol_identifier ==(byte)PS_IPPROTO_UDP)
            {
              ip_filter->next_prot_hdr.udp.dst.port = ps_ntohs(port_number);
              ip_filter->next_prot_hdr.udp.dst.range = 0;
              ip_filter->next_prot_hdr.udp.field_mask |=
                                             (int)IPFLTR_MASK_UDP_DST_PORT;
            }
            else
            {
              /*use either TCP or UDP as both are of same format and size*/
              ip_filter->next_prot_hdr.udp.dst.port = ps_ntohs(port_number);
              ip_filter->next_prot_hdr.udp.dst.range = 0;
            }            
            DS_MSG2(MSG_LEGACY_HIGH,
                    "Received remote port no:%d, but protocol is:%d in uplink",
                    port_number,protocol_identifier); 
          } /* direction == UPLINK */
        } /* else filter_length*/
        break;
        
      case DS_PDN_TFT_REMOTE_PORT_RANGE:
        if((uint32)filter_mask & (uint32)DS_PDN_TFT_FILTER_REMOTE_PORT_TYPE)
        {
          local_err = DS_LOCAL_ERR_VAL_28;
          *cause_code_p = DS_BEARER_FLOW_INTERNAL_SEMANTIC_ERR_IN_FILTER;
          goto func_exit;
        }
        else
        {
          filter_mask =(ds_pdn_tft_filter_mask_enum_type)
            ((uint32)filter_mask |(uint32)DS_PDN_TFT_FILTER_REMOTE_PORT_TYPE);
        }
        if(filter_length < (inc + 4))
        {
          local_err = DS_LOCAL_ERR_VAL_29;
          *cause_code_p = DS_BEARER_FLOW_INTERNAL_SYNTACTICAL_ERR_IN_FILTER;
          goto func_exit;
        }
        else
        {
          port_number =(uint16)(filter_contents[inc++] << 8);
          port_number |=(uint16)(filter_contents[inc++]);

          port_range =(uint16)(filter_contents[inc++] << 8);
          port_range |=(uint16)(filter_contents[inc++]);
		 

          if(direction == DS_PDN_TFT_DOWNLINK)
          {
            if(protocol_identifier ==(byte)PS_IPPROTO_TCP)
            {
              if(!(ARE_PORT_PARAM_VALID(port_number,port_range)))
              {
                local_err = DS_LOCAL_ERR_VAL_30;
                *cause_code_p = DS_BEARER_FLOW_INTERNAL_SEMANTIC_ERR_IN_FILTER;
                goto func_exit;
              }
              
              port_range = port_range - port_number;
              ip_filter->next_prot_hdr.tcp.src.port = ps_ntohs(port_number);
              ip_filter->next_prot_hdr.tcp.src.range = (port_range);              
              ip_filter->next_prot_hdr.tcp.field_mask |=
                                              (int)IPFLTR_MASK_TCP_SRC_PORT;          
            }
            else if(protocol_identifier ==(byte)PS_IPPROTO_UDP)
            {
              if(!(ARE_PORT_PARAM_VALID(port_number,port_range)))
              {
                local_err = DS_LOCAL_ERR_VAL_31;
                *cause_code_p = DS_BEARER_FLOW_INTERNAL_SEMANTIC_ERR_IN_FILTER;
                goto func_exit;
              }  

              port_range = port_range - port_number;           
              ip_filter->next_prot_hdr.udp.src.port = ps_ntohs(port_number);
              ip_filter->next_prot_hdr.udp.src.range = (port_range);         
              ip_filter->next_prot_hdr.udp.field_mask |=
                                               (int)IPFLTR_MASK_UDP_SRC_PORT;
            }
            else
            {
              /*use either TCP or UDP as both are of same format and size*/
              if(!(ARE_PORT_PARAM_VALID(port_number,port_range)))
              {
                local_err = DS_LOCAL_ERR_VAL_32;
                *cause_code_p = DS_BEARER_FLOW_INTERNAL_SEMANTIC_ERR_IN_FILTER;
                goto func_exit;
              }
              port_range = port_range - port_number;
              ip_filter->next_prot_hdr.udp.src.port = ps_ntohs(port_number);
              ip_filter->next_prot_hdr.udp.src.range = (port_range);
            }
            
            DS_MSG3(MSG_LEGACY_HIGH,
                    "Rec remote port no:%d, range:%d for proto %d in downlink",
                    port_number, port_range, protocol_identifier); 
          }/* direction == DOWNLINK */
          if(direction == DS_PDN_TFT_UPLINK)
          {
            if(protocol_identifier ==(byte)PS_IPPROTO_TCP)
            {
              if(!(ARE_PORT_PARAM_VALID(port_number,port_range)))
              {
                local_err = DS_LOCAL_ERR_VAL_33;
                *cause_code_p = DS_BEARER_FLOW_INTERNAL_SEMANTIC_ERR_IN_FILTER;
                goto func_exit;
              }
              port_range = port_range - port_number;
              ip_filter->next_prot_hdr.tcp.dst.port = ps_ntohs(port_number);
              ip_filter->next_prot_hdr.tcp.dst.range =(port_range);
              ip_filter->next_prot_hdr.tcp.field_mask |=
                                        (int)IPFLTR_MASK_TCP_DST_PORT;
            }
            else if(protocol_identifier ==(byte)PS_IPPROTO_UDP)
            {
              if(!(ARE_PORT_PARAM_VALID(port_number,port_range)))
              {
                local_err = DS_LOCAL_ERR_VAL_34;
                *cause_code_p = DS_BEARER_FLOW_INTERNAL_SEMANTIC_ERR_IN_FILTER;
                goto func_exit;
              }
              port_range = port_range - port_number;
              ip_filter->next_prot_hdr.udp.dst.port = ps_ntohs(port_number);
              ip_filter->next_prot_hdr.udp.dst.range = (port_range);
              ip_filter->next_prot_hdr.udp.field_mask |=
                                       (int)IPFLTR_MASK_UDP_DST_PORT;
            }
            else
            {
              /*use either TCP or UDP as both are of same format and size*/
              if(!(ARE_PORT_PARAM_VALID(port_number,port_range)))
              {
                local_err = DS_LOCAL_ERR_VAL_35;
                *cause_code_p = DS_BEARER_FLOW_INTERNAL_SEMANTIC_ERR_IN_FILTER;
                goto func_exit;
              }

              port_range = port_range - port_number;
              ip_filter->next_prot_hdr.udp.dst.port = ps_ntohs(port_number);
              ip_filter->next_prot_hdr.udp.dst.range = (port_range);
            }
            
            DS_MSG3(MSG_LEGACY_HIGH,
                    "Rec remote port no:%d, range:%d for proto %d in uplink",
                    port_number, port_range, protocol_identifier); 
          } /* direction == UPLINK */
        } /* else filter length*/
        break;
        
      case DS_PDN_TFT_TYPE_OF_SRVC:
      //case DS_3GPP_TFT_IP6_TRAFFIC_CLASS:
        if((uint32)filter_mask & (uint32)DS_PDN_TFT_FILTER_TOS_TYPE)
        {
          local_err = DS_LOCAL_ERR_VAL_36;
          *cause_code_p = DS_BEARER_FLOW_INTERNAL_SEMANTIC_ERR_IN_FILTER;
          goto func_exit;
        }
        else
        {
          filter_mask =(ds_pdn_tft_filter_mask_enum_type)
              ((uint32)filter_mask|(uint32)DS_PDN_TFT_FILTER_TOS_TYPE);
        }
        if(filter_length < (inc + 2))
        {
          local_err = DS_LOCAL_ERR_VAL_37;
          *cause_code_p = DS_BEARER_FLOW_INTERNAL_SYNTACTICAL_ERR_IN_FILTER;
          goto func_exit;
        }
        else
        {
          if(ip_filter->ip_vsn == IP_V4)
          {
            ip_filter->ip_hdr.v4.field_mask|=(uint32)IPFLTR_MASK_IP4_TOS;
            ip_filter->ip_hdr.v4.tos.val =  filter_contents[inc];
            inc++;
            ip_filter->ip_hdr.v4.tos.mask = filter_contents[inc];
            inc++;
            DS_MSG2(MSG_LEGACY_HIGH,"Received TOS-value 0x%x and TOS-mask 0x%x",
                 	  ip_filter->ip_hdr.v4.tos.val,ip_filter->ip_hdr.v4.tos.mask);
          }
          else if(ip_filter->ip_vsn == IP_V6)
          {
            ip_filter->ip_hdr.v6.field_mask|=
                                   (uint32)IPFLTR_MASK_IP6_TRAFFIC_CLASS;
            ip_filter->ip_hdr.v6.trf_cls.val =  filter_contents[inc];
            inc++;
            ip_filter->ip_hdr.v6.trf_cls.mask = filter_contents[inc];
            inc++;
            DS_MSG2(MSG_LEGACY_HIGH,
                    "Received TRF_CLASS-value=%d and TRF_CLASS-mask = %d",
         	          ip_filter->ip_hdr.v6.trf_cls.val,
                    ip_filter->ip_hdr.v6.trf_cls.mask);
          }
        }/* else filter_length */
        break;
        
      case DS_PDN_TFT_SEC_PARAM_IDX: 
        if((uint32)filter_mask & (uint32)DS_PDN_TFT_FILTER_SEC_TYPE)
        {
          local_err = DS_LOCAL_ERR_VAL_38;
          *cause_code_p = DS_BEARER_FLOW_INTERNAL_SEMANTIC_ERR_IN_FILTER;
          goto func_exit;
        }
        else
        {
          filter_mask =(ds_pdn_tft_filter_mask_enum_type)
               ((uint32)filter_mask|(uint32)DS_PDN_TFT_FILTER_SEC_TYPE);
        }
        if(filter_length < (inc + 4))
        {
          local_err = DS_LOCAL_ERR_VAL_39;
          *cause_code_p = DS_BEARER_FLOW_INTERNAL_SYNTACTICAL_ERR_IN_FILTER;
          goto func_exit;
        }
        else
        {
          ip_filter->next_prot_hdr.esp.field_mask |=
                                             (uint32)IPFLTR_MASK_ESP_SPI;
          esp_spi =(uint32)(filter_contents[inc++] << 24);
          esp_spi |=(uint32)(filter_contents[inc++] <<16);
          esp_spi |=(uint32)(filter_contents[inc++] <<8);
          esp_spi |=(uint32)(filter_contents[inc++]);
          ip_filter->next_prot_hdr.esp.spi = ps_ntohl(esp_spi);
		  
          DS_MSG1(MSG_LEGACY_HIGH, "Received SEC Param Index-value 0x%x",
               	  ip_filter->next_prot_hdr.esp.spi);
        }
        break;
        
      case DS_PDN_TFT_FLOW_LABEL:
        if((uint32)filter_mask & (uint32)DS_PDN_TFT_FILTER_FLOW_LABEL_TYPE)
        {
          local_err = DS_LOCAL_ERR_VAL_40;
          *cause_code_p = DS_BEARER_FLOW_INTERNAL_SEMANTIC_ERR_IN_FILTER;
          goto func_exit;
        }
        else
        {
          filter_mask =(ds_pdn_tft_filter_mask_enum_type)
              ((uint32)filter_mask|(uint32)DS_PDN_TFT_FILTER_FLOW_LABEL_TYPE);
        }
        if(filter_length < (inc + 3))
        {
          local_err = DS_LOCAL_ERR_VAL_41;
          *cause_code_p = DS_BEARER_FLOW_INTERNAL_SYNTACTICAL_ERR_IN_FILTER;
          goto func_exit;
        }
        else
        {
          ip_filter->ip_hdr.v6.field_mask|=
                                (uint32)IPFLTR_MASK_IP6_FLOW_LABEL;
          flow_label = (filter_contents[inc])&(0xF);
          flow_label = flow_label<< 8;
          inc++;
          flow_label |= filter_contents[inc];
          flow_label = flow_label << 8;
          inc++;
          flow_label |= filter_contents[inc];        
          inc++;
          ip_filter->ip_hdr.v6.flow_label = flow_label;
          DS_MSG1(MSG_LEGACY_HIGH, "Received V6 FLOW LABEL 0x%x",flow_label);
        }
      	 break;
        
      default:
        local_err = DS_LOCAL_ERR_VAL_42;
        *cause_code_p = DS_BEARER_FLOW_INTERNAL_SYNTACTICAL_ERR_IN_FILTER;
        goto func_exit;
    }/* switch filter_type */
  }/* while */

  if(( ((uint32)filter_mask & (uint32)DS_PDN_TFT_FILTER_REMOTE_PORT_TYPE)||
       ((uint32)filter_mask & (uint32)DS_PDN_TFT_FILTER_LOCAL_PORT_TYPE) ) &&
       (!((uint32)filter_mask & (uint32)DS_PDN_TFT_FILTER_NXT_HDR_TYPE)) )
  {
    DS_MSG0(MSG_LEGACY_HIGH, "Recieved filter with port number only");
    protocol_identifier = (byte)PS_IPPROTO_TCP_UDP;
    if(ip_filter->ip_vsn == IP_V4)
    {
      ip_filter->ip_hdr.v4.next_hdr_prot = protocol_identifier;
      ip_filter->ip_hdr.v4.field_mask |=(int)IPFLTR_MASK_IP4_NEXT_HDR_PROT;
    }
    if(ip_filter->ip_vsn == IP_V6)
    {
      ip_filter->ip_hdr.v6.next_hdr_prot = protocol_identifier;
      ip_filter->ip_hdr.v6.field_mask |=(int)IPFLTR_MASK_IP6_NEXT_HDR_PROT;
    }
    /* If the port information is sent before the next header*/
    if (((direction == DS_PDN_TFT_DOWNLINK) &&
         (filter_mask & DS_PDN_TFT_FILTER_REMOTE_PORT_TYPE)) ||
         ((direction == DS_PDN_TFT_UPLINK) &&
         (filter_mask & DS_PDN_TFT_FILTER_LOCAL_PORT_TYPE)))
    {
      DS_MSG0(MSG_LEGACY_HIGH, "Previously received SRC port information");
      ip_filter->next_prot_hdr.tcp.field_mask |=
                                   (int)IPFLTR_MASK_TCP_UDP_SRC_PORT;
    }
    if (((direction == DS_PDN_TFT_DOWNLINK) &&
         (filter_mask & DS_PDN_TFT_FILTER_LOCAL_PORT_TYPE)) ||
         ((direction == DS_PDN_TFT_UPLINK) &&
         (filter_mask & DS_PDN_TFT_FILTER_REMOTE_PORT_TYPE)))
    {
      DS_MSG0(MSG_LEGACY_HIGH, "Previously received DST port information");
      ip_filter->next_prot_hdr.tcp.field_mask |=
                                     (int)IPFLTR_MASK_TCP_UDP_DST_PORT;
    } 
  }/* if port only */

func_exit:
  if(local_err != DS_LOCAL_ERR_VAL_NONE)
  {
    DS_ERROR_LOG_1("Convert filter failed with error =d", local_err);    
    return FALSE;
  }
  
  return TRUE;
} /* ds_pdn_flow_manager_convert_nw_filter_to_app_filter */

/*===========================================================================
FUNCTION DS_PDN_FLOW_MANAGER_CONVERT_NW_TFT_TO_APP_TFT

DESCRIPTION
  Converts the NW passed TFT spec to App filter spec.

PARAMETERS
 nw_tft            -  network TFT to be converted
 app_filter        -  App flow to store the converted info.
 ip_type           -  IP type of the PDN.
 flow_context_p    -  Pointer to the flow context

DEPENDENCIES
  None.

RETURN VALUE
boolean          - returns if conversion is successfull
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_flow_manager_convert_nw_tft_to_app_tft
(
  tft_type_T                                   *nw_tft,
  qos_spec_type                                *app_filter,
  ds_pdn_mgr_instance_type_e                    inst_type,
  ds_flow_type                                 *flow_context_p,
  ds_bearer_flow_internal_cause_code_enum_type *icause_code
)
{
  packet_filter_type_T                      *nw_filter = NULL;
  int                                        index = 0;
  byte                                       filter_id =0;
  byte                                       direction =0;
  ds_pdn_tft_pkt_filter_direction_enum_type  filter_direction;
  ds_local_err_val_e_type                    local_err = DS_LOCAL_ERR_VAL_NONE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_MSG1(MSG_LEGACY_HIGH, "Convert NW TFT to App TFT for inst_type: %d",
          inst_type);

  if(nw_tft ==NULL || app_filter == NULL)
  {
    local_err = DS_LOCAL_ERR_VAL_0;
    *icause_code = DS_BEARER_FLOW_INTERNAL_INVALID_MANDATORY_INFO;
    goto func_exit;
  }

  if(nw_tft->num_filters == 0)
  {
    local_err = DS_LOCAL_ERR_VAL_1;
    *icause_code = DS_BEARER_FLOW_INTERNAL_SYNTACTICAL_ERR_IN_TFT;
    goto func_exit;
  }

  for(index=0; index < nw_tft->num_filters; index++)
  {
    /*----------------------------------------------------------------------
     Process Each packet filter
    -----------------------------------------------------------------------*/    
    nw_filter = &(nw_tft->filter_list[index]);
    if(nw_filter->content_length == 0)
    {
      local_err = DS_LOCAL_ERR_VAL_2;
      *icause_code = DS_BEARER_FLOW_INTERNAL_SYNTACTICAL_ERR_IN_TFT;
    }
    
    filter_id = nw_filter->identifier & 0xF;
    direction = ((nw_filter->identifier & 0x30)>>4);
    DS_MSG2(MSG_LEGACY_HIGH, 
            "Processing filter with ID:%d and direction 0x%x",
            filter_id, direction);

    filter_direction =(ds_pdn_tft_pkt_filter_direction_enum_type)direction; 


    if(filter_direction == DS_PDN_TFT_DOWNLINK ||
       filter_direction == DS_PDN_TFT_BIDIRECTIONAL ||
       filter_direction == DS_PDN_TFT_PRE_REL_7)
    {
      if(ds_pdn_flow_manager_convert_nw_filter_to_app_filter(
                nw_filter,
                &(app_filter->rx.fltr_template.list_ptr[
                          app_filter->rx.fltr_template.num_filters]),
                filter_id, 
                DS_PDN_TFT_DOWNLINK,
                inst_type, 
                flow_context_p,
                icause_code) == FALSE)
      {
        local_err = DS_LOCAL_ERR_VAL_3;
        goto func_exit;
      }
      
      app_filter->rx.fltr_template.num_filters++;
      app_filter->field_mask |= (qos_spec_field_mask_type)QOS_MASK_RX_FLOW;
    }
    
    if(filter_direction == DS_PDN_TFT_UPLINK ||
       filter_direction == DS_PDN_TFT_BIDIRECTIONAL ||
       filter_direction == DS_PDN_TFT_PRE_REL_7)
    {
      if(ds_pdn_flow_manager_convert_nw_filter_to_app_filter(
                nw_filter,
                &(app_filter->tx.fltr_template.list_ptr[
                          app_filter->tx.fltr_template.num_filters]),
                filter_id, 
                DS_PDN_TFT_UPLINK,
                inst_type, 
                flow_context_p,
                icause_code) == FALSE)
      {
        local_err = DS_LOCAL_ERR_VAL_4;
        goto func_exit;
      }
      
      app_filter->tx.fltr_template.num_filters++;
      app_filter->field_mask |= (qos_spec_field_mask_type)QOS_MASK_TX_FLOW; 
    }
  } /* for filters */

func_exit:
  if(local_err != DS_LOCAL_ERR_VAL_NONE)
  {
    DS_ERROR_LOG_1("Convert TFT failed with error =d", local_err);    
    return FALSE;
  }
  
  return TRUE;
} /* ds_flow_manager_convert_nw_tft_to_app_tft */

/*===========================================================================
FUNCTION DS_PDN_FLOW_MANAGER_UPDATE_PRECEDENCE_BIT_MASK

DESCRIPTION
  Update precednce bit mask for the given flow context.

PARAMETERS
 pdn_context_s       - Pointer to PDN Context.
 flow_context_p      - Pointer to flow context.
 req_modify          - boolean. If true, request from modify filter set.
 set_precedence      - boolean. If true, set the bit else clear the bit.
 req_precedence      - boolean. If true, update requested precedence else
                       update precedence bit mask.  

DEPENDENCIES
  None.

RETURN VALUE
 boolean             - Indicates true or false.
  
SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_pdn_flow_manager_update_precedence_bit_mask
( 
  ds_pdn_mgr_s      *pdn_mgr_p,
  ds_flow_type      *flow_context_p,
  boolean            req_modify,
  boolean            set_precedence,
  boolean            req_precedence     
)
{
  ds_pdn_flow_manager_s   *flow_manager_p = NULL;
  qos_spec_type           *orig_spec = NULL;
  ip_filter_type          *fltr = NULL; 
  int                      cnt = 0;          
  uint16                   preced = 0;
  uint16                   identifier = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  if(!ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p) ||
     !ds_flow_is_valid(flow_context_p))
  {
    return FALSE;
  }

  flow_manager_p = ds_pdn_flow_mgr_get_entry(pdn_mgr_p->index);
  if(!flow_manager_p)
  {
    return FALSE;
  }

  if(req_modify)
  {
    orig_spec = flow_context_p->qos_modify_info_ptr;
  }
  else
  {
    orig_spec = flow_context_p->qos_info_ptr;
  }
 
  /*------------------------------------------------------------------------
     Loop thru all filters and update the precedence bit mask for RX
  -------------------------------------------------------------------------*/
  for(cnt =0; cnt<orig_spec->rx.fltr_template.num_filters; cnt++)
  {
    fltr = &(orig_spec->rx.fltr_template.list_ptr[cnt]);
    preced = fltr->ipfltr_aux_info.fi_precedence;
    identifier = (fltr->ipfltr_aux_info.fi_id & 0xf);

    if(ds_pdn_tft_is_filter_identifier_requested(
                         identifier, &(flow_context_p->ignore_fi_id)) == TRUE)
    {
      continue;
    }
    
    if(set_precedence)
    {
      if(req_precedence)
      {
        /*-------------------------------------------------------------------
           Set the Requested FI Preced for Rx
        -------------------------------------------------------------------*/
        ds_pdn_tft_set_filter_preced_bit(preced, flow_manager_p->req_fi_preced);  
      }
      else
      {
        /*-------------------------------------------------------------------
           Set the FI Preced for Rx
        -------------------------------------------------------------------*/
        ds_pdn_tft_set_filter_preced_bit(preced, flow_manager_p->fi_preced);  
      }
    }
    else
    {
      if(req_precedence)
      {
        /*-------------------------------------------------------------------
           Clear the Requested FI Preced for Rx
        -------------------------------------------------------------------*/
        ds_pdn_tft_clear_filter_preced_bit(preced,
                                           flow_manager_p->req_fi_preced);  
      }
      else
      {
        /*-------------------------------------------------------------------
           Clear the FI Preced for Rx
        -------------------------------------------------------------------*/
        ds_pdn_tft_clear_filter_preced_bit(preced, flow_manager_p->fi_preced);  
      }
    }
  }/* RX FOR Loop*/
  
  /*------------------------------------------------------------------------
     Loop thru all filters and update the precedence bit mask for TX
  -------------------------------------------------------------------------*/
  for(cnt =0; cnt<orig_spec->tx.fltr_template.num_filters; cnt++)
  {
    fltr = &(orig_spec->tx.fltr_template.list_ptr[cnt]);
    preced = fltr->ipfltr_aux_info.fi_precedence;
    identifier = (fltr->ipfltr_aux_info.fi_id & 0xf);

    if(ds_pdn_tft_is_filter_identifier_requested(
                         identifier, &(flow_context_p->ignore_fi_id))== TRUE)
    {
      continue;
    }
    
    if(set_precedence)
    {
      if(req_precedence)
      {
        /*-------------------------------------------------------------------
           Set the Requested FI Preced for Tx
        -------------------------------------------------------------------*/
        ds_pdn_tft_set_filter_preced_bit(preced, flow_manager_p->req_fi_preced);  
      }
      else
      {
        /*-------------------------------------------------------------------
           Set the FI Preced for Tx
        -------------------------------------------------------------------*/
        ds_pdn_tft_set_filter_preced_bit(preced, flow_manager_p->fi_preced);  
      }
    }
    else
    {
      if(req_precedence)
      {
        /*-------------------------------------------------------------------
           Clear the Requested FI Preced for Tx
        -------------------------------------------------------------------*/
        ds_pdn_tft_clear_filter_preced_bit(preced,
                                           flow_manager_p->req_fi_preced);  
      }
      else
      {
        /*-------------------------------------------------------------------
           Clear the FI Preced for Tx
        -------------------------------------------------------------------*/
        ds_pdn_tft_clear_filter_preced_bit(preced, flow_manager_p->fi_preced);  
      }
    }
  }/* FOR TX  Filters */

  return TRUE; 
}/* ds_pdn_flow_manager_update_precedence_bit_mask*/

/*===========================================================================
FUNCTION DS_PDN_FLOW_MANAGER_CREATE_FLOW_CONTEXT

DESCRIPTION
  Creates new flow context.

PARAMETERS
  pdn_context_p         -  PDN information
  bearer_context_p      -  Bearer information
  inst_p                     - Instance pointer
  info_p           -  mode specific call info
  nw_tft_p              -  TFT in network format.
  icause_code           -  internal cause code to be updated.

DEPENDENCIES
  None.

RETURN VALUE
boolean - return TRUE or FALSE
  
SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_pdn_flow_manager_create_flow_context
(
  ds_pdn_mgr_s                                         *pdn_mgr_p,
  ds_bearer_mgr_s                                      *bearer_mgr_p,
  ds_pdn_mgr_inst_type_s                               *inst_p,
  emm_act_dedicated_bearer_context_request_ind_s_type  *info_p,
  tft_type_T                                           *nw_tft_p,
  ds_bearer_flow_internal_cause_code_enum_type         *icause_code,
  esm_cause_T                                          *esm_cause
)
{
  qos_spec_type                *qos_spec = NULL;
  ds_flow_type                 *flow_context = NULL;
  ds_bearer_flow_manager_s     *bflow_manager_p = NULL;
  ds_local_err_val_e_type       local_err = DS_LOCAL_ERR_VAL_NONE;

  if(icause_code == NULL)
  {
    local_err = DS_LOCAL_ERR_VAL_0;
    goto func_exit;
  }

  if(!ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p) ||
     !ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p) ||
     !inst_p || !info_p )
  {
    local_err = DS_LOCAL_ERR_VAL_1;
      *icause_code = DS_BEARER_FLOW_INTERNAL_INSUFFICIENT_RESOURCES;
    goto func_exit;
  }

  DS_MSG2(MSG_LEGACY_HIGH, "Create Flow Context for PDN %d, bearer %d",
          pdn_mgr_p->index, bearer_mgr_p->index);

  flow_context = ds_flow_allocate_context(inst_p);
  if(flow_context == NULL)
  {
    local_err = DS_LOCAL_ERR_VAL_2;
    *icause_code = DS_BEARER_FLOW_INTERNAL_INSUFFICIENT_RESOURCES;
    goto func_exit;
  }

  qos_spec = flow_context->qos_info_ptr;
  
  /*------------------------------------------------------------------------
    Convert NW Flow values to DS-Specific Qos value
    The conversion should be done only once.
  -------------------------------------------------------------------------*/
  if(ds_bearer_flow_manager_convert_data_rate(bearer_mgr_p,
                                              &info_p->sdf_qos,
                                              &info_p->neg_qos,
                                              qos_spec,
                                              icause_code) == FALSE)
  {
    local_err = DS_LOCAL_ERR_VAL_3;
    goto func_exit;
  }

  bflow_manager_p = ds_bearer_flow_manager_find_entry(bearer_mgr_p);
  if(bflow_manager_p == NULL)
  {
    local_err = DS_LOCAL_ERR_VAL_4;
    *icause_code = DS_BEARER_FLOW_INTERNAL_INSUFFICIENT_RESOURCES;
    goto func_exit;
  }

  /*-------------------------------------------------------------------------
    Verify Filter Fields 
  -------------------------------------------------------------------------*/
  if(nw_tft_p->valid)
  {
    if((nw_tft_p->tft_total_length < 3) ||
  	    (nw_tft_p->tft_total_length > 257))
    {
      local_err = DS_LOCAL_ERR_VAL_5;
      *icause_code = DS_BEARER_FLOW_INTERNAL_SYNTACTICAL_ERR_IN_TFT;
      goto func_exit;
    }
  
    if(nw_tft_p->e_bit != DS_FLOW_E_BIT_DISABLE)
    {
      local_err = DS_LOCAL_ERR_VAL_6;
      *icause_code = DS_BEARER_FLOW_INTERNAL_SERV_OPT_NOT_SUPPORTED;
      goto func_exit;
    }
  
    if(nw_tft_p->operation_code != DS_PDN_TFT_OPCODE_CREATE)
    {
      local_err = DS_LOCAL_ERR_VAL_7;
      *icause_code = DS_BEARER_FLOW_INTERNAL_SEMANTIC_ERR_IN_TFT;
      goto func_exit;
    }
   
    if((nw_tft_p->num_filters > DS_PDN_TFT_MAX_FILTERS) ||
    	  (nw_tft_p->num_filters < 1))
    {
      local_err = DS_LOCAL_ERR_VAL_8;
      *icause_code = DS_BEARER_FLOW_INTERNAL_SYNTACTICAL_ERR_IN_TFT;
      goto func_exit;
    }
   
    /*------------------------------------------------------------------------
       Process the Filters 
    -------------------------------------------------------------------------*/    
    DS_SYSTEM_HEAP_MEM_ALLOC(qos_spec->rx.fltr_template.list_ptr,
                             (MAX_PACKET_FILTERS*sizeof(ip_filter_type)),
                             ip_filter_type*);
    if(qos_spec->rx.fltr_template.list_ptr == NULL)
    {
      local_err = DS_LOCAL_ERR_VAL_9;
      goto func_exit;
    }
    memset(qos_spec->rx.fltr_template.list_ptr, 0, 
          (MAX_PACKET_FILTERS*sizeof(ip_filter_type)));
    
    DS_SYSTEM_HEAP_MEM_ALLOC(qos_spec->tx.fltr_template.list_ptr,
                             (MAX_PACKET_FILTERS*sizeof(ip_filter_type)),
                             ip_filter_type*);
    if(qos_spec->tx.fltr_template.list_ptr == NULL)
    {
      local_err = DS_LOCAL_ERR_VAL_10;
      goto func_exit;
    }    
    memset(qos_spec->tx.fltr_template.list_ptr, 0, 
           (MAX_PACKET_FILTERS*sizeof(ip_filter_type)));
      
    if(ds_pdn_flow_manager_convert_nw_tft_to_app_tft(nw_tft_p,
  	                                                  qos_spec, 
  	                                                  inst_p->type, 
  	                                                  flow_context, 
  	                                                  icause_code) == FALSE)
    {
      local_err = DS_LOCAL_ERR_VAL_11;
      goto func_exit;
    }
  }
  else
  {
    DS_ERROR_LOG_0("TFT valid flag is not set, skipping related ops");
  }

  if(ds_pdn_nv_manager_is_rel_12_or_above_supported())
  {
    if((qos_spec->tx.fltr_template.num_filters == 0) && 
       (!bearer_mgr_p->is_default)) 
    {
      *icause_code = DS_BEARER_FLOW_INTERNAL_SEMANTIC_ERR_IN_FILTER;  
      local_err = DS_LOCAL_ERR_VAL_12;
      goto func_exit;
    }
  } 

  if(ds_flow_create_flow_context(flow_context, 
                                 qos_spec, 
                                 pdn_mgr_p, 
                                 inst_p->type, 
                                 bearer_mgr_p) == FALSE)
  {
    *icause_code = DS_BEARER_FLOW_INTERNAL_SEMANTIC_ERR_IN_FILTER;  
    local_err = DS_LOCAL_ERR_VAL_13;
    goto func_exit;
  }

  // Disable flow to avoid data write
  ds_flow_enable_disable_tx(flow_context, FALSE);

  // Add flow context to bearer flow manager
  if(ds_bearer_flow_manager_add_flow_context_to_flow_manager(bearer_mgr_p, 
                                                         flow_context) == FALSE)
  {
    *icause_code = DS_BEARER_FLOW_INTERNAL_INSUFFICIENT_RESOURCES;
    local_err = DS_LOCAL_ERR_VAL_14;
    goto func_exit;
  }  

  /*-----------------------------------------------------------------------
       Set Precedence bit mask
    -----------------------------------------------------------------------*/
  if(ds_pdn_flow_manager_update_precedence_bit_mask(pdn_mgr_p, 
                                                    flow_context,
                                                    FALSE,
                                                    TRUE,
                                                    FALSE) == FALSE)
  {
    DS_ERROR_LOG_0("Error in setting precedence bit mask");
  }

  
  /*-----------------------------------------------------------------------
       Set identifier bit mask
    -----------------------------------------------------------------------*/
  if(ds_bearer_flow_manager_update_filter_identifier_bit_mask(
                                                bearer_mgr_p, 
                                                flow_context,
                                                TRUE) == FALSE)
  {
    DS_ERROR_LOG_0("Error in setting filter identifier bit mask in bearer");
  }

  if(ds_flow_update_filter_identifier_bit_mask(flow_context, TRUE) == FALSE)
  {
    DS_ERROR_LOG_0("Error in setting filter identifier bit mask in flow");
  }

func_exit:
  if(local_err != DS_LOCAL_ERR_VAL_NONE)
  {
    DS_ERROR_LOG_1("Create flow failed with error =d", local_err);
    if(flow_context != NULL)
    {
      ds_flow_release_flow_context(flow_context);
    }

    return FALSE;
  }
  
  return TRUE;
}

/*===========================================================================
FUNCTION DS_PDN_FLOW_MANAGER_UPDATE_APP_FILTER_TO_DELETE

DESCRIPTION
  Update the NW TFT with the filters to be deleted.

PARAMETERS
 nw_tft            -  network TFT to be converted
 flow_context_p    -  App flow to store the converted info.

DEPENDENCIES
  None.

RETURN VALUE
boolean          - returns TRUE if update is successfull
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_flow_manager_update_app_filter_to_delete
(
  tft_type_T          *nw_tft,
  ds_flow_type        *flow_context_p
)
{
  qos_spec_type           *orig_spec;  
  ip_filter_type          *fltr = NULL; 
  int                      cnt = 0;          
  uint8                    fi_identifier = 0;
  uint16                   temp_fi_id =0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  if(!ds_flow_is_valid(flow_context_p) || !nw_tft)
  {
    return FALSE;
  }

  orig_spec = flow_context_p->qos_info_ptr;
   

  /*------------------------------------------------------------------------
     Loop thru all filters and update the NW TFT
  -------------------------------------------------------------------------*/
  for(cnt =0; cnt<orig_spec->rx.fltr_template.num_filters; cnt++)
  {
    fltr = &(orig_spec->rx.fltr_template.list_ptr[cnt]);
    fi_identifier = (uint8)(fltr->ipfltr_aux_info.fi_id);
    /* Filter identifier is in first 4 bits.*/
    fi_identifier = fi_identifier & 0xF;
    temp_fi_id = fi_identifier;
    if(ds_pdn_tft_is_filter_identifier_requested(
                                    temp_fi_id,
                                    &(flow_context_p->ignore_fi_id)) == FALSE)
    {
      nw_tft->filter_list[nw_tft->num_filters].identifier = fi_identifier;
      nw_tft->num_filters= nw_tft->num_filters + 1;
    }
  }/* RX FOR Loop*/

  /*------------------------------------------------------------------------
     Loop thru all filters and update the precedence bit mask for TX
  -------------------------------------------------------------------------*/
  for(cnt =0; cnt<orig_spec->tx.fltr_template.num_filters; cnt++)
  {
    fltr = &(orig_spec->tx.fltr_template.list_ptr[cnt]);
    fi_identifier = (uint8)(fltr->ipfltr_aux_info.fi_id);
    // Filter identifier is in first 4 bits.
    fi_identifier = fi_identifier & 0xF;
    temp_fi_id = fi_identifier;
    if(ds_pdn_tft_is_filter_identifier_requested(
                                    temp_fi_id,
                                    &(flow_context_p->ignore_fi_id)) == FALSE)
    {
      nw_tft->filter_list[nw_tft->num_filters].identifier = fi_identifier;
      nw_tft->num_filters= nw_tft->num_filters + 1;    
    }
  }/* FOR TX  Filters */
  
  return TRUE;
} /*  ds_pdn_flow_manager_update_app_filter_to_delete */

/*===========================================================================
FUNCTION DS_PDN_FLOW_MANAGER_BEARER_MODIFY_REQ

DESCRIPTION
  Handle the bearer modification request.

PARAMETERS
  flow_context_p   -  Pointer to Flow context to be modified.
  nw_tft_p         -  Pointer to Network TFT.
  flow_state        -  flow state when the command was posted

DEPENDENCIES
  None.

RETURN VALUE
  boolean              - FALSE if it fails, TRUE if success.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_flow_manager_bearer_modify_req
(
  ds_flow_type             *flow_context_p,
  tft_type_T               *nw_tft_p
)
{
  emm_bearer_resource_modification_req_T  *res_mod = NULL;  
  ds_pdn_mgr_s                            *pdn_context_p = NULL;
  ds_pdn_mgr_inst_type_s                  *inst_p    = NULL;
  ds_bearer_mgr_s                         *bearer_context_p = NULL;
  ds_bearer_flow_manager_s                *bflow_manager_p = NULL;
  tft_type_T                              *nw_tft = NULL;
  ds_local_err_val_e_type                  local_err = DS_LOCAL_ERR_VAL_NONE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(!ds_flow_is_valid(flow_context_p))
  {
    local_err = DS_LOCAL_ERR_VAL_0;
    goto func_exit;
  }
     
  if( (inst_p = flow_context_p->inst_ptr) == NULL)
  {
    local_err = DS_LOCAL_ERR_VAL_1;
    goto func_exit;
  } 
   
  if(flow_context_p->bearer_ptr == NULL)
  {
    local_err = DS_LOCAL_ERR_VAL_2;
    goto func_exit;
  }

  bearer_context_p = flow_context_p->bearer_ptr;  
  pdn_context_p = bearer_context_p->pdn_mgr_ptr;
  if(!ds_pdn_mgr_validate_pdn_ptr(pdn_context_p))
  {
    local_err = DS_LOCAL_ERR_VAL_3;
    goto func_exit;
  }
  
  bflow_manager_p = flow_context_p->bearer_flow_mgr_ptr;

  if(!bflow_manager_p)
  {
    local_err = DS_LOCAL_ERR_VAL_4;
    goto func_exit;
  }

  DS_SYSTEM_HEAP_MEM_ALLOC(res_mod,
                           sizeof(emm_bearer_resource_modification_req_T),
                           emm_bearer_resource_modification_req_T*);
  if(res_mod == NULL)
  {
    local_err = DS_LOCAL_ERR_VAL_5;
    goto func_exit;
  }
   
  memset(res_mod, 0, sizeof(emm_bearer_resource_modification_req_T));
  res_mod->esm_cause.valid = FALSE;

  // Assign nw tft pointer
  nw_tft = &(res_mod->tad);

  /*-------------------------------------------------------------------
    Request deleting only the filters associated with the FLOW.
  -------------------------------------------------------------------*/
  if((nw_tft_p->valid) && 
     (nw_tft_p->operation_code == (byte)DS_PDN_TFT_OPCODE_DEL_FILTER))
  {
    DS_MSG0(MSG_LEGACY_HIGH, "Processing filled in tft del filter");
    memscpy(nw_tft, sizeof(tft_type_T), nw_tft_p, sizeof(tft_type_T));
  }
  else
  {
    nw_tft->valid = TRUE;
    nw_tft->operation_code = (byte)DS_PDN_TFT_OPCODE_DEL_FILTER;
    nw_tft->e_bit = 0;  
    if(ds_pdn_flow_manager_update_app_filter_to_delete(
                                             nw_tft,flow_context_p) == FALSE)
    {
      local_err = DS_LOCAL_ERR_VAL_6;
      goto func_exit;
    }
    nw_tft->tft_total_length = 1 + nw_tft->num_filters;
  }
  
  if(list_size(&(bflow_manager_p->flow_context)) == 1)
  {
    res_mod->esm_cause.valid = TRUE;
    res_mod->esm_cause.esm_cause = NAS_ESM_REGULAR_DEACTIVATION;
    // This logic is off if we have more than 1 bearer modification
    // on the same bearer.
  }
  
  /*---------------------------------------------------------------------
    Update the params and send out the NW request.
  ---------------------------------------------------------------------*/
  res_mod->esm_hdr.sdf_id                = flow_context_p->sdf_id;
  res_mod->esm_hdr.lbi                   = bearer_context_p->bearer_id;
  res_mod->protocol_config_options.valid = FALSE;

#ifndef FEATURE_DATA_MPSS_AT20_DEV
  res_mod->low_priority_signaling =
              ds_pdn_mgr_get_low_priority_signaling_bit(pdn_context_p);
#endif /* FEATURE_DATA_MPSS_AT20_DEV */

  res_mod->message_header.message_id = EMM_BEARER_RESRC_MOD_REQ;
  res_mod->message_header.message_set = MS_MM_DS;      
  res_mod->message_header.message_len_lsb =
    (sizeof( emm_bearer_resource_modification_req_T ) - sizeof(IMH_T)) % 256;
  res_mod->message_header.message_len_msb =
    (sizeof( emm_bearer_resource_modification_req_T ) - sizeof(IMH_T)) / 256;
  
  /* Set the connection ID as invalid */
  res_mod->esm_hdr.connection_id = ESM_INVALID_CON_ID;
  
  DS_MSG1(MSG_LEGACY_HIGH, 
          "Bearer Resource Modification Request for bearer %d",
          bearer_context_p->index);

  ds_pdn_mgr_ds_to_nas_send_msg((void*)res_mod);    

  ds_bearer_flow_manager_remove_flow_context_from_flow_manager(
                                    bearer_context_p, flow_context_p);

  // Assign the flow SDF ID,a s this will be used while finding the flow during response.
  bearer_context_p->sdf_id = flow_context_p->sdf_id;
  
  /*---------------------------------------------------------------------
    Move the flow context from bearer flow manager to pending context
    Convert App Qos to NW Qos.
  ---------------------------------------------------------------------*/
  ds_pdn_flow_manager_add_pending_flow_context(pdn_context_p, flow_context_p);
  
  /*---------------------------------------------------------------------
    Keep a copy of the UE request to NW.
  ---------------------------------------------------------------------*/
  memscpy(&(flow_context_p->tft_mod), sizeof(tft_type_T),
          nw_tft, sizeof(tft_type_T));

func_exit:  

  if(res_mod)
  {
    DS_SYSTEM_HEAP_MEM_FREE(res_mod);
  }

  if(local_err != DS_LOCAL_ERR_VAL_NONE)
  {
    DS_ERROR_LOG_1("ds_pdn_flow_manager_bearer_modify_req failed with err =d",
                   local_err);
    return FALSE;
  }
  
  return TRUE;
}/* ds_pdn_flow_manager_bearer_modify_req */

/*===========================================================================
FUNCTION DS_PDN_FLOW_MANAGER_TEARDOWN_BEARER

DESCRIPTION
  Function to process the secondary bearer tear down. 

PARAMETERS
  *bearer_context_p      - Ptr to bearer context
  *preced_conflict       - Ptr to array of precedence conflict values

DEPENDENCIES
  None.

RETURN VALUE
  TRUE   - On Success
  FALSE - On Failure
   
SIDE EFFECTS

===========================================================================*/
boolean ds_pdn_flow_manager_teardown_bearer
(
  ds_bearer_mgr_s      *bearer_context_p,
  uint32               *preced_conflict
)
{
  ds_pdn_flow_manager_s         *flow_manager_p = NULL;
  ds_bearer_flow_manager_s      *bflow_manager_p = NULL;
  boolean                        last_flow = FALSE;
  uint16                         loop =0;
  uint32                         temp_value = 1;
  tft_type_T                     nw_tft;
  uint32                         temp_fi_id = 0;
  ds_bearer_flow_manager_list_type  *bflow_list_p = NULL;
  ds_bearer_flow_manager_list_type  *next_flow_cntx = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  if(!ds_bearer_mgr_validate_bearer_ptr(bearer_context_p) || 
     (preced_conflict == NULL))
  {
    return FALSE;
  }
  
  /*---------------------------------------------------------------------
    Find the bearer flow manager
  ---------------------------------------------------------------------*/
  bflow_manager_p = ds_bearer_flow_manager_find_entry(bearer_context_p);
  if(!bflow_manager_p || !bflow_manager_p->pdn_ptr)
  {
    return FALSE;
  }

  /*---------------------------------------------------------------------
    Find the flow manager
  ---------------------------------------------------------------------*/
  flow_manager_p = ds_pdn_flow_mgr_get_entry(bflow_manager_p->pdn_ptr->index);
  if(!flow_manager_p)
  {
    return FALSE;
  }
  
  /*---------------------------------------------------------------------
    Initialize the network TFT
  ---------------------------------------------------------------------*/
  memset(&nw_tft, 0, sizeof(tft_type_T));

  // Copy filter identifier before clearing.
  temp_fi_id = bflow_manager_p->fi_identifier;

  /*---------------------------------------------------------------------
    Go through all the flow context list for the bearer. 
    Keep the last flow context.
  ---------------------------------------------------------------------*/
  bflow_list_p = list_peek_front(&(bflow_manager_p->flow_context)); 
  if(!bflow_list_p ||
  	  !bflow_list_p->flow_context_p)
  {
    return FALSE;
  }

  if(list_size(&(bflow_manager_p->flow_context)) == 1)
  {
    last_flow = TRUE;
  }
  
  while(!last_flow)
  {
    /*---------------------------------------------------------------------
      Update the Filter precedence bit mask
    ---------------------------------------------------------------------*/
    if(ds_pdn_flow_manager_update_precedence_bit_mask(
                   bflow_manager_p->pdn_ptr, bflow_list_p->flow_context_p,
                   FALSE, FALSE, FALSE) == FALSE)
    {
      DS_ERROR_LOG_0("Error in clearing precedence bit mask");
    }
    
    /*---------------------------------------------------------------------
      Update the Filter Identifier bit mask
    ---------------------------------------------------------------------*/
    if(ds_bearer_flow_manager_update_filter_identifier_bit_mask(
                  bflow_manager_p->bearer_ptr, 
                  bflow_list_p->flow_context_p, FALSE) == FALSE)
    {
      DS_ERROR_LOG_0("Error in clearing Filter identifier bit mask");
    }
 
    next_flow_cntx = list_peek_next(&(bflow_manager_p->flow_context),
                                    &(bflow_list_p->bflow_link));
    if(next_flow_cntx != NULL &&
    	  next_flow_cntx->flow_context_p != NULL)
    {
      list_pop_item(&(bflow_manager_p->flow_context),
                    &(bflow_list_p->bflow_link));
      
      DS_MSG3(MSG_LEGACY_HIGH, "Flow %x removed from bearer %d bearer flow %d", 
              bflow_list_p->flow_context_p, 
              bearer_context_p->index, bflow_manager_p);
      
      ds_flow_release_flow_context(bflow_list_p->flow_context_p);
      DS_SYSTEM_HEAP_MEM_FREE(bflow_list_p);
      
      bflow_list_p = next_flow_cntx;
    }
    else
    {
      last_flow = TRUE;  
    }  
  }/* while*/  
  
  /*---------------------------------------------------------------------
    Update the Filter precedence bit mask for last flow context
   ---------------------------------------------------------------------*/
  if(ds_pdn_flow_manager_update_precedence_bit_mask(
                     bflow_manager_p->pdn_ptr, bflow_list_p->flow_context_p,
                     FALSE, FALSE, FALSE) == FALSE)
  {
    DS_ERROR_LOG_0("Error in clearing precedence bit mask");
  }
  /*---------------------------------------------------------------------
    Update the Filter Identifier bit mask for last flow context
  ---------------------------------------------------------------------*/
  if(ds_bearer_flow_manager_update_filter_identifier_bit_mask(
                 bflow_manager_p->bearer_ptr, 
                 bflow_list_p->flow_context_p, FALSE) == FALSE)
  {
    DS_ERROR_LOG_0("Error in Clearing Filter identifier bit mask");
  }

  /*---------------------------------------------------------------------
    Reset the Filter precedence bit mask for conflict filter precedence
  ---------------------------------------------------------------------*/
  for(loop=0; loop<256; loop++)
  {
    if(ds_pdn_tft_is_filter_preced_bit_set(loop, preced_conflict))
    {
      ds_pdn_tft_set_filter_preced_bit(loop, flow_manager_p->fi_preced);      
      DS_MSG1(MSG_LEGACY_HIGH, "Precedence set for 0x:%x",loop);
    }
  }

  if(!bflow_list_p->flow_context_p)
  {
    DS_ERR_FATAL("Last Flow context is INVALID");
  }

  /*---------------------------------------------------------------------
    Configure the Filter identifier in the TFT
  ---------------------------------------------------------------------*/
  temp_value = 1;
  loop = 0;
  while (loop < 32)
  {
    if(temp_fi_id  & (temp_value << loop)) 
    {
      if(loop > 15)
      {
        DS_ERROR_LOG_1("Invalid FI-ID for =x", loop);
      }
      else
      {
        nw_tft.filter_list[nw_tft.num_filters].identifier =(byte)(loop);
        nw_tft.num_filters++; 
        DS_MSG1(MSG_LEGACY_HIGH, "FI-ID set for 0x:%x",loop);
      }
    }
    loop++;
  }

 /*---------------------------------------------------------------------
    Update TFT Length,other params
  ---------------------------------------------------------------------*/
  nw_tft.valid = TRUE;
  nw_tft.tft_total_length = 1 + nw_tft.num_filters;
  nw_tft.operation_code = (byte)DS_PDN_TFT_OPCODE_DEL_FILTER;
  nw_tft.e_bit = DS_FLOW_E_BIT_DISABLE; 

  /*---------------------------------------------------------------------
    Invoke the modify specific bearer modify.
  ---------------------------------------------------------------------*/
  if(ds_pdn_flow_manager_bearer_modify_req(bflow_list_p->flow_context_p,
                                           (tft_type_T *)&nw_tft) == FALSE)
  {
    DS_ERROR_LOG_0("UE bearer modify req failed");
    return FALSE;
  } 
  
  return TRUE;
}/* ds_pdn_flow_manager_teardown_bearer */

/*===========================================================================
FUNCTION DS_PDN_FLOW_MANAGER_HANDLE_FLTR_PRECED_CONFLICT

DESCRIPTION
  Handles the fltr preced conflict. Posts a command.

PARAMETERS
  pdn_context_p         -  PDN information
  bearer_context_p      -  Bearer information
  flow_context_p        -  Flow context
  nw_bk                 -  Network Mod book keeping info
 
DEPENDENCIES
  None.

RETURN VALUE
boolean - return TRUE or FALSE
  
SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_pdn_flow_manager_handle_fltr_preced_conflict
(
 ds_pdn_mgr_s                   *pdn_context_p,
 ds_bearer_mgr_s                *bearer_context_p,
 ds_pdn_tft_nw_modify_qos_bk_s  *nw_bk       
)
{
  uint32                             cnt =0;
  ds_command_type                   *cmd_ptr = NULL;
  ds_pdn_flow_manager_cmd_info_type *cmd_info_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  if(!ds_pdn_mgr_validate_pdn_ptr(pdn_context_p) ||
     !ds_bearer_mgr_validate_bearer_ptr(bearer_context_p) ||
     (nw_bk == NULL))
  {
    return FALSE;
  }

  /*---------------------------------------------------------------------
   If any of the bearers with the precedence conflict is the default bearer; 
   we need to release the PDN connection.
   Loop thru all the bearers
  -----------------------------------------------------------------------*/
  for (cnt=0; cnt < nw_bk->conflict_bearer_cnt; cnt++)
  {
    ds_bearer_mgr_s *bearer_w_conflict_p = nw_bk->conflict_bearer[cnt];

    if(nw_bk->prec_conf_on_same_bearer_create && 
       ((void*)bearer_context_p == (void*)(nw_bk->conflict_bearer[cnt])) )
    {
      DS_MSG1(MSG_LEGACY_HIGH,
              "Not posting command because precedence conflict on"
              "same bearer and opcode is create:%x", 
              nw_bk->conflict_bearer[cnt]);
      continue;
    }
    
    if((!ds_bearer_mgr_validate_bearer_ptr(bearer_w_conflict_p)))
    {
      continue;
    }

    if (TRUE == bearer_w_conflict_p->is_default)
    {
      DS_MSG1(MSG_LEGACY_HIGH, "Tearing Down PDN %d", pdn_context_p->index);     
      pdn_context_p->state = DS_PDN_MGR_STATE_GOING_DOWN;
      ds_pdn_mgr_teardown_instances_for_pdn(pdn_context_p, 
                                            DS_NET_DOWN_REASON_NOT_SPECIFIED);              
      return TRUE;
    }

    /*---------------------------------------------------------------
      Post a command.
    ----------------------------------------------------------------*/
    cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_pdn_flow_manager_cmd_info_type));
    
    if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
    {
      DS_ASSERT(0);
      return FALSE;
    }
    
    /* -----------------------------------------------------------------
       In modify bearer when Precedence conflict occures on same bearer
       (nw_bk->prec_conf_on_same_bearer set to TRUE)
       and OPCODE is create, do not post Precedence conflict command since 
       complete TFT is getting replaced so we will not have precedence conflict
       in resultant TFT.      
       -----------------------------------------------------------------*/  
    cmd_ptr->hdr.cmd_id = DS_CMD_PDN_FLOW_FLTR_PRECED_CONFLICT;
    
    cmd_info_ptr = (ds_pdn_flow_manager_cmd_info_type*)cmd_ptr->cmd_payload_ptr;
    cmd_info_ptr->flow_context_p = NULL;
    cmd_info_ptr->bearer_context_p = nw_bk->conflict_bearer[cnt];
    cmd_info_ptr->prec_conf_on_same_bearer_modify  = FALSE;
    
    if(nw_bk->prec_conf_on_same_bearer_modify == TRUE)
    {
      cmd_info_ptr->prec_conf_on_same_bearer_modify = TRUE;
    }
    
    memscpy((cmd_info_ptr->preced_conflict),
            (sizeof(uint32)*DS_FI_PRECED_ARR_SIZE),
            (nw_bk->preced_conflict),
            (sizeof(uint32)*DS_FI_PRECED_ARR_SIZE));
    
    DS_MSG1(MSG_LEGACY_HIGH,
            "Posted precedence conflict on bearer 0x:%x",
            nw_bk->conflict_bearer[cnt]);
    
    ds_put_cmd(cmd_ptr); 
  }/* for */

  return TRUE;
}/* ds_pdn_flow_manager_handle_fltr_preced_conflict */

/*===========================================================================
FUNCTION DS_PDN_FLOW_MANAGER_HANDLE_FLOW_ALLOCATION

DESCRIPTION
  Handles new flow allocation.

PARAMETERS
  pdn_context_p         -  PDN information
  bearer_context_p      -  Bearer information
  info_p           -  mode specific call info
  nw_tft_p              -  TFT in network format.
  icause_code           -  internal cause code to be updated.

DEPENDENCIES
  None.

RETURN VALUE
boolean - return TRUE or FALSE
  
SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_pdn_flow_manager_handle_flow_allocation
(
  ds_pdn_mgr_s                                         *pdn_mgr_p,
  ds_bearer_mgr_s                                      *bearer_mgr_p,
  emm_act_dedicated_bearer_context_request_ind_s_type  *info_p,
  tft_type_T                                           *nw_tft_p,
  ds_bearer_flow_internal_cause_code_enum_type         *icause_code,
  esm_cause_T                                          *esm_cause
)
{
  ds_pdn_mgr_inst_type_s        *v4_inst = NULL, *v6_inst = NULL;
  uint8                          index =  0;
  packet_filter_type_T          *nw_filter =  NULL;
  uint16                         filter_id = 0, preced = 0;
  uint32                         temp_filter_id = 0;
  uint32                         temp_preced[DS_FI_PRECED_ARR_SIZE] = {0};
  ds_pdn_flow_manager_s         *pflow_manager_p;
  int                            preced_conflict = 0;
  uint32                         preced_conflict_ar[DS_FI_PRECED_ARR_SIZE] = {0};
  ds_pdn_tft_nw_modify_qos_bk_s *nw_bk = NULL;
  ds_profile_pdp_type_e          pdn_pdp_type;
  ds_local_err_val_e_type        local_err = DS_LOCAL_ERR_VAL_NONE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(icause_code == NULL)
  {
    local_err = DS_LOCAL_ERR_VAL_0;
    goto func_exit;
  }

  if(!ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p) ||
     !ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p) || !info_p )
  {
    local_err = DS_LOCAL_ERR_VAL_1;
      *icause_code = DS_BEARER_FLOW_INTERNAL_INSUFFICIENT_RESOURCES;
    goto func_exit;
  }
  
  DS_MSG2(MSG_LEGACY_HIGH, "Handle Flow allocation for PDN %d, bearer %d",
          pdn_mgr_p->index, bearer_mgr_p->index);
  
  /*---------------------------------------------------------------------
     Find the flow manager.
  ---------------------------------------------------------------------*/ 
  pflow_manager_p = ds_pdn_flow_mgr_get_entry(pdn_mgr_p->index);
  if(!pflow_manager_p)
  { 
    local_err = DS_LOCAL_ERR_VAL_2;
    *icause_code = DS_BEARER_FLOW_INTERNAL_INSUFFICIENT_RESOURCES;
    goto func_exit;
  }
  
  /*-------------------------------------------------------------------------
    Verify Filter Fields 
  -------------------------------------------------------------------------*/
  if(nw_tft_p == NULL || (nw_tft_p->valid != TRUE))
  {
    local_err = DS_LOCAL_ERR_VAL_3;
    *icause_code = DS_BEARER_FLOW_INTERNAL_INVALID_MANDATORY_INFO;
    goto func_exit;
  }
  
  /*-------------------------------------------------------------------------
    Check filter content
  -------------------------------------------------------------------------*/
  for(index=0; index < nw_tft_p->num_filters; index++)
  {
    /*----------------------------------------------------------------------
     Check each filter
    -----------------------------------------------------------------------*/    
    nw_filter = &(nw_tft_p->filter_list[index]);
    filter_id = nw_filter->identifier & 0xF;
    preced = nw_filter->eval_precedence;

    /*---------------------------------------------------------------------
      Verify if the filter ID is repeated in the request.
    ---------------------------------------------------------------------*/
    if(ds_pdn_tft_is_filter_identifier_requested(
                                     filter_id, &temp_filter_id) == FALSE)
    {
      ds_pdn_tft_set_filter_identifier_bit(filter_id,&temp_filter_id);
    }
    else
    {      
      local_err = DS_LOCAL_ERR_VAL_4;
      *icause_code = DS_BEARER_FLOW_INTERNAL_SYNTACTICAL_ERR_IN_FILTER;
      if(ds_bearer_flow_manager_convert_cause_code(esm_cause,
                                                   icause_code)== FALSE)
      {
        local_err = DS_LOCAL_ERR_VAL_5;
      }

      goto func_exit;
    }

    /*---------------------------------------------------------------------
      Verify precedence is repeated 
    ---------------------------------------------------------------------*/
    if(ds_pdn_tft_is_filter_preced_bit_set(preced,temp_preced)==FALSE) 
    {
      ds_pdn_tft_set_filter_preced_bit(preced, temp_preced);
    }
    else
    {
      local_err = DS_LOCAL_ERR_VAL_6;
      *icause_code = DS_BEARER_FLOW_INTERNAL_SYNTACTICAL_ERR_IN_FILTER;
      if(ds_bearer_flow_manager_convert_cause_code(esm_cause,
                                                   icause_code)== FALSE)
      {
        local_err = DS_LOCAL_ERR_VAL_7;
      }
      
      goto func_exit;    
    } 

    /*---------------------------------------------------------------------
      Detect Precedence Conflict
    ---------------------------------------------------------------------*/
    if(ds_pdn_tft_is_filter_preced_bit_set(preced,
                                           pflow_manager_p->fi_preced) == TRUE)
    {
      DS_MSG1(MSG_LEGACY_HIGH, "Precedence Conflict 0x:%x", preced);
      preced_conflict++;
      ds_pdn_tft_set_filter_preced_bit(preced, preced_conflict_ar);
    }
  }/* for index */

  /*-------------------------------------------------------------------------
    Find out the PDP Type supported by this PDN.
  -------------------------------------------------------------------------*/
  pdn_pdp_type = ds_apn_table_get_pdn_pdp_type(pdn_mgr_p);

  if(pdn_pdp_type == DS_PROFILE_PDP_IPV4 ||
     pdn_pdp_type == DS_PROFILE_PDP_IPV4V6 )
  {
    v4_inst = pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV4];
    if(ds_pdn_flow_manager_create_flow_context(
                                 pdn_mgr_p,
                                 bearer_mgr_p,
                                 v4_inst,
                                 info_p,
                                 nw_tft_p,
                                 icause_code,
                                 esm_cause) != TRUE)
    {
      local_err = DS_LOCAL_ERR_VAL_8;
      ds_bearer_flow_manager_release_flow(bearer_mgr_p);
      if(ds_bearer_flow_manager_convert_cause_code(esm_cause, icause_code)
                                                                     == FALSE)
      {
        local_err = DS_LOCAL_ERR_VAL_9;
      }
      
      goto func_exit;
    }
  }

  if(pdn_pdp_type == DS_PROFILE_PDP_IPV6 ||
     pdn_pdp_type == DS_PROFILE_PDP_IPV4V6 )
  {
    v6_inst = pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV6];
    if(ds_pdn_flow_manager_create_flow_context(
                                 pdn_mgr_p,
                                 bearer_mgr_p,
                                 v6_inst,
                                 info_p,
                                 nw_tft_p,
                                 icause_code,
                                 esm_cause) != TRUE)
    {
      local_err = DS_LOCAL_ERR_VAL_10;    
      ds_bearer_flow_manager_release_flow(bearer_mgr_p);
      if(ds_bearer_flow_manager_convert_cause_code(esm_cause, icause_code)
                                                                     == FALSE)
      {
        local_err = DS_LOCAL_ERR_VAL_11;
      }
      
      goto func_exit;
    }
  }

  if(preced_conflict >  0)
  {
    /*-----------------------------------------------------------------------
      Allocate Heap Memory
    -----------------------------------------------------------------------*/
    DS_SYSTEM_HEAP_MEM_ALLOC(nw_bk,
                             sizeof(ds_pdn_tft_nw_modify_qos_bk_s),
                             ds_pdn_tft_nw_modify_qos_bk_s*);
    if(nw_bk == NULL)
    {
      local_err = DS_LOCAL_ERR_VAL_12;
      goto func_exit;
    }
    
    memset(nw_bk, 0,sizeof(ds_pdn_tft_nw_modify_qos_bk_s));
    
    // Update the preced conflict information.
    nw_bk->preced_conflict_cnt = preced_conflict;
    memscpy(nw_bk->preced_conflict, (sizeof(uint32) * DS_FI_PRECED_ARR_SIZE),
	           preced_conflict_ar, (sizeof(uint32) * DS_FI_PRECED_ARR_SIZE));

    if(ds_pdn_tft_find_bearer_with_preced_conflict(pdn_mgr_p,
                                                   bearer_mgr_p,
                                                   nw_bk) == FALSE)
    {
      DS_SYSTEM_HEAP_MEM_FREE(nw_bk);
      local_err = DS_LOCAL_ERR_VAL_13;
      goto func_exit;
    }
  
    /* Handle precedence conflict */
    if(ds_pdn_flow_manager_handle_fltr_preced_conflict(pdn_mgr_p,
                                                       bearer_mgr_p,
                                                       nw_bk) == FALSE)
    {      
      DS_SYSTEM_HEAP_MEM_FREE(nw_bk);
      local_err = DS_LOCAL_ERR_VAL_14;
      goto func_exit;
      
    }

    /* release heap memory */
    DS_SYSTEM_HEAP_MEM_FREE(nw_bk);
  }/* preced_conflict >  0 */

  /*-------------------------------------------------------------------------
    Add Bearer Flow manager to Flow manager.
  -------------------------------------------------------------------------*/
  if ((ds_pdn_flow_mgr_add_bearer_to_pdn_flow(pdn_mgr_p,
                                              bearer_mgr_p)) == FALSE)
  {
    local_err = DS_LOCAL_ERR_VAL_15;
    goto func_exit;
  }

func_exit:
  if(local_err != DS_LOCAL_ERR_VAL_NONE)
  {
    DS_ERROR_LOG_1("Create flow failed with error =d", local_err);
    return FALSE;
  }
  
  return TRUE;
} /*ds_pdn_flow_manager_create_flow_context */

/*===========================================================================
FUNCTION DS_PDN_FLOW_MANAGER_BEARER_ALLOCATION_IND

DESCRIPTION
  Handle the bearer allocation Indication.

PARAMETERS
  pdn_ptr         -  Ptr to the PDN context.
  bearer_ptr      -  Ptr to the bearer context.
  mode_info_ptr   -  CM passed mode information.

DEPENDENCIES
  None.

RETURN VALUE
  DS3G_SUCCESS or DS3G_FAILURE.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_flow_manager_bearer_allocation_ind
(
  ds_pdn_mgr_s               *pdn_mgr_p,
  ds_bearer_mgr_s            *bearer_mgr_p,
  emm_act_dedicated_bearer_context_request_ind_s_type   
                             *info_ptr,
  esm_cause_T                *esm_cause                            
)
{
  ds_bearer_flow_manager_s   *bflow_manager_p = NULL;
  ds_pdn_flow_manager_s      *pflow_manager_p = NULL;
  ds_bearer_flow_internal_cause_code_enum_type
                              icause_code = DS_BEARER_FLOW_INTERNAL_CC_INVALID;
  ds_local_err_val_e_type     local_err = DS_LOCAL_ERR_VAL_NONE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  if(!ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p) ||
     !ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p) ||
     (info_ptr == NULL) || (esm_cause == NULL))
  {
    local_err = DS_LOCAL_ERR_VAL_0;
    goto func_exit;
  }

  DS_MSG1(MSG_LEGACY_HIGH, "Bearer allocation ind for bearer id %d",
          bearer_mgr_p->index);

  /*-------------------------------------------------------------------------
    Verify FLOW Fields 
  -------------------------------------------------------------------------*/
  if(info_ptr->sdf_qos.valid != TRUE)
  {
    local_err = DS_LOCAL_ERR_VAL_1;
    esm_cause->valid = TRUE;
    esm_cause->esm_cause = NAS_ESM_INVALID_MANDATORY_INFORMATION;
    goto func_exit;
  }
  
  /*------------------------------------------------------------------------
    Check if the PDN has flow manager associated with it.
  -------------------------------------------------------------------------*/
  pflow_manager_p = ds_pdn_flow_mgr_get_entry(pdn_mgr_p->index);
  if(pflow_manager_p == NULL)
  {
    local_err = DS_LOCAL_ERR_VAL_2;
    esm_cause->valid = TRUE;
    esm_cause->esm_cause = NAS_ESM_INSUFFICIENT_RESOURCES;
    goto func_exit;
  }

  /*-------------------------------------------------------------------------
    Verify if the bearer already has a flow manager.
  -------------------------------------------------------------------------*/
  bflow_manager_p = ds_bearer_flow_manager_find_entry(bearer_mgr_p);
  if(bflow_manager_p != NULL) 
  {
    local_err = DS_LOCAL_ERR_VAL_3;
    esm_cause->valid = TRUE;
    esm_cause->esm_cause = NAS_ESM_INSUFFICIENT_RESOURCES;
    goto func_exit;
  }

  /*-------------------------------------------------------------------------
    Allocate a new bearer flow manager.
  -------------------------------------------------------------------------*/
  bflow_manager_p = ds_bearer_flow_manager_allocate_mem(bearer_mgr_p);
  if(bflow_manager_p == NULL)
  {
    local_err = DS_LOCAL_ERR_VAL_4;
    esm_cause->valid = TRUE;
    esm_cause->esm_cause = NAS_ESM_INSUFFICIENT_RESOURCES;
    goto func_exit;
  }

  /*---------------------------------------------------------------------
     Update the PDN PTR.
  ---------------------------------------------------------------------*/
  bflow_manager_p->pdn_ptr = pdn_mgr_p;      

  if(ds_pdn_flow_manager_handle_flow_allocation(pdn_mgr_p,
                                                bearer_mgr_p,
                                                info_ptr,
                                                &(info_ptr->tft),
                                                &icause_code,
                                                esm_cause) == FALSE)
  {
    local_err = DS_LOCAL_ERR_VAL_5;
    ds_bearer_flow_manager_release_flow(bearer_mgr_p);
    if(esm_cause->valid != TRUE)
    {      
      esm_cause->valid = TRUE;
      esm_cause->esm_cause = NAS_ESM_INSUFFICIENT_RESOURCES;
    }
    
    goto func_exit;
  }

func_exit:
  if(local_err != DS_LOCAL_ERR_VAL_NONE)
  {
    DS_ERROR_LOG_1("Bearer flow allocation indication failed with err =d",
                   local_err);
    return FALSE;
  }

  return TRUE;
}/* ds_pdn_flow_manager_bearer_allocation_ind */

/*===========================================================================
FUNCTION DS_PDN_FLOW_MANAGER_DETECT_CONFLICT_IN_TFT

DESCRIPTION
  Detects precedence and identifier conflicts on the Nw input.

PARAMETERS
  pdn_context_p         -  PDN information
  bearer_context_p      -  Bearer information
  flow_context_p        -  Flow context information
  nw_tft                -  TFT information
  nw_bk                 -  Network book keeping info.
  cause_code_p          -  Cause code NW TFT is not accurate

DEPENDENCIES
  None.

RETURN VALUE
boolean - return TRUE or FALSE
  
SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_pdn_flow_manager_detect_conflict_in_tft
(
  ds_pdn_mgr_s                                 *pdn_context_p,
  ds_bearer_mgr_s                              *bearer_context_p,
  ds_flow_type                                 *flow_context_p,
  tft_type_T                                   *nw_tft,
  ds_pdn_tft_nw_modify_qos_bk_s                *nw_bk,
  ds_bearer_flow_internal_cause_code_enum_type *cause_code_p,
  boolean                                       del_filter
)
{
  /*-------------------------------------------------------------------
    Check if the new fltr has any fltr ID or fltr preced conflicts.
  -------------------------------------------------------------------*/
  if(ds_pdn_tft_check_nw_fltr_id_preced(pdn_context_p,
                                        bearer_context_p,
                                        flow_context_p,
                                        nw_tft,
                                        nw_bk,
                                        del_filter) == FALSE)
  {
    *cause_code_p = DS_BEARER_FLOW_INTERNAL_SYNTACTICAL_ERR_IN_FILTER;
    return FALSE;
  }

  /*-------------------------------------------------------------------
    Check if there is a precedence conflict and make sure there is no
    conlfict on the bearer with modification
  -------------------------------------------------------------------*/
  if(nw_bk->preced_conflict_cnt != 0)
  {
    if(ds_pdn_tft_find_bearer_with_preced_conflict(pdn_context_p,
                                                   bearer_context_p,
                                                   nw_bk) == FALSE)
    {
      *cause_code_p = DS_BEARER_FLOW_INTERNAL_INSUFFICIENT_RESOURCES;
      return FALSE;
    }

    if(ds_pdn_tft_preced_conflict_on_bearer(
                                       nw_bk,(void *)bearer_context_p) == TRUE)
    {
    /* When op code is not create and precendence conflict is on the same bearer
      for now continuing to make it a semantic error; still understanding the best 
      way to resolve; but when opcode is create we are setting nw_bk->prec_conf_on_same_bearer
      and not returing an error */
      if( nw_tft->operation_code != (byte)DS_PDN_TFT_OPCODE_CREATE )
      {
        nw_bk->prec_conf_on_same_bearer_modify = TRUE;
      }          
      else
      {
         nw_bk->prec_conf_on_same_bearer_create = TRUE;
      }
    }          
  } /* if preced_conflict_cnt != 0 */
  
  if(nw_bk->is_add == TRUE)
  {
    /*-------------------------------------------------------------------
      Verify flow filter id coflict when it is new flow.
    -------------------------------------------------------------------*/     
    if((nw_bk->new_flow == TRUE) && (nw_bk->flow_cntx_fltr_id != 0))
    {
      DS_ERROR_LOG_1("filter ID conflict on filters =x",
                     nw_bk->flow_cntx_fltr_id);
      *cause_code_p = DS_BEARER_FLOW_INTERNAL_SYNTACTICAL_ERR_IN_TFT;
      return FALSE; 
    }
  }
  
  return TRUE;
}/* ds_pdn_flow_manager_detect_conflict_in_tft */

/*===========================================================================
FUNCTION DS_PDN_FLOW_MANAGER_UPDATE_FILTER_ID_PRECEDENCE

DESCRIPTION
  Update the filter ID and precedence for the filters.

PARAMETERS
  pdn_context_p    - PDN information
  bearer_context_p - Bearer information
  flow_context_p   - Flow Context information.
  filter_id        - Filter to be removed.
  app_qos_spec     - Original filter spec.
  clear_bit        -  If True will clear the bit else will set.
DEPENDENCIES
  None.

RETURN VALUE
  boolean          - Returns true if filters are retrieved
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_flow_manager_update_filter_id_precedence
( 
 ds_pdn_mgr_s         *pdn_context_p,
 ds_bearer_mgr_s      *bearer_context_p,
 ds_flow_type         *flow_context_p,
 uint32                filter_id,
 qos_spec_type        *app_qos_spec,
 boolean               clear_bit
)
{
  ip_filter_type            *fltr = NULL;
  int                        cnt = 0;       
  uint16                     identifier = 0;
  ds_bearer_flow_manager_s  *bflow_manager_p = NULL;
  ds_pdn_flow_manager_s     *flow_manager_p = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  bflow_manager_p = ds_bearer_flow_manager_find_entry(bearer_context_p);

  if(!bflow_manager_p)
  {
    return FALSE;
  }

  flow_manager_p = ds_pdn_flow_mgr_get_entry(pdn_context_p->index);
  if(!flow_manager_p)
  {
    return FALSE;
  }
  
  /*------------------------------------------------------------------------
    Loop thru all filters and update the filter identifier bit mask for RX
  -------------------------------------------------------------------------*/
  for(cnt =0; cnt<app_qos_spec->rx.fltr_template.num_filters; cnt++)
  {
    fltr = &(app_qos_spec->rx.fltr_template.list_ptr[cnt]);
    /*---------------------------------------------------------------------
      NW Assigned filter id is in the last 4 bits.
    ---------------------------------------------------------------------*/    
    identifier = (fltr->ipfltr_aux_info.fi_id & (0xF));
    if (((1 << identifier) & filter_id) || (filter_id == 0))
    {
      if(clear_bit)
      {
        /*-------------------------------------------------------------------
          Clear the FI Identifier
        -------------------------------------------------------------------*/
        ds_pdn_tft_clear_filter_identifier_bit(
                                 identifier,
                                 &(bflow_manager_p->fi_identifier));
        /*-------------------------------------------------------------------
          Clear the FI Identifier
        -------------------------------------------------------------------*/
        ds_pdn_tft_clear_filter_identifier_bit(
                                 identifier,
                                 &(flow_context_p->fi_identifier));  
        /*-------------------------------------------------------------------
          Clear the FI precedence
        -------------------------------------------------------------------*/
        ds_pdn_tft_clear_filter_preced_bit(
                                 fltr->ipfltr_aux_info.fi_precedence,
                                (flow_manager_p->fi_preced));
      }/* if clear_bit*/
      else
      {
        /*-------------------------------------------------------------------
          Set the FI Identifier
        -------------------------------------------------------------------*/
        ds_pdn_tft_set_filter_identifier_bit(
                                 identifier,
                                 &(bflow_manager_p->fi_identifier));
        /*-------------------------------------------------------------------
          Set the FI Identifier
        -------------------------------------------------------------------*/
        ds_pdn_tft_set_filter_identifier_bit(
                                 identifier,
                                 &(flow_context_p->fi_identifier));  
        /*-------------------------------------------------------------------
          Set the FI precedence
        -------------------------------------------------------------------*/
        ds_pdn_tft_set_filter_preced_bit(
                                 fltr->ipfltr_aux_info.fi_precedence,
                                 (flow_manager_p->fi_preced));
      }/* else clear_bit*/
    }/* if filter id matches*/    
  }/* RX FOR Loop*/

  /*------------------------------------------------------------------------
    Loop thru all filters and update the filter identifier bit mask for TX
  -------------------------------------------------------------------------*/
  for(cnt =0; cnt<app_qos_spec->tx.fltr_template.num_filters; cnt++)
  {
    fltr = &(app_qos_spec->tx.fltr_template.list_ptr[cnt]);
    /*---------------------------------------------------------------------
      NW Assigned filter id is in the last 4 bits.
    ---------------------------------------------------------------------*/
    identifier = (fltr->ipfltr_aux_info.fi_id & (0xF));
    if (((1 << identifier) & filter_id) || (filter_id == 0))
    {
      if(clear_bit)
      {
        /*-------------------------------------------------------------------
          Clear the FI Identifier
        -------------------------------------------------------------------*/
        ds_pdn_tft_clear_filter_identifier_bit(
                                 identifier,
                                 &(bflow_manager_p->fi_identifier));
        /*-------------------------------------------------------------------
          Clear the FI Identifier
        -------------------------------------------------------------------*/
        ds_pdn_tft_clear_filter_identifier_bit(
                                 identifier,
                                 &(flow_context_p->fi_identifier));  
        /*-------------------------------------------------------------------
          Clear the FI precedence
        -------------------------------------------------------------------*/
        ds_pdn_tft_clear_filter_preced_bit(
                                 fltr->ipfltr_aux_info.fi_precedence,
                                 (flow_manager_p->fi_preced));
      }/* if clear_bit*/
      else
      {
        /*-------------------------------------------------------------------
          Set the FI Identifier
        -------------------------------------------------------------------*/
        ds_pdn_tft_set_filter_identifier_bit(
                                 identifier,
                                 &(bflow_manager_p->fi_identifier));
        /*-------------------------------------------------------------------
          Set the FI Identifier
        -------------------------------------------------------------------*/
        ds_pdn_tft_set_filter_identifier_bit(
                                 identifier,
                                 &(flow_context_p->fi_identifier));
        /*-------------------------------------------------------------------
          Set the FI precedence
        -------------------------------------------------------------------*/
        ds_pdn_tft_set_filter_preced_bit(
                                 fltr->ipfltr_aux_info.fi_precedence,
                                 (flow_manager_p->fi_preced));
      }/* else clear_bit*/
    }/* if filter id matches */
  }/* TX FOR Loop*/
  
  return TRUE;
}/* ds_pdn_flow_manager_update_filter_id_precedence*/

/*===========================================================================
FUNCTION DS_PDN_FLOW_MANAGER_ADD_FILTER_TO_FLOW

DESCRIPTION
  Modify existing TFT.

PARAMETERS
  pdn_context_p         -  PDN information
  bearer_context_p      -  Bearer information
  flow_context_p        -  Flow context to add filters.
  nw_tft                -  TFT information
  nw_bk                 -  Book keeping information.
  mode_info_p           -  Mode specific Qos.
  new_flow              -  Indicates a new NW flow is created.

DEPENDENCIES
  None.

RETURN VALUE
boolean - return TRUE or FALSE
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_flow_manager_add_filter_to_flow
(
  ds_pdn_mgr_s                                 *pdn_context_p,
  ds_bearer_mgr_s                              *bearer_context_p,
  ds_flow_type                                 *flow_context_p,
  tft_type_T                                   *nw_tft,
  ds_pdn_tft_nw_modify_qos_bk_s                *nw_bk,
  emm_modify_bearer_context_request_ind_s_type *mode_info_p,
  boolean                                       new_flow,
  ds_bearer_flow_internal_cause_code_enum_type *cause_code_p
)
{
  ds_pdn_mgr_inst_type_s     *inst_ptr = NULL;
  packet_filter_type_T       *nw_filter=NULL;
  uint16                      filter_id = 0;
  byte                        direction  =0;
  ds_bearer_flow_manager_s   *bflow_manager_p = NULL;
  ds_pdn_flow_manager_s      *flow_manager_p  = NULL;
  ds_pdn_tft_pkt_filter_direction_enum_type 
                              filter_direction;
  ds_ipfltr_ctrl_param_type   fltr_ctrl_param = {0};
  int16                       err_no;
  int                         cnt = 0;
  boolean                     result = FALSE;
  uint8                       orig_rx_filter_cnt=0, orig_tx_filter_cnt=0;
  uint8                       torig_rx_filter_cnt=0,torig_tx_filter_cnt=0;  
  ip_filter_type             *temp_rx_fltr_list = NULL;
  ip_filter_type             *temp_tx_fltr_list = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  *cause_code_p = DS_BEARER_FLOW_INTERNAL_INSUFFICIENT_RESOURCES;

  bflow_manager_p = ds_bearer_flow_manager_find_entry(bearer_context_p);
  if(!bflow_manager_p)
  {
    return FALSE;
  }

  flow_manager_p = ds_pdn_flow_mgr_get_entry(pdn_context_p->index);
  if(flow_manager_p == NULL)
  {
    return FALSE;
  }
              
  /*-------------------------------------------------------------------------
    Configure the QOS flow spec
  -------------------------------------------------------------------------*/
  if(ds_pdn_tft_configure_nw_bk_flow_spec(nw_bk) == FALSE)
  {
    return FALSE;
  }

  /*-------------------------------------------------------------------
    Retrieve current filter info for flow context.
  ---------------------------------------------------------------------*/
  if(new_flow == FALSE)
  {    
    memscpy(&nw_bk->curr_qos, sizeof(qos_spec_type),
            flow_context_p->qos_info_ptr, sizeof(qos_spec_type));    
    
    nw_bk->curr_qos.rx.fltr_template.list_ptr = nw_bk->curr_rx_fltr;    
    memscpy(nw_bk->curr_qos.rx.fltr_template.list_ptr,
            sizeof(ip_filter_type)*MAX_PACKET_FILTERS,
            flow_context_p->qos_info_ptr->rx.fltr_template.list_ptr,
            sizeof(ip_filter_type)*MAX_PACKET_FILTERS);
    
    nw_bk->curr_qos.tx.fltr_template.list_ptr = nw_bk->curr_tx_fltr;    
    memscpy(nw_bk->curr_qos.tx.fltr_template.list_ptr,
            sizeof(ip_filter_type)*MAX_PACKET_FILTERS,
            flow_context_p->qos_info_ptr->tx.fltr_template.list_ptr,
            sizeof(ip_filter_type)*MAX_PACKET_FILTERS);
  }

  inst_ptr = flow_context_p->inst_ptr;
  if(!inst_ptr)
  {
    return FALSE;
  }
  
  /*-------------------------------------------------------------------
    Keep track of the orig rx and orig tx filter cnt
  ---------------------------------------------------------------------*/
  orig_rx_filter_cnt = nw_bk->curr_qos.rx.fltr_template.num_filters;
  orig_tx_filter_cnt = nw_bk->curr_qos.tx.fltr_template.num_filters;

  /*-------------------------------------------------------------------
    If there is a filter-identifier clash on NW Flow.
  ---------------------------------------------------------------------*/
  if(nw_tft->operation_code ==(byte) DS_PDN_TFT_OPCODE_CREATE)
  {
    nw_bk->curr_qos.rx.fltr_template.list_ptr = nw_bk->mod_rx_fltr;
    nw_bk->curr_qos.tx.fltr_template.list_ptr = nw_bk->mod_tx_fltr;
    nw_bk->curr_qos.rx.fltr_template.num_filters = 
                                  nw_bk->mod_qos.rx.fltr_template.num_filters;
    nw_bk->curr_qos.tx.fltr_template.num_filters = 
                                  nw_bk->mod_qos.tx.fltr_template.num_filters;
    nw_bk->flow_cntx_fltr_id = 0;
    nw_bk->bearer_fltr_id = 0;
  }
  else
  {
    if(nw_bk->flow_cntx_fltr_id != 0)
    {
      /*-------------------------------------------------------------------
        Remove the flow filters.
      ---------------------------------------------------------------------*/
      if(ds_flow_delete_filter_from_flow(flow_context_p,
                                         nw_bk->flow_cntx_fltr_id,
                                         &nw_bk->curr_qos, 
                                         &nw_bk->mod_qos)==FALSE)
      {
        DS_ERROR_LOG_0("error deleting filter from flow");
        return FALSE;
      }
    
      /*-------------------------------------------------------------------
        Update the filter pointers. The filter count is already stored.
      ---------------------------------------------------------------------*/
      nw_bk->curr_qos.rx.fltr_template.list_ptr = nw_bk->mod_rx_fltr;
      nw_bk->curr_qos.tx.fltr_template.list_ptr = nw_bk->mod_tx_fltr;
      nw_bk->curr_qos.rx.fltr_template.num_filters = 
                                    nw_bk->mod_qos.rx.fltr_template.num_filters;
      nw_bk->curr_qos.tx.fltr_template.num_filters = 
                                    nw_bk->mod_qos.tx.fltr_template.num_filters;
    }/* nw_bk->flow_cntx_fltr_idd != 0 */
  }

  /*-----------------------------------------------------------------------
    Loop through all the filters and add them to the NW flow context.
  -----------------------------------------------------------------------*/
  for(cnt=0; cnt< nw_tft->num_filters; cnt++)
  {
    nw_filter = &(nw_tft->filter_list[cnt]);
    filter_id = nw_filter->identifier & 0xF;
    direction = ((nw_filter->identifier & 0x30)>>4);
    filter_direction =(ds_pdn_tft_pkt_filter_direction_enum_type)direction; 
    switch(filter_direction)
    {
      case DS_PDN_TFT_UPLINK:
        if(nw_bk->curr_qos.tx.fltr_template.num_filters >= MAX_PACKET_FILTERS)
        {
          DS_ERROR_LOG_0("Excedding UE supported max filters for uplink ");
          return FALSE;
        }
        
        result = ds_pdn_flow_manager_convert_nw_filter_to_app_filter(
                  nw_filter,
                  &(nw_bk->curr_qos.tx.fltr_template.list_ptr[
                               nw_bk->curr_qos.tx.fltr_template.num_filters]),
                  (byte)filter_id, 
                  filter_direction, 
                  inst_ptr->type, 
                  flow_context_p,
                  cause_code_p);
        if(result == FALSE)
        {
          DS_ERROR_LOG_1("Error returned for filter No: =d,in Uplink",cnt);
          return FALSE;
        }
        nw_bk->curr_qos.tx.fltr_template.num_filters++;
        break;

      case DS_PDN_TFT_DOWNLINK:
        if(nw_bk->curr_qos.rx.fltr_template.num_filters >= MAX_PACKET_FILTERS)
        {
          DS_ERROR_LOG_0("Excedding UE supported max filters for downlink");
          return FALSE;
        }
        
        result = ds_pdn_flow_manager_convert_nw_filter_to_app_filter(
                   nw_filter,
	                  &(nw_bk->curr_qos.rx.fltr_template.list_ptr[
                              nw_bk->curr_qos.rx.fltr_template.num_filters]),
			                (byte)filter_id, 
			                DS_PDN_TFT_DOWNLINK, 
			                inst_ptr->type, 
			                flow_context_p,
                   cause_code_p);
        if(result == FALSE)
        {
          DS_ERROR_LOG_1("Error returned for filter No: =d,in Downlink",cnt);
          return FALSE;
        }
        nw_bk->curr_qos.rx.fltr_template.num_filters++;
        break;

      case DS_PDN_TFT_BIDIRECTIONAL:
      case DS_PDN_TFT_PRE_REL_7:
        if(nw_bk->curr_qos.rx.fltr_template.num_filters >= MAX_PACKET_FILTERS)
        {
          DS_ERROR_LOG_0("Excedding UE supported max filters for downlink ");
          return FALSE;
        }
        
        result = ds_pdn_flow_manager_convert_nw_filter_to_app_filter(
                    nw_filter,
	                   &(nw_bk->curr_qos.rx.fltr_template.list_ptr[
                              nw_bk->curr_qos.rx.fltr_template.num_filters]),
			                 (byte)filter_id, 
			                 DS_PDN_TFT_DOWNLINK,
			                 inst_ptr->type,
			                 flow_context_p,
                    cause_code_p);
        if(result == FALSE)
        {
          DS_ERROR_LOG_1("Error returned for filter No: =d,in Downlink",cnt);
          return FALSE;
        }
        
        nw_bk->curr_qos.rx.fltr_template.num_filters++;
        if(nw_bk->curr_qos.tx.fltr_template.num_filters >= MAX_PACKET_FILTERS)
        {
          DS_ERROR_LOG_0("Excedding UE supported max filters for uplink ");
          return FALSE;
        }
        
        result = ds_pdn_flow_manager_convert_nw_filter_to_app_filter(
                       nw_filter,
	                      &(nw_bk->curr_qos.tx.fltr_template.list_ptr[
                           nw_bk->curr_qos.tx.fltr_template.num_filters]),
                    			(byte)filter_id, 
                    			DS_PDN_TFT_UPLINK, 
                    			inst_ptr->type,
                    			flow_context_p,
                       cause_code_p);
        if(result == FALSE)
        {
          DS_ERROR_LOG_1("Error returned for filter No: =d,in Uplink", cnt);
          return FALSE;
        }
        nw_bk->curr_qos.tx.fltr_template.num_filters++;
        break;
        
      default:
        DS_ERROR_LOG_0("unexpected direction received");
    }/* switch */   
  }/* for */
  
  /*-------------------------------------------------------------------
    Update the TX Filter Mask.
  ---------------------------------------------------------------------*/
  if(nw_bk->curr_qos.tx.fltr_template.num_filters > 0)
  {
    if(orig_tx_filter_cnt == 0)
    {
      nw_bk->curr_qos.field_mask |= (qos_spec_field_mask_type)QOS_MASK_TX_FLOW;
      if(mode_info_p == NULL)
      {
        DS_MSG0(MSG_LEGACY_HIGH, "New Filter added on TX, but no data rate");
        // Extract the flow from bearer
      }
    }
    else
    {
      nw_bk->curr_qos.field_mask |= 
                   (qos_spec_field_mask_type)QOS_MODIFY_MASK_TX_FLTR_MODIFY;
    }
  }
  
  /*-------------------------------------------------------------------
    Update the RX Filter Mask.
  ---------------------------------------------------------------------*/ 
  if(nw_bk->curr_qos.rx.fltr_template.num_filters > 0)
  {
    if(orig_rx_filter_cnt == 0)
    {
      nw_bk->curr_qos.field_mask |= (qos_spec_field_mask_type)QOS_MASK_RX_FLOW;
      if(mode_info_p == NULL)
      {
        DS_MSG0(MSG_LEGACY_HIGH, "New Filter added on RX, but no data rate");
        // Extact the flow from Bearer.
      }
    }
    else
    {
      nw_bk->curr_qos.field_mask |=
            (qos_spec_field_mask_type)QOS_MODIFY_MASK_RX_FLTR_MODIFY;
    }
  }
  
  /*-------------------------------------------------------------------
    Update the QOS.
  ---------------------------------------------------------------------*/
  if(mode_info_p != NULL)
  {
    /*---------------------------------------------------------------------
      Convert NW Qos to App Qos
    ---------------------------------------------------------------------*/
    if(ds_bearer_flow_manager_convert_data_rate(bearer_context_p,
                                                &mode_info_p->sdf_qos,
                                                &mode_info_p->neg_qos,
                                                &(nw_bk->curr_qos),
                                                cause_code_p) == FALSE)
    {
      DS_ERROR_LOG_0("Network Qos is not accepted");
      return FALSE;
    }
    
    // Need to check data rate before configuring the field mask.
    if(nw_bk->curr_qos.rx.flow_template.req_flow.field_mask)                        
    {
      if(nw_bk->curr_qos.rx.fltr_template.num_filters > 0)
      {
        nw_bk->curr_qos.field_mask |= ((qos_spec_field_mask_type)
                                            QOS_MASK_RX_FLOW);
      }
    }
    if(nw_bk->curr_qos.tx.flow_template.req_flow.field_mask)                        
    {
      if(nw_bk->curr_qos.tx.fltr_template.num_filters > 0)
      {
        nw_bk->curr_qos.field_mask |= ((qos_spec_field_mask_type)
                                            QOS_MASK_TX_FLOW);
      } 
    } 
  }/* mode_info_p  != NULL*/

  if(ds_pdn_nv_manager_is_rel_12_or_above_supported())
  {
    if((nw_bk->curr_qos.tx.fltr_template.num_filters == 0) && 
       (!bearer_context_p->is_default))
    {
      DS_ERROR_LOG_0("Resultant tft does not have any uplink TX filters");
      *cause_code_p = DS_BEARER_FLOW_INTERNAL_SEMANTIC_ERR_IN_FILTER;  
      return FALSE;
    }
  }

  if(new_flow)
  {
  /*-------------------------------------------------------------------
      Copy the previous bearer Qos to flow.
    ---------------------------------------------------------------------*/
    if(mode_info_p == NULL)
    {
      memscpy(&(nw_bk->curr_qos.rx.flow_template.req_flow),
	             sizeof(ip_flow_type),
              &(bflow_manager_p->rx_flow), 
              sizeof(ip_flow_type));
      memscpy(&(nw_bk->curr_qos.tx.flow_template.req_flow),
   	          sizeof(ip_flow_type),
              &(bflow_manager_p->tx_flow),
              sizeof(ip_flow_type));
    }
    
    /*-----------------------------------------------------------------------
      Clear the filter modify flag.
    -----------------------------------------------------------------------*/
    if(nw_bk->curr_qos.tx.fltr_template.num_filters > 0) 
    {
      nw_bk->curr_qos.field_mask &=~((qos_spec_field_mask_type)
                       (QOS_MODIFY_MASK_TX_FLTR_MODIFY));
      nw_bk->curr_qos.field_mask |= ((qos_spec_field_mask_type)
                                        QOS_MASK_TX_FLOW);
    }
    if(nw_bk->curr_qos.rx.fltr_template.num_filters > 0)
    {
      nw_bk->curr_qos.field_mask &=~((qos_spec_field_mask_type)
                       (QOS_MODIFY_MASK_RX_FLTR_MODIFY));
      nw_bk->curr_qos.field_mask |= ((qos_spec_field_mask_type)
                                            QOS_MASK_RX_FLOW);
    }

    result = ds_flow_create_flow_context(flow_context_p,
                                         &(nw_bk->curr_qos),
                                         pdn_context_p,
                                         inst_ptr->type,
                                         bearer_context_p);
    if(result == FALSE)
    {
      DS_ERROR_LOG_0("create flow failed");
      ds_flow_release_flow_context(flow_context_p);
      *cause_code_p = DS_BEARER_FLOW_INTERNAL_SEMANTIC_ERR_IN_FILTER;
      return FALSE;
    }    

    // Back up Rx and Tx list ptr.
    temp_rx_fltr_list = 
             flow_context_p->qos_info_ptr->rx.fltr_template.list_ptr;
    temp_tx_fltr_list = 
             flow_context_p->qos_info_ptr->tx.fltr_template.list_ptr;

    // Copy the flow spec to flow context.
    memscpy(flow_context_p->qos_info_ptr, sizeof(qos_spec_type),
            &nw_bk->curr_qos, sizeof(qos_spec_type));

    // Restore Rx filter list ptr and copy the Rx filters
    flow_context_p->qos_info_ptr->rx.fltr_template.list_ptr = 
                                                    temp_rx_fltr_list;
    memset(flow_context_p->qos_info_ptr->rx.fltr_template.list_ptr, 0,
    	      sizeof(ip_filter_type)*MAX_PACKET_FILTERS);  
    memscpy(flow_context_p->qos_info_ptr->rx.fltr_template.list_ptr,
  	         sizeof(ip_filter_type)*MAX_PACKET_FILTERS,
  	         nw_bk->curr_qos.rx.fltr_template.list_ptr,
  	         sizeof(ip_filter_type)*MAX_PACKET_FILTERS);

    // Restore Tx filter list ptr and copy the Tx filters
    flow_context_p->qos_info_ptr->tx.fltr_template.list_ptr = 
                                                    temp_tx_fltr_list;
    memset(flow_context_p->qos_info_ptr->tx.fltr_template.list_ptr, 0,
    	      sizeof(ip_filter_type)*MAX_PACKET_FILTERS);  
    memscpy(flow_context_p->qos_info_ptr->tx.fltr_template.list_ptr,
  	         sizeof(ip_filter_type)*MAX_PACKET_FILTERS,
  	         nw_bk->curr_qos.tx.fltr_template.list_ptr,
  	         sizeof(ip_filter_type)*MAX_PACKET_FILTERS);
    
    // Disable flow to avoid data write
    ds_flow_enable_disable_tx(flow_context_p, FALSE);
       
    // Add flow context to bearer flow manager for secondary flows
    if(inst_ptr->flow->default_flow_ptr != flow_context_p)
    {
      if(ds_bearer_flow_manager_add_flow_context_to_flow_manager(
                                                     bearer_context_p, 
                                                     flow_context_p) == FALSE)
      {
        ds_flow_release_flow_context(flow_context_p);
        *cause_code_p = DS_BEARER_FLOW_INTERNAL_INSUFFICIENT_RESOURCES;
        return FALSE;
      }  
    }

    ds_flow_context_activate_ind(flow_context_p);    
  }/* if new flow*/
  else
  {
    /*-------------------------------------------------------------------
      Disable Filtering on the Flow.
    ---------------------------------------------------------------------*/
    fltr_ctrl_param.ds_fwk_inst_ptr = ds_fwk_get_inst_ptr_from_index(
                                                  pdn_context_p->fw_index);
    fltr_ctrl_param.enable = FALSE;
    fltr_ctrl_param.client_id = FLTR_CLIENT_QOS_OUTPUT;
    fltr_ctrl_param.fi_handle = 
    	           flow_context_p->qos_info_ptr->tx.fltr_template.fltr_handle;
    	
    if(ds_ipfltr_control(&fltr_ctrl_param, &err_no) == -1)
    {
      DS_ERROR_LOG_0("flow filtering control failed");
    }
    
    /*-----------------------------------------------------------------------
      Invoke the Flow Modify.
    -----------------------------------------------------------------------*/
    result = ds_flow_modify_flow_context(flow_context_p, &(nw_bk->curr_qos));
    if(result == FALSE)
    {
      DS_ERROR_LOG_0("Modify Flow failed");
      
      fltr_ctrl_param.ds_fwk_inst_ptr = ds_fwk_get_inst_ptr_from_index(
                                                    pdn_context_p->fw_index);
      fltr_ctrl_param.enable = TRUE;
      fltr_ctrl_param.client_id = FLTR_CLIENT_QOS_OUTPUT;
      fltr_ctrl_param.fi_handle = 
                  flow_context_p->qos_info_ptr->tx.fltr_template.fltr_handle;
       
      if(ds_ipfltr_control(&fltr_ctrl_param, &err_no) == -1)
      {
        DS_ERROR_LOG_0("flow filtering control failed");
      }
      
      return FALSE;
    }
  }/* new_flow == FALSE */

  if(new_flow == FALSE)
  {
    flow_context_p->state = DS_FLOW_STATE_PENDING_CONFIRM;
  }
  else if(nw_bk->curr_qos.field_mask & QOS_MASK_TX_FLOW)
  {
    /*-------------------------------------------------------------------
      Enable Filtering on the Flow.
    ---------------------------------------------------------------------*/
    fltr_ctrl_param.ds_fwk_inst_ptr = ds_fwk_get_inst_ptr_from_index(
                                                  pdn_context_p->fw_index);
    fltr_ctrl_param.enable = TRUE;
    fltr_ctrl_param.client_id = FLTR_CLIENT_QOS_OUTPUT;
    fltr_ctrl_param.fi_handle = 
    	           flow_context_p->qos_info_ptr->tx.fltr_template.fltr_handle;
    	
    if(ds_ipfltr_control(&fltr_ctrl_param, &err_no) == -1)
    {
      DS_ERROR_LOG_0("flow filtering control failed");
    }
    
  }
  else
  {
    DS_MSG0(MSG_LEGACY_HIGH, 
            "Network is adding only RX filters, added successfully");
  }
 
  /*-------------------------------------------------------------------
    Update the Identifier and Precedence if there was Flter Id conflict
    on the flow ;also including the case where operator code is create; 
    here then we clear the filter id and precendence for all the initial filters.
  ---------------------------------------------------------------------*/
  if(nw_bk->flow_cntx_fltr_id != 0 ||
     (nw_tft->operation_code == (byte)DS_PDN_TFT_OPCODE_CREATE))
  {
    /*-------------------------------------------------------------------
      Update the filter pointers and filter count to the original cnt
    ---------------------------------------------------------------------*/
    nw_bk->curr_qos.rx.fltr_template.list_ptr  = nw_bk->curr_rx_fltr;
    nw_bk->curr_qos.tx.fltr_template.list_ptr  = nw_bk->curr_tx_fltr;
    /*-------------------------------------------------------------------
      Store the current filter count
    ---------------------------------------------------------------------*/
    torig_rx_filter_cnt = nw_bk->curr_qos.rx.fltr_template.num_filters;
    torig_tx_filter_cnt = nw_bk->curr_qos.tx.fltr_template.num_filters;
    /*-------------------------------------------------------------------
      Retrieve the filter count before modification
    ---------------------------------------------------------------------*/
    nw_bk->curr_qos.rx.fltr_template.num_filters = orig_rx_filter_cnt;
    nw_bk->curr_qos.tx.fltr_template.num_filters = orig_tx_filter_cnt;
  
    /*-------------------------------------------------------------------
      clear Precedence and Filter ID information.
    ---------------------------------------------------------------------*/
    if(ds_pdn_flow_manager_update_filter_id_precedence(
                                              pdn_context_p,
                                              bearer_context_p,
                                              flow_context_p,
                                              nw_bk->flow_cntx_fltr_id,
                                              &(nw_bk->curr_qos),
                                              TRUE) == FALSE)
    {
      DS_ERROR_LOG_0("update filter id and precedence failed");
      return FALSE;
    }
    
    /*-------------------------------------------------------------------
      Update the filter count to current count
    ---------------------------------------------------------------------*/
    nw_bk->curr_qos.rx.fltr_template.num_filters = torig_rx_filter_cnt;
    nw_bk->curr_qos.tx.fltr_template.num_filters = torig_tx_filter_cnt;
    /*-------------------------------------------------------------------
      Update the filter pointer.
    ---------------------------------------------------------------------*/
    nw_bk->curr_qos.rx.fltr_template.list_ptr    = nw_bk->mod_rx_fltr;
    nw_bk->curr_qos.tx.fltr_template.list_ptr    = nw_bk->mod_tx_fltr;  
  } /* nw_bk->flow_cntx_fltr_id != 0*/

  /*-------------------------------------------------------------------
    Set the precedence and filter_identifier.
  ---------------------------------------------------------------------*/
  if(ds_pdn_flow_manager_update_filter_id_precedence(pdn_context_p,
                                                     bearer_context_p,
                                                     flow_context_p,
                                                     0,
                                                     &(nw_bk->curr_qos),
                                                     FALSE) == FALSE)
  {
    DS_ERROR_LOG_0("filter update failed");
    return FALSE;
  }
  
  return TRUE;
} /*ds_pdn_flow_manager_add_filter_to_flow*/

/*===========================================================================
FUNCTION DS_PDN_FLOW_MANAGER_REPLACE_FILTER_IN_FLOW

DESCRIPTION
  Replace Filter in a flow .

PARAMETERS
  pdn_context_p         -  PDN information
  bearer_context_p      -  Bearer information
  flow_context_p        -  Flow context.
  nw_tft                -  TFT information.
  mode_info_p           -  Mode specific Qos.
  nw_bk                 -  network book keeping info.

DEPENDENCIES
  None.

RETURN VALUE
boolean - return TRUE or FALSE
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_flow_manager_replace_filter_in_flow
(
 ds_pdn_mgr_s                                 *pdn_context_p,
 ds_bearer_mgr_s                              *bearer_context_p,
 ds_flow_type                                 *flow_context_p,
 tft_type_T                                   *nw_tft,
 ds_pdn_tft_nw_modify_qos_bk_s                *nw_bk,
 emm_modify_bearer_context_request_ind_s_type *mode_info_p,
 ds_bearer_flow_internal_cause_code_enum_type *cause_code_p
)
{
  ds_pdn_mgr_inst_type_s      *inst_ptr = NULL;
  int                          cnt = 0;
  packet_filter_type_T        *nw_filter=NULL;
  uint16                       filter_id = 0;
  byte                         direction  =0;
  boolean                      result = FALSE;
  ds_pdn_tft_pkt_filter_direction_enum_type 
                               filter_direction; 
  ds_ipfltr_ctrl_param_type    fltr_ctrl_param = {0};
  int16                        err_no;
  uint8                        orig_rx_filter_cnt=0, orig_tx_filter_cnt=0;
  uint8                        torig_rx_filter_cnt=0, torig_tx_filter_cnt=0;
  boolean                      update_filter_info = FALSE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  *cause_code_p = DS_BEARER_FLOW_INTERNAL_INSUFFICIENT_RESOURCES;  

  /*-------------------------------------------------------------------------
    Configure the QOS flow spec
  -------------------------------------------------------------------------*/
  if(ds_pdn_tft_configure_nw_bk_flow_spec(nw_bk) == FALSE)
  {
    DS_ERROR_LOG_0("error configuring nw book-keeping");
    return FALSE;
  }

  memscpy(&nw_bk->curr_qos, sizeof(qos_spec_type),
          flow_context_p->qos_info_ptr, sizeof(qos_spec_type));

  nw_bk->curr_qos.rx.fltr_template.list_ptr = nw_bk->curr_rx_fltr;  
  memscpy(nw_bk->curr_qos.rx.fltr_template.list_ptr,
          sizeof(ip_filter_type)*MAX_PACKET_FILTERS,
          flow_context_p->qos_info_ptr->rx.fltr_template.list_ptr,          
          sizeof(ip_filter_type)*MAX_PACKET_FILTERS);
  
  nw_bk->curr_qos.tx.fltr_template.list_ptr = nw_bk->curr_tx_fltr;
  memscpy(nw_bk->curr_qos.tx.fltr_template.list_ptr,
          sizeof(ip_filter_type)*MAX_PACKET_FILTERS,
          flow_context_p->qos_info_ptr->tx.fltr_template.list_ptr,
          sizeof(ip_filter_type)*MAX_PACKET_FILTERS);

  /*-----------------------------------------------------------------------
    Store the orginal filter count before modification.
  -----------------------------------------------------------------------*/
  torig_rx_filter_cnt = nw_bk->curr_qos.rx.fltr_template.num_filters;
  torig_tx_filter_cnt = nw_bk->curr_qos.tx.fltr_template.num_filters;
  
  /*-----------------------------------------------------------------------
  According to spec 24.008:Section: 6.1.3.3.3
  1) TFT operation = "Create a new TFT" when there is already an existing TFT 
  for the PDP context.The receiver shall further process the new activation 
  request and, if it was processed successfully, delete the old TFT.
  -----------------------------------------------------------------------*/
  if(nw_tft->operation_code == DS_PDN_TFT_OPCODE_CREATE)
  {
    /*-------------------------------------------------------------------
      Remove the flow filters.
    ---------------------------------------------------------------------*/
    DS_MSG3(MSG_LEGACY_HIGH,
            "replace_filter_in_flow op-code %d, flow cntxfltr id "
            "mask =%d non_match_fltr_id= %d", 
            nw_tft->operation_code,
	           flow_context_p->fi_identifier,
  	         nw_bk->non_match_fltr_id);
    
    nw_bk->flow_cntx_fltr_id = 0;
    update_filter_info = TRUE;
  }
  else if((nw_bk->flow_cntx_fltr_id != 0) )
  {
    /*-------------------------------------------------------------------
     flow_cntx_fltr_id will be set only if there is any filter already installed
     with same filter ID, Remove the flow filters.
    ---------------------------------------------------------------------*/
    DS_MSG3(MSG_LEGACY_HIGH,
            "replace_filter_in_flow op-code:%d,flow_cntx_fltr_id =%d"
            " non_match_fltr_id= %d",
            nw_tft->operation_code,
            nw_bk->flow_cntx_fltr_id,
            nw_bk->non_match_fltr_id);

    if(ds_flow_delete_filter_from_flow(flow_context_p,
                                       nw_bk->flow_cntx_fltr_id,
                                       &(nw_bk->curr_qos),
                                       &(nw_bk->mod_qos)) == FALSE)
    {
      DS_ERROR_LOG_0("error deleting filter from flow");
      return FALSE;
    }
    update_filter_info = TRUE;
  }/* flow_filter_id != 0*/
  
  if(update_filter_info == TRUE)
  {
    /*-------------------------------------------------------------------
      Update the filter pointers and number of filters.
    ---------------------------------------------------------------------*/
    nw_bk->curr_qos.rx.fltr_template.list_ptr = nw_bk->mod_rx_fltr;
    nw_bk->curr_qos.tx.fltr_template.list_ptr = nw_bk->mod_tx_fltr;
    orig_rx_filter_cnt = nw_bk->curr_qos.rx.fltr_template.num_filters;
    orig_tx_filter_cnt = nw_bk->curr_qos.tx.fltr_template.num_filters;
    nw_bk->curr_qos.rx.fltr_template.num_filters = 
                                nw_bk->mod_qos.rx.fltr_template.num_filters;
    nw_bk->curr_qos.tx.fltr_template.num_filters = 
                                nw_bk->mod_qos.tx.fltr_template.num_filters;
  }/* flow_filter_id != 0*/

  inst_ptr = flow_context_p->inst_ptr;
  if(!inst_ptr)
  {
    return FALSE;
  }
  
  /*-----------------------------------------------------------------------
    Add the filters requested by NW.
  -----------------------------------------------------------------------*/          
  for(cnt=0; cnt< nw_tft->num_filters; cnt++)
  {
    nw_filter = &(nw_tft->filter_list[cnt]);
    filter_id = nw_filter->identifier & 0xF;
    direction = ((nw_filter->identifier & 0x30)>>4);
    filter_direction =(ds_pdn_tft_pkt_filter_direction_enum_type)direction; 
    switch(filter_direction)
    {
      case DS_PDN_TFT_UPLINK:
        if(nw_bk->curr_qos.tx.fltr_template.num_filters >= MAX_PACKET_FILTERS)
        {
          DS_ERROR_LOG_0("Excedding UE supported max filters for uplink ");
          return FALSE;
        }
        result = ds_pdn_flow_manager_convert_nw_filter_to_app_filter(
                    nw_filter,
                    &(nw_bk->curr_qos.tx.fltr_template.list_ptr[
                               nw_bk->curr_qos.tx.fltr_template.num_filters]),
                    (byte)filter_id, 
                    filter_direction, 
                    inst_ptr->type, 
                    flow_context_p,
                    cause_code_p);
        if(result == FALSE)
        {
          DS_ERROR_LOG_1("Error returned for filter No: =d,in Uplink", cnt);
          return FALSE;
        }
        nw_bk->curr_qos.tx.fltr_template.num_filters++;
        break;

      case DS_PDN_TFT_DOWNLINK:
        if(nw_bk->curr_qos.rx.fltr_template.num_filters >= MAX_PACKET_FILTERS)
        {
          DS_ERROR_LOG_0("Excedding UE supported max filters for downlink");
          return FALSE;
        }
        result = ds_pdn_flow_manager_convert_nw_filter_to_app_filter(
                       nw_filter,
                   	   &(nw_bk->curr_qos.rx.fltr_template.list_ptr[
                            nw_bk->curr_qos.rx.fltr_template.num_filters]),
                 	     (byte)filter_id, 
                 	     DS_PDN_TFT_DOWNLINK,
                 	     inst_ptr->type, 
                 	     flow_context_p,
                       cause_code_p);
        if(result == FALSE)
        {
          DS_ERROR_LOG_1("Error returned for filter No: =d,in Downlink",cnt);
          return FALSE;
        }
        nw_bk->curr_qos.rx.fltr_template.num_filters++;
        break;

      case DS_PDN_TFT_BIDIRECTIONAL:
      case DS_PDN_TFT_PRE_REL_7:
        if(nw_bk->curr_qos.rx.fltr_template.num_filters >= MAX_PACKET_FILTERS)
        {
          DS_ERROR_LOG_0("Excedding UE supported max filters for downlink ");
          return FALSE;
        }
        result = ds_pdn_flow_manager_convert_nw_filter_to_app_filter(
                        nw_filter,
               	        &(nw_bk->curr_qos.rx.fltr_template.list_ptr[
                            nw_bk->curr_qos.rx.fltr_template.num_filters]),
			                     (byte)filter_id, 
			                     DS_PDN_TFT_DOWNLINK, 
			                     inst_ptr->type,
			                     flow_context_p,
                        cause_code_p);
        if(result == FALSE)
        {
          DS_ERROR_LOG_1("Error returned for filter No: =d,in Downlink", cnt);
          return FALSE;
        }
        nw_bk->curr_qos.rx.fltr_template.num_filters++;

        if(nw_bk->curr_qos.tx.fltr_template.num_filters >= MAX_PACKET_FILTERS)
        { 
          DS_ERROR_LOG_0("Excedding UE supported max filters for uplink ");
          return FALSE;
        }
        
        result = ds_pdn_flow_manager_convert_nw_filter_to_app_filter(
                       nw_filter,
	                      &(nw_bk->curr_qos.tx.fltr_template.list_ptr[
                             nw_bk->curr_qos.tx.fltr_template.num_filters]),
			                    (byte)filter_id, 
			                    DS_PDN_TFT_UPLINK,
			                    inst_ptr->type,
			                    flow_context_p,
                       cause_code_p);
        if(result == FALSE)
        {
          DS_ERROR_LOG_1("Error returned for filter No: =d,in Uplink", cnt);
          return FALSE;
        }
        nw_bk->curr_qos.tx.fltr_template.num_filters++;
        break;
        
      default:
        DS_ERROR_LOG_0("unexpected direction received");
    }/* switch */   
  }/* for */

  /*-------------------------------------------------------------------
    Update the TX Filter Mask.
  ---------------------------------------------------------------------*/
  if(nw_bk->curr_qos.tx.fltr_template.num_filters > 0)
  {
    if(torig_tx_filter_cnt == 0)
    {
      nw_bk->curr_qos.field_mask |=
               (qos_spec_field_mask_type)QOS_MASK_TX_FLOW;
      if(mode_info_p == NULL)
      {
        DS_MSG0(MSG_LEGACY_HIGH, "Filter Add/Rep on TX, but no data rate");
        // Extract the flow from bearer
      }
    }
    else
    {
      nw_bk->curr_qos.field_mask |=
            (qos_spec_field_mask_type)QOS_MODIFY_MASK_TX_FLTR_MODIFY;
    }
  }
  
  /*-------------------------------------------------------------------
    Update the RX Filter Mask.
  ---------------------------------------------------------------------*/ 
  if(nw_bk->curr_qos.rx.fltr_template.num_filters > 0)
  {
    if(torig_rx_filter_cnt == 0)
    {
      nw_bk->curr_qos.field_mask |=
                   (qos_spec_field_mask_type)QOS_MASK_RX_FLOW;
      if(mode_info_p == NULL)
      {
        DS_MSG0(MSG_LEGACY_HIGH, "Filter add/Rep on RX, but no data rate");
        // Extact the flow from Bearer.
      }
    }
    else
    {
      nw_bk->curr_qos.field_mask |=
            (qos_spec_field_mask_type)QOS_MODIFY_MASK_RX_FLTR_MODIFY;
    }
  }
  /*---------------------------------------------------------------------
    Convert NW Qos to App Qos
  ---------------------------------------------------------------------*/
  if(mode_info_p != NULL)
  {
    if(ds_bearer_flow_manager_convert_data_rate(bearer_context_p,
                                                &mode_info_p->sdf_qos,
                                                &mode_info_p->neg_qos,
                                                &nw_bk->curr_qos,
                                                cause_code_p) == FALSE)
    {
      return FALSE;
    }    
    //  Need to check data rate before configuring the field mask.
    
    if(nw_bk->curr_qos.rx.flow_template.req_flow.field_mask)                        
    {
      if(nw_bk->curr_qos.rx.fltr_template.num_filters > 0)
      {
        nw_bk->curr_qos.field_mask |= ((qos_spec_field_mask_type)
                                            QOS_MASK_RX_FLOW);
      }
    }
    if(nw_bk->curr_qos.tx.flow_template.req_flow.field_mask)                        
    {
      if(nw_bk->curr_qos.tx.fltr_template.num_filters > 0)
      {	  
        nw_bk->curr_qos.field_mask |= ((qos_spec_field_mask_type)
                                            QOS_MASK_TX_FLOW);
      } 
    } 
  }/* mode_info_p != NULL */
  /*-----------------------------------------------------------------------
    If deletion of filter has made num_filters 0 in some direction. 
    We need to Update IP_FLOW_MASK_NONE so that framework deletes all filters.
  -----------------------------------------------------------------------*/
  if( (torig_rx_filter_cnt > 0) &&
      (nw_bk->curr_qos.rx.fltr_template.num_filters == 0))
  {
    DS_MSG1(MSG_LEGACY_HIGH,
            "Deleted all (%d) filters in RX direction", torig_rx_filter_cnt);
    nw_bk->curr_qos.field_mask |= QOS_MASK_RX_FLOW;
    nw_bk->curr_qos.rx.flow_template.req_flow.field_mask = (ipflow_field_mask_type)IPFLOW_MASK_NONE;
  }
  
  if( (torig_tx_filter_cnt > 0) &&
      (nw_bk->curr_qos.tx.fltr_template.num_filters == 0))
  {
    DS_MSG1(MSG_LEGACY_HIGH,
            "Deleted all (%d) filters in TX direction", torig_tx_filter_cnt);
    nw_bk->curr_qos.field_mask |= QOS_MASK_TX_FLOW;
    nw_bk->curr_qos.tx.flow_template.req_flow.field_mask = (ipflow_field_mask_type)IPFLOW_MASK_NONE;
  }

  if(ds_pdn_nv_manager_is_rel_12_or_above_supported())
  {
    if((nw_bk->curr_qos.tx.fltr_template.num_filters == 0) && 
       (!bearer_context_p->is_default))
    {
      DS_ERROR_LOG_0("Resultant tft does not have any uplink TX filters");
      *cause_code_p = DS_BEARER_FLOW_INTERNAL_SEMANTIC_ERR_IN_FILTER;  
      return FALSE;
      //"modify Flow failed");
    }
  }
    
  /*-------------------------------------------------------------------
    Disable Filtering on the Flow.
  ---------------------------------------------------------------------*/
  fltr_ctrl_param.ds_fwk_inst_ptr = ds_fwk_get_inst_ptr_from_index(
                                                pdn_context_p->fw_index);
  fltr_ctrl_param.enable = FALSE;
  fltr_ctrl_param.client_id = FLTR_CLIENT_QOS_OUTPUT;
  fltr_ctrl_param.fi_handle = 
              flow_context_p->qos_info_ptr->tx.fltr_template.fltr_handle;
   
  if(ds_ipfltr_control(&fltr_ctrl_param, &err_no) == -1)
  {
    DS_ERROR_LOG_0("flow filtering control failed");
  }
  

  /*-----------------------------------------------------------------------
    Invoke the Flow Modify.
  -----------------------------------------------------------------------*/
  result = ds_flow_modify_flow_context(flow_context_p, &(nw_bk->curr_qos));
  if(result == FALSE)
  {
    DS_ERROR_LOG_0("Modify Flow failed");
    /*-------------------------------------------------------------------
      Enable Filtering on the Flow.
    ---------------------------------------------------------------------*/
    fltr_ctrl_param.ds_fwk_inst_ptr = ds_fwk_get_inst_ptr_from_index(
                                                  pdn_context_p->fw_index);
    fltr_ctrl_param.enable = TRUE;
    fltr_ctrl_param.client_id = FLTR_CLIENT_QOS_OUTPUT;
    fltr_ctrl_param.fi_handle = 
    	           flow_context_p->qos_info_ptr->tx.fltr_template.fltr_handle;
    	
    if(ds_ipfltr_control(&fltr_ctrl_param, &err_no) == -1)
    {
      DS_ERROR_LOG_0("flow filtering control failed");
    }
   
    return FALSE;
  }

  if((nw_bk->flow_cntx_fltr_id != 0) ||
     (nw_tft->operation_code == (byte)DS_PDN_TFT_OPCODE_CREATE))
  {
    /*-------------------------------------------------------------------
      Update the filter pointers and filter count to the original cnt
    ---------------------------------------------------------------------*/
    nw_bk->curr_qos.rx.fltr_template.list_ptr  = nw_bk->curr_rx_fltr;
    nw_bk->curr_qos.tx.fltr_template.list_ptr  = nw_bk->curr_tx_fltr;
    /*-------------------------------------------------------------------
      Store the current filter count
    ---------------------------------------------------------------------*/
    torig_rx_filter_cnt = nw_bk->curr_qos.rx.fltr_template.num_filters;
    torig_tx_filter_cnt = nw_bk->curr_qos.tx.fltr_template.num_filters;
    /*-------------------------------------------------------------------
      Retrieve the filter count before modification
    ---------------------------------------------------------------------*/
    nw_bk->curr_qos.rx.fltr_template.num_filters = orig_rx_filter_cnt;
    nw_bk->curr_qos.tx.fltr_template.num_filters = orig_tx_filter_cnt;
    DS_MSG3(MSG_LEGACY_HIGH,
            "Store the current Tx filter count: %d, "
            "Tx filter count before modification: %d, filter_id %d", 
  	         torig_tx_filter_cnt, orig_tx_filter_cnt, nw_bk->flow_cntx_fltr_id);
    
    /*-------------------------------------------------------------------
      clear Precedence and Filter ID information.
    ---------------------------------------------------------------------*/
    if(ds_pdn_flow_manager_update_filter_id_precedence(
                                              pdn_context_p,
                                              bearer_context_p,
                                              flow_context_p,
                                              nw_bk->flow_cntx_fltr_id,
                                              &(nw_bk->curr_qos),
                                              TRUE) == FALSE)
    {
      DS_ERROR_LOG_0("update filter id and precedence failed");
      return FALSE;
    }
    
    /*-------------------------------------------------------------------
      Update the filter count to current count
    ---------------------------------------------------------------------*/
    nw_bk->curr_qos.rx.fltr_template.num_filters = torig_rx_filter_cnt;
    nw_bk->curr_qos.tx.fltr_template.num_filters = torig_tx_filter_cnt;
    /*-------------------------------------------------------------------
      Update the filter pointer.
    ---------------------------------------------------------------------*/
    nw_bk->curr_qos.rx.fltr_template.list_ptr    = nw_bk->mod_rx_fltr;
    nw_bk->curr_qos.tx.fltr_template.list_ptr    = nw_bk->mod_tx_fltr;  
  } /* nw_bk->flow_cntx_fltr_id != 0*/

  if(ds_pdn_flow_manager_update_filter_id_precedence(pdn_context_p,
                                                     bearer_context_p,
                                                     flow_context_p,
                                                     0,
                                                     &(nw_bk->curr_qos), 
                                                     FALSE) == FALSE)
  {
    DS_ERROR_LOG_0("update filter id and precedence failed");
    return FALSE;
  }

  flow_context_p->state = DS_FLOW_STATE_PENDING_CONFIRM;

  return TRUE;
} /* ds_pdn_flow_manager_replace_filter_in_flow */

/*===========================================================================
FUNCTION DS_PDN_FLOW_MANAGER_HANDLE_FLTR_CONFLICT

DESCRIPTION
  Handles the fltr precedence and identifier conflicts.

PARAMETERS
  pdn_context_p         -  PDN information
  bearer_context_p      -  Bearer information
  flow_context_p        -  Flow context
  nw_bk                 -  Network Mod book keeping info
 

DEPENDENCIES
  None.

RETURN VALUE
boolean - return TRUE or FALSE
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_flow_manager_handle_fltr_conflict
(
 ds_pdn_mgr_s                        *pdn_context_p,
 ds_bearer_mgr_s                     *bearer_context_p,
 ds_flow_type                        *flow_context_p,
 ds_pdn_tft_nw_modify_qos_bk_s       *nw_bk       
)
{  
  ds_bearer_flow_manager_s           *bflow_manager_p = NULL;
  ds_flow_type                       *flow_cntx = NULL;
  ds_command_type                    *cmd_ptr = NULL;
  ds_pdn_flow_manager_cmd_info_type  *cmd_info_ptr = NULL;
  ds_ipfltr_ctrl_param_type           fltr_ctrl_param = {0};
  int16                               err_no;
  ds_bearer_flow_manager_list_type   *bflow_list_p;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

 
  if(nw_bk->bearer_fltr_id != 0)
  {
    /*-------------------------------------------------------------------
      Resolve the filter ID conflict on the bearer.
    ---------------------------------------------------------------------*/
    bflow_manager_p = ds_bearer_flow_manager_find_entry(bearer_context_p);
    if(!bflow_manager_p)
    {
      return FALSE;
    }
    
    /*---------------------------------------------------------------------
      Loop thu all the flows on the bearer to find conflict
    -----------------------------------------------------------------------*/
    bflow_list_p = list_peek_front(&(bflow_manager_p->flow_context));
    while(bflow_list_p && bflow_list_p->flow_context_p)
    {
      flow_cntx = bflow_list_p->flow_context_p;
      if(flow_cntx != flow_context_p)
      {
        /*---------------------------------------------------------------------
          Verify if the flow has any of the filter ID conflicts
        -----------------------------------------------------------------------*/
        if((flow_cntx->fi_identifier & nw_bk->bearer_fltr_id) > 0)
        {
          DS_MSG1(MSG_LEGACY_HIGH, 
                  "Flow context 0x%x has Fi-ID conflict", flow_cntx);
       /*-----------------------------------------------------------------
            Post a command.
          -----------------------------------------------------------------*/
          cmd_ptr = ds_allocate_cmd_buf(
                                     sizeof(ds_pdn_flow_manager_cmd_info_type));
          if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
          {
            DS_ASSERT(0);
            return FALSE;
          }
    
          cmd_ptr->hdr.cmd_id = DS_CMD_PDN_FLOW_FLTR_ID_CONFLICT;
          cmd_info_ptr = (ds_pdn_flow_manager_cmd_info_type*)
                                                        cmd_ptr->cmd_payload_ptr;
    
          cmd_info_ptr->flow_context_p = flow_cntx;
          cmd_info_ptr->fltr_id_conflict = nw_bk->bearer_fltr_id;
    
          ds_put_cmd(cmd_ptr);
    
         /*-----------------------------------------------------------------
            Disable filtering on the flow.
          -----------------------------------------------------------------*/
          fltr_ctrl_param.ds_fwk_inst_ptr = ds_fwk_get_inst_ptr_from_index(
                                                        pdn_context_p->fw_index);
          fltr_ctrl_param.enable = FALSE;
          fltr_ctrl_param.client_id = FLTR_CLIENT_QOS_OUTPUT;
          fltr_ctrl_param.fi_handle = 
                      flow_context_p->qos_info_ptr->tx.fltr_template.fltr_handle;
           
          if(ds_ipfltr_control(&fltr_ctrl_param, &err_no) == -1)
          {
            DS_ERROR_LOG_0("flow filtering control failed");
          }
        }/* if flow_context has a conflict */
      }
      bflow_list_p = list_peek_next(&(bflow_manager_p->flow_context),
                                    &(bflow_list_p->bflow_link));
    }/* while flow_cntx != NULL */
  }

  if(nw_bk->conflict_bearer_cnt != 0)
  {
    ds_pdn_flow_manager_handle_fltr_preced_conflict(pdn_context_p,
                                                    bearer_context_p,
                                                    nw_bk);   
  }
  
  return TRUE;
}/* ds_pdn_flow_manager_handle_fltr_conflict*/

/*===========================================================================
FUNCTION DS_PDN_FLOW_MANAGER_ADD_REPLACE_FILTER_TO_TFT_HELPER

DESCRIPTION
  Add filters to existing TFT.

PARAMETERS
  pdn_context_p         -  PDN information
  bearer_context_p      -  Bearer information
  flow_context_p        -  Flow context information
  nw_tft                -  TFT information
  nw_bk                 -  Network book keeping info.
  mode_info_p           -  Mode specific Qos.
  cause_code_p          -  Cause code NW TFT is not accurate

DEPENDENCIES
  None.

RETURN VALUE
boolean - return TRUE or FALSE
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_flow_manager_add_replace_filter_to_tft_helper
(
  ds_pdn_mgr_s                                 *pdn_context_p,
  ds_bearer_mgr_s                              *bearer_context_p,
  ds_flow_type                                 *flow_context_p,
  tft_type_T                                   *nw_tft,
  ds_pdn_tft_nw_modify_qos_bk_s                *nw_bk,
  emm_modify_bearer_context_request_ind_s_type *mode_info_p,
  ds_bearer_flow_internal_cause_code_enum_type *cause_code_p
)
{
  /*-------------------------------------------------------------------
    Check if the network fltr has any fltr ID or fltr preced conflicts.
  -------------------------------------------------------------------*/
  if(ds_pdn_flow_manager_detect_conflict_in_tft(pdn_context_p,
                                                bearer_context_p,
                                                flow_context_p,
                                                nw_tft,
                                                nw_bk,
                                                cause_code_p,
                                                FALSE) == FALSE)
  {
    DS_ERROR_LOG_0("Unresolved Conflict in TFT");
    return FALSE;
  }

  if(nw_bk->is_add == TRUE)
  {
    /*-------------------------------------------------------------------
      Add the filters to this flow.
    -------------------------------------------------------------------*/        
    if(ds_pdn_flow_manager_add_filter_to_flow(
                                  pdn_context_p,
                                  bearer_context_p,
                                  flow_context_p,
                                  nw_tft,
                                  nw_bk,
                                  mode_info_p,
                                  nw_bk->new_flow,
                                  cause_code_p) != TRUE)
    { 
      return FALSE;
    }
  }/* is_add == TRUE */
  else
  {
    /*-------------------------------------------------------------------
      Replace the filters to this flow.
    -------------------------------------------------------------------*/        
    if(ds_pdn_flow_manager_replace_filter_in_flow(
                                  pdn_context_p,
                                  bearer_context_p,
                                  flow_context_p,
                                  nw_tft,
                                  nw_bk,
                                  mode_info_p,
                                  cause_code_p)!= TRUE)
    { 
      return FALSE;
    }
  }/* is_add == FALSE*/
  
  /*-------------------------------------------------------------------
     Resolve any filter conflict.
  ---------------------------------------------------------------------*/
  if(ds_pdn_flow_manager_handle_fltr_conflict(pdn_context_p,
                                              bearer_context_p,
                                              flow_context_p,
                                              nw_bk) == FALSE)
  {
    DS_ERROR_LOG_0("unable to handle filter conflict");
    return FALSE;
  }
  return TRUE;
}/* ds_pdn_flow_manager_add_replace_filter_to_tft_helper */

/*===========================================================================
FUNCTION DS_PDN_FLOW_MANAGER_FREE_NW_FLOW_CONTEXT

DESCRIPTION
  Utility function used to free the nw flow contexts associated with a given
  bearer. Also cleans up the filter ids and precedences in the flow managers.

PARAMETERS
  bearer_context_p     (in) -  Bearer context info. 
 
DEPENDENCIES
  None.

RETURN VALUE
 TRUE - Success
 FALSE - Failure.
  
SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_pdn_flow_manager_free_nw_flow_context
(
  ds_bearer_mgr_s *bearer_context_p
)
{
  ds_flow_type             *flow_context_p = NULL;
  boolean                   result = FALSE;

  do
  {
    //Find the IPV4 flow context and release it.
    flow_context_p = ds_bearer_flow_mgr_find_flow_context(
                                   bearer_context_p,DS_FLOW_V4_FLOW_TYPE);
    if(flow_context_p)
    {            
      result = ds_pdn_flow_manager_update_filter_id_precedence(
                        bearer_context_p->pdn_mgr_ptr,
                        bearer_context_p,
                        flow_context_p,
                        0,
                        flow_context_p->qos_info_ptr,
                        TRUE);
      if (FALSE == result)
      {
        break;
      }
  
      ds_flow_go_null_ind(flow_context_p);
    }
  
    //Find the IPV6 flow context and release it.
    flow_context_p = ds_bearer_flow_mgr_find_flow_context(
                                   bearer_context_p,DS_FLOW_V6_FLOW_TYPE);
    if(flow_context_p)
    {            
      result = ds_pdn_flow_manager_update_filter_id_precedence(
                        bearer_context_p->pdn_mgr_ptr,
                        bearer_context_p,
                        flow_context_p,
                        0,
                        flow_context_p->qos_info_ptr,
                        TRUE);
      if (FALSE == result)
      {
        break;
      }
  
      ds_flow_go_null_ind(flow_context_p);
    }  

    result = TRUE;  
  } while(0);

  return result;
}/* ds_pdn_flow_manager_free_nw_flow_context */

/*===========================================================================
FUNCTION DS_PDN_FLOW_MANAGER_ADD_FILTER_TO_TFT

DESCRIPTION
  Add filters to existing TFT.

PARAMETERS
  pdn_context_p         -  PDN information.
  bearer_context_p      -  Bearer information.
  flow_context_p        -  Flow context information.
  nw_tft                -  TFT information.
  mode_info_p           -  Mode specific Qos.
  cause_code_p          -  Cause code NW TFT is not accurate.

DEPENDENCIES
  None.

RETURN VALUE
boolean - return TRUE or FALSE
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_flow_manager_add_filter_to_tft
(
  ds_pdn_mgr_s                                 *pdn_context_p,
  ds_bearer_mgr_s                              *bearer_context_p,
  tft_type_T                                   *nw_tft,
  emm_modify_bearer_context_request_ind_s_type *mode_info_p,
  ds_bearer_flow_internal_cause_code_enum_type *cause_code_p,
  boolean                                       allow_flow_create
)
{
  ds_flow_type                    *flow_cntxv4 = NULL;
  ds_flow_type                    *flow_cntxv6 = NULL; 
  ds_pdn_tft_nw_modify_qos_bk_s   *nw_bk;
  ds_pdn_mgr_inst_type_s          *inst_ptr = NULL;
  ds_profile_pdp_type_e            pdp_type;
  boolean                          return_value = FALSE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
   
  /*-----------------------------------------------------------------------
    Verify Input params
  -----------------------------------------------------------------------*/
  if( !ds_pdn_mgr_validate_pdn_ptr(pdn_context_p) ||
      !ds_bearer_mgr_validate_bearer_ptr(bearer_context_p) ||
      (nw_tft == NULL) || (cause_code_p== NULL) )
  {
    if(cause_code_p != NULL)
    {
      *cause_code_p = DS_BEARER_FLOW_INTERNAL_INSUFFICIENT_RESOURCES;
    }
    return FALSE;
  }
  
  /*-----------------------------------------------------------------------
     Check for TFT Semantic Errors
  -----------------------------------------------------------------------*/
  if(nw_tft->valid)
  {
    if(ds_pdn_tft_check_mod_fltr_semantic_error(nw_tft) == TRUE)
    {
      *cause_code_p = DS_BEARER_FLOW_INTERNAL_SYNTACTICAL_ERR_IN_TFT;
      return FALSE;
    }
    else if(ds_pdn_tft_check_for_repeated_precedence(nw_tft) == TRUE)
    {
      *cause_code_p = DS_BEARER_FLOW_INTERNAL_SEMANTIC_ERR_IN_FILTER;
      return FALSE;
    }
  }
  /*-----------------------------------------------------------------------
     Allocate Heap Memory
  -----------------------------------------------------------------------*/
  DS_SYSTEM_HEAP_MEM_ALLOC(nw_bk,
                           sizeof(ds_pdn_tft_nw_modify_qos_bk_s),
                           ds_pdn_tft_nw_modify_qos_bk_s*);
  if(nw_bk == NULL)
  {
    *cause_code_p = DS_BEARER_FLOW_INTERNAL_INSUFFICIENT_RESOURCES;
    return FALSE;
  }
  memset(nw_bk, 0,sizeof(ds_pdn_tft_nw_modify_qos_bk_s));

  pdp_type = ds_apn_table_get_pdn_pdp_type(pdn_context_p);
  
  if(pdp_type == DS_PROFILE_PDP_IPV4)
  {
    inst_ptr = pdn_context_p->inst_ptr[DS_PDN_INSTANCE_IPV4];
    if(!inst_ptr)
    {
      *cause_code_p = DS_BEARER_FLOW_INTERNAL_INSUFFICIENT_RESOURCES;
      DS_SYSTEM_HEAP_MEM_FREE(nw_bk);
      return FALSE;
    }
    
    flow_cntxv4 = ds_bearer_flow_mgr_find_flow_context(bearer_context_p,
                                                       DS_FLOW_V4_FLOW_TYPE);
    if(flow_cntxv4 == NULL)
    {
      if(!allow_flow_create)
      {
        // flow create is not allowed, this is a BCM violation, nw tries to 
        // install tft on a UE init flow
        *cause_code_p = DS_BEARER_FLOW_INTERNAL_BCM_VIOLATION;
        DS_SYSTEM_HEAP_MEM_FREE(nw_bk);
        return FALSE;
      }
      /* 
       There is a race condition here, the flow could be getting
       deleted on the PDN, while this request comes thru. In this case
       we bearer flow manager will have 0 flows.
       One other race condition is the between when UE wants to delete
       the bearer, and NW wants to add to this.
      */
      /*-----------------------------------------------------------------
         Retrieve an empty flow context.
      -----------------------------------------------------------------*/
      flow_cntxv4 = ds_flow_allocate_context(inst_ptr);
      if(flow_cntxv4 == NULL)
      {
        *cause_code_p = DS_BEARER_FLOW_INTERNAL_INSUFFICIENT_RESOURCES;
        DS_SYSTEM_HEAP_MEM_FREE(nw_bk);
        return FALSE;
      }
      else
      {         
        ds_flow_configure_flow_context(flow_cntxv4, bearer_context_p);
        nw_bk->new_flow = TRUE;
      }/* else */
      
      nw_bk->is_add = TRUE;   
    }/*flow_cntxv4 == NULL*/
    else if(bearer_context_p->is_default == TRUE &&
    	       flow_cntxv4->qos_info_ptr->field_mask == QOS_MASK_INVALID)
    {
      nw_bk->new_flow = TRUE;
      nw_bk->is_add = TRUE; 
    }
    else if( nw_tft->operation_code == DS_PDN_TFT_OPCODE_ADD)
    {
      nw_bk->is_add = TRUE; 
    }
    else 
    {
      nw_bk->is_add = FALSE;
    }

    /*-------------------------------------------------------------------
      Invoke the add filter to tft helper.
    -------------------------------------------------------------------*/
    return_value = ds_pdn_flow_manager_add_replace_filter_to_tft_helper(
                                                pdn_context_p,
                                                bearer_context_p,
                                                flow_cntxv4,
                                                nw_tft,
                                                nw_bk,
                                                mode_info_p,
                                                cause_code_p);
    //If this is a new flow then we created a new TFT.
    //For the default bearer; we need to treat this differently by disallowing
    //data on the default flow.
    if ((return_value == TRUE) && (TRUE == bearer_context_p->is_default))
      {
        int i;
        // Packets not matching the installed filters will be dropped.
        for (i = 0; i < DS_PDN_INSTANCE_MAX; i++)
        {
          ds_pdn_mgr_inst_type_s *inst_p = pdn_context_p->inst_ptr[i];

          if(inst_p != NULL &&
             inst_p->flow != NULL)
          {      
            ds_flow_set_capability(inst_p->flow->default_flow_ptr,
                                   DS_FLOW_CAPABILITY_DATA_DISALLOWED);
          }
        }
      }

    DS_SYSTEM_HEAP_MEM_FREE(nw_bk);
    return return_value;
  }
  else if(pdp_type == DS_PROFILE_PDP_IPV6)
  {
    inst_ptr = pdn_context_p->inst_ptr[DS_PDN_INSTANCE_IPV6];
    if(!inst_ptr)
    {
      *cause_code_p = DS_BEARER_FLOW_INTERNAL_INSUFFICIENT_RESOURCES;
      DS_SYSTEM_HEAP_MEM_FREE(nw_bk);
      return FALSE;
    }
    
    flow_cntxv6 = ds_bearer_flow_mgr_find_flow_context(bearer_context_p,
                                                       DS_FLOW_V6_FLOW_TYPE);
    if(flow_cntxv6 == NULL)
    {
      if(!allow_flow_create)
      {
        // flow create is not allowed, this is a BCM violation, nw tries to 
        // install tft on a UE init flow
        *cause_code_p = DS_BEARER_FLOW_INTERNAL_BCM_VIOLATION;
        DS_SYSTEM_HEAP_MEM_FREE(nw_bk);
        return FALSE;
      }
      /* 
       There is a race condition here, the flow could be getting
       deleted on the PDN, while this request comes thru. In this case
       we bearer flow manager will have 0 flows.
       One other race condition is the between when UE wants to delete
       the bearer, and NW wants to add to this.
      */
      /*-----------------------------------------------------------------
         Retrieve an empty flow context.
      -----------------------------------------------------------------*/
      flow_cntxv6 = ds_flow_allocate_context(inst_ptr);
      if(flow_cntxv6 == NULL)
      {
        *cause_code_p = DS_BEARER_FLOW_INTERNAL_INSUFFICIENT_RESOURCES;
        DS_SYSTEM_HEAP_MEM_FREE(nw_bk);
        return FALSE;
      }
      else
      {         
        ds_flow_configure_flow_context(flow_cntxv6, bearer_context_p);
        nw_bk->new_flow = TRUE;
      }/* else */
      
      nw_bk->is_add = TRUE;   
    }/*flow_cntxv6 == NULL*/
    else if(bearer_context_p->is_default == TRUE &&
    	       flow_cntxv6->qos_info_ptr->field_mask == QOS_MASK_INVALID)
    {
      nw_bk->new_flow = TRUE;
      nw_bk->is_add = TRUE; 
    }
    else if( nw_tft->operation_code == DS_PDN_TFT_OPCODE_ADD)
    {
      nw_bk->is_add = TRUE; 
    }
    else 
    {
      nw_bk->is_add = FALSE;
    }

    /*-------------------------------------------------------------------
      Invoke the add filter to tft helper.
    -------------------------------------------------------------------*/
    return_value = ds_pdn_flow_manager_add_replace_filter_to_tft_helper(
                                                pdn_context_p,
                                                bearer_context_p,
                                                flow_cntxv6,
                                                nw_tft,
                                                nw_bk,
                                                mode_info_p,
                                                cause_code_p);
    //If this is a new flow then we created a new TFT.
    //For the default bearer; we need to treat this differently by disallowing
    //data on the default flow.
    if ((return_value == TRUE) && (TRUE == bearer_context_p->is_default))
      {
        int i;
        // Packets not matching the installed filters will be dropped.
        for (i = 0; i < DS_PDN_INSTANCE_MAX; i++)
        {
          ds_pdn_mgr_inst_type_s *inst_p = pdn_context_p->inst_ptr[i];

          if(inst_p != NULL &&
             inst_p->flow != NULL)
          {      
            ds_flow_set_capability(inst_p->flow->default_flow_ptr,
                                   DS_FLOW_CAPABILITY_DATA_DISALLOWED);
          }
        }
      }

    DS_SYSTEM_HEAP_MEM_FREE(nw_bk);
    return return_value;
  }
  else if(pdp_type == DS_PROFILE_PDP_IPV4V6)
  {
    inst_ptr = pdn_context_p->inst_ptr[DS_PDN_INSTANCE_IPV4];
    if(!inst_ptr)
    {
      *cause_code_p = DS_BEARER_FLOW_INTERNAL_INSUFFICIENT_RESOURCES;
      DS_SYSTEM_HEAP_MEM_FREE(nw_bk);
      return FALSE;
    }
    
    flow_cntxv4 = ds_bearer_flow_mgr_find_flow_context(bearer_context_p,
                                                       DS_FLOW_V4_FLOW_TYPE);
    if(flow_cntxv4 == NULL)
    {
      if(!allow_flow_create)
      {
        // flow create is not allowed, this is a BCM violation, nw tries to 
        // install tft on a UE init flow
        *cause_code_p = DS_BEARER_FLOW_INTERNAL_BCM_VIOLATION;
        DS_SYSTEM_HEAP_MEM_FREE(nw_bk);
        return FALSE;
      }
      /* 
       There is a race condition here, the flow could be getting
       deleted on the PDN, while this request comes thru. In this case
       we bearer flow manager will have 0 flows.
       One other race condition is the between when UE wants to delete
       the bearer, and NW wants to add to this.
      */
      /*-----------------------------------------------------------------
         Retrieve an empty flow context.
      -----------------------------------------------------------------*/
      flow_cntxv4 = ds_flow_allocate_context(inst_ptr);
      if(flow_cntxv4 == NULL)
      {
        *cause_code_p = DS_BEARER_FLOW_INTERNAL_INSUFFICIENT_RESOURCES;
        DS_SYSTEM_HEAP_MEM_FREE(nw_bk);
        return FALSE;
      }
      else
      {         
        ds_flow_configure_flow_context(flow_cntxv4, bearer_context_p);
        nw_bk->new_flow = TRUE;
      }/* else */
      
      nw_bk->is_add = TRUE;   
    }/*flow_cntxv4 == NULL*/
    else if(bearer_context_p->is_default == TRUE &&
    	       flow_cntxv4->qos_info_ptr->field_mask == QOS_MASK_INVALID)
    {
      nw_bk->new_flow = TRUE;
      nw_bk->is_add = TRUE; 
    }
    else if( nw_tft->operation_code == DS_PDN_TFT_OPCODE_ADD)
    {
      nw_bk->is_add = TRUE; 
    }
    else 
    {
      nw_bk->is_add = FALSE;
    }  

    inst_ptr = pdn_context_p->inst_ptr[DS_PDN_INSTANCE_IPV6];
    if(!inst_ptr)
    {
      *cause_code_p = DS_BEARER_FLOW_INTERNAL_INSUFFICIENT_RESOURCES;
      DS_SYSTEM_HEAP_MEM_FREE(nw_bk);
      return FALSE;
    }
    
    flow_cntxv6 = ds_bearer_flow_mgr_find_flow_context(bearer_context_p,
                                                       DS_FLOW_V6_FLOW_TYPE);
    if(flow_cntxv6 == NULL)
    {
      if(!allow_flow_create)
      {
        // flow create is not allowed, this is a BCM violation, nw tries to 
        // install tft on a UE init flow
        *cause_code_p = DS_BEARER_FLOW_INTERNAL_BCM_VIOLATION;
        DS_SYSTEM_HEAP_MEM_FREE(nw_bk);
        return FALSE;
      }
      /* 
       There is a race condition here, the flow could be getting
       deleted on the PDN, while this request comes thru. In this case
       we bearer flow manager will have 0 flows.
       One other race condition is the between when UE wants to delete
       the bearer, and NW wants to add to this.
      */
      /*-----------------------------------------------------------------
         Retrieve an empty flow context.
      -----------------------------------------------------------------*/
      flow_cntxv6 = ds_flow_allocate_context(inst_ptr);
      if(flow_cntxv6 == NULL)
      {
        *cause_code_p = DS_BEARER_FLOW_INTERNAL_INSUFFICIENT_RESOURCES;
        DS_SYSTEM_HEAP_MEM_FREE(nw_bk);
        return FALSE;
      }
      else
      {         
        ds_flow_configure_flow_context(flow_cntxv6, bearer_context_p);
        nw_bk->new_flow = TRUE;
      }/* else */
      
      nw_bk->is_add = TRUE;   
    }/*flow_cntxv6 == NULL*/
    else if(bearer_context_p->is_default == TRUE &&
    	       flow_cntxv6->qos_info_ptr->field_mask == QOS_MASK_INVALID)
    {
      nw_bk->new_flow= TRUE;
      nw_bk->is_add = TRUE; 
    }
    else if( nw_bk->new_flow == TRUE )
    {
      DS_ERROR_LOG_0("Mismatch of Network flows");
      DS_SYSTEM_HEAP_MEM_FREE(nw_bk);
      *cause_code_p = DS_BEARER_FLOW_INTERNAL_INSUFFICIENT_RESOURCES;
      return FALSE;
    }
    else if( nw_tft->operation_code == DS_PDN_TFT_OPCODE_ADD)
    {
      nw_bk->is_add = TRUE; 
    }
    else 
    {
      nw_bk->is_add = FALSE;
    }

   /*-------------------------------------------------------------------
      Check if the network fltr has any fltr ID or fltr preced conflicts.
    -------------------------------------------------------------------*/
    if(ds_pdn_flow_manager_detect_conflict_in_tft(pdn_context_p,
                                                  bearer_context_p,
                                                  flow_cntxv4,
                                                  nw_tft,
                                                  nw_bk,
                                                  cause_code_p,
                                                  FALSE) == FALSE)
    {
      DS_ERROR_LOG_0("Unresolved Conflict in TFT");
      DS_SYSTEM_HEAP_MEM_FREE(nw_bk);
      return FALSE;
    }
    
    /*-------------------------------------------------------------------
       Add the filters to V4 and V6 flows if there is no previous NW flow
       or opcode is add, Otherwise Replace the filters from V4 and V6 flow 
       with new filters 
    -------------------------------------------------------------------*/        
    if(nw_bk->is_add == TRUE)
    {
      if(ds_pdn_flow_manager_add_filter_to_flow(pdn_context_p,
                                                bearer_context_p,
                                                flow_cntxv4,
                                                nw_tft,
                                                nw_bk,
                                                mode_info_p,
                                                nw_bk->new_flow,
                                                cause_code_p) != TRUE)
      { 
        DS_SYSTEM_HEAP_MEM_FREE(nw_bk);
        return FALSE;
      }
 
      if(ds_pdn_flow_manager_add_filter_to_flow(pdn_context_p,
                                                bearer_context_p,
                                                flow_cntxv6,
                                                nw_tft,
                                                nw_bk,
                                                mode_info_p,
                                                nw_bk->new_flow,
                                                cause_code_p) != TRUE)
      { 
        DS_SYSTEM_HEAP_MEM_FREE(nw_bk);
        return FALSE;
      }
    }
    else
    {
      if(ds_pdn_flow_manager_replace_filter_in_flow(pdn_context_p,
                                                    bearer_context_p,
                                                    flow_cntxv4,
                                                    nw_tft,
                                                    nw_bk,
                                                    mode_info_p,
                                                    cause_code_p) != TRUE)
      { 
        DS_SYSTEM_HEAP_MEM_FREE(nw_bk);
        return FALSE;
      } 

      if(ds_pdn_flow_manager_replace_filter_in_flow(pdn_context_p,
                                                    bearer_context_p,
                                                    flow_cntxv6,
                                                    nw_tft,
                                                    nw_bk,
                                                    mode_info_p,
                                                    cause_code_p) != TRUE)
      { 
        DS_SYSTEM_HEAP_MEM_FREE(nw_bk);
        return FALSE;
      }
    }
    
    /*-------------------------------------------------------------------
      Resolve any filter conflict.
    ---------------------------------------------------------------------*/
    if(ds_pdn_flow_manager_handle_fltr_conflict(pdn_context_p,
                                                bearer_context_p,
                                                flow_cntxv6,
                                                nw_bk) == FALSE)
    {
      DS_ERROR_LOG_0("unable to handle filter conflict");
      DS_SYSTEM_HEAP_MEM_FREE(nw_bk);
      return FALSE;
    }
    
    //If this is a new flow then we created a new TFT.
    //For the default bearer; we need to treat this differently by disallowing
    //data on the default flow.
    if (TRUE == bearer_context_p->is_default)
      {
        int i;
        // Packets not matching the installed filters will be dropped.
        for (i = 0; i < DS_PDN_INSTANCE_MAX; i++)
        {
          ds_pdn_mgr_inst_type_s *inst_p = pdn_context_p->inst_ptr[i];

          if(inst_p != NULL &&
             inst_p->flow != NULL)
          {      
            ds_flow_set_capability(inst_p->flow->default_flow_ptr,
                                   DS_FLOW_CAPABILITY_DATA_DISALLOWED);
          }
        }
      }

    DS_SYSTEM_HEAP_MEM_FREE(nw_bk);
    return TRUE;
  }

  DS_ERROR_LOG_0("Unknown IP type");
  DS_SYSTEM_HEAP_MEM_FREE(nw_bk);
  return FALSE;
} /* ds_pdn_flow_manager_add_filter_to_tft */

/*===========================================================================
FUNCTION DS_PDN_FLOW_MANAGER_REPLACE_FILTER_IN_TFT

DESCRIPTION
  Replace filters in existing TFT.

PARAMETERS
  pdn_context_p         -  PDN information.
  bearer_context_p      -  Bearer information.
  nw_tft                -  TFT information.
  cause_code_p          -  Cause code NW TFT is not accurate.

DEPENDENCIES
  None.

RETURN VALUE
boolean - return TRUE or FALSE
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_flow_manager_replace_filter_in_tft
(
  ds_pdn_mgr_s                                 *pdn_context_p,
  ds_bearer_mgr_s                              *bearer_context_p,
  tft_type_T                                   *nw_tft,
  emm_modify_bearer_context_request_ind_s_type *mode_info_p,
  ds_bearer_flow_internal_cause_code_enum_type *cause_code_p
)
{
  ds_flow_type                    *flow_cntxv4 = NULL;
  ds_flow_type                    *flow_cntxv6 = NULL; 
  ds_pdn_tft_nw_modify_qos_bk_s   *nw_bk;
  ds_profile_pdp_type_e            pdp_type;
  boolean                          return_value = FALSE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
   
  /*-----------------------------------------------------------------------
    Verify Input params
  -----------------------------------------------------------------------*/
  if( !ds_pdn_mgr_validate_pdn_ptr(pdn_context_p) ||
      !ds_bearer_mgr_validate_bearer_ptr(bearer_context_p) ||
      (nw_tft == NULL) || (cause_code_p== NULL) )
  {
    if(cause_code_p != NULL)
    {
      *cause_code_p = DS_BEARER_FLOW_INTERNAL_INSUFFICIENT_RESOURCES;
    }
    return FALSE;
  }
  
  /*-----------------------------------------------------------------------
     Check for TFT Semantic Errors
  -----------------------------------------------------------------------*/
  if(ds_pdn_tft_check_mod_fltr_semantic_error(nw_tft) == TRUE)
  {
    *cause_code_p = DS_BEARER_FLOW_INTERNAL_SYNTACTICAL_ERR_IN_TFT;
    return FALSE;
  }
  else if(ds_pdn_tft_check_for_repeated_precedence(nw_tft) == TRUE)
  {
    *cause_code_p = DS_BEARER_FLOW_INTERNAL_SEMANTIC_ERR_IN_FILTER;
    return FALSE;
  }

  /*-----------------------------------------------------------------------
     Allocate Heap Memory
  -----------------------------------------------------------------------*/
  DS_SYSTEM_HEAP_MEM_ALLOC(nw_bk,
                                       sizeof(ds_pdn_tft_nw_modify_qos_bk_s),
                           ds_pdn_tft_nw_modify_qos_bk_s*);
  if(nw_bk == NULL)
  {
    *cause_code_p = DS_BEARER_FLOW_INTERNAL_INSUFFICIENT_RESOURCES;
    return FALSE;
  }
  memset(nw_bk, 0,sizeof(ds_pdn_tft_nw_modify_qos_bk_s));

  pdp_type = ds_apn_table_get_pdn_pdp_type(pdn_context_p);
  
  if(pdp_type == DS_PROFILE_PDP_IPV4)
  {   
    flow_cntxv4 = ds_bearer_flow_mgr_find_flow_context(bearer_context_p,
                                                       DS_FLOW_V4_FLOW_TYPE);
    if(flow_cntxv4 == NULL)
    {
     DS_ERROR_LOG_0("No previous NW flow context associated flow manager");      
     *cause_code_p = DS_BEARER_FLOW_INTERNAL_INSUFFICIENT_RESOURCES;
     return_value = ds_pdn_flow_manager_add_filter_to_tft(pdn_context_p,
                                                          bearer_context_p,
                                                          nw_tft,
                                                          mode_info_p,
                                                          cause_code_p,
                                                          TRUE);            
     DS_SYSTEM_HEAP_MEM_FREE(nw_bk);
     return return_value;        
    }/*flow_cntxv4 == NULL*/

    nw_bk->is_add = FALSE; 

    /*-------------------------------------------------------------------
      Invoke the add filter to tft helper.
    -------------------------------------------------------------------*/
    return_value = ds_pdn_flow_manager_add_replace_filter_to_tft_helper(
                                                pdn_context_p,
                                                bearer_context_p,
                                                flow_cntxv4,
                                                nw_tft,
                                                nw_bk,
                                                mode_info_p,
                                                cause_code_p);     
    DS_SYSTEM_HEAP_MEM_FREE(nw_bk);
    return return_value;
  }
  else if(pdp_type == DS_PROFILE_PDP_IPV6)
  {    
    flow_cntxv6 = ds_bearer_flow_mgr_find_flow_context(bearer_context_p,
                                                       DS_FLOW_V6_FLOW_TYPE);
    if(flow_cntxv6 == NULL)
    {
      DS_ERROR_LOG_0("No previous NW flow context associated flow manager");      
      *cause_code_p = DS_BEARER_FLOW_INTERNAL_INSUFFICIENT_RESOURCES;
      return_value = ds_pdn_flow_manager_add_filter_to_tft(pdn_context_p,
                                                           bearer_context_p,
                                                           nw_tft,
                                                           mode_info_p,
                                                           cause_code_p,
                                                           TRUE);            
      DS_SYSTEM_HEAP_MEM_FREE(nw_bk);
      return return_value;        
    }/*flow_cntxv4 == NULL*/

    nw_bk->is_add = FALSE; 

    /*-------------------------------------------------------------------
      Invoke the add filter to tft helper.
    -------------------------------------------------------------------*/
    return_value = ds_pdn_flow_manager_add_replace_filter_to_tft_helper(
                                                pdn_context_p,
                                                bearer_context_p,
                                                flow_cntxv6,
                                                nw_tft,
                                                nw_bk,
                                                mode_info_p,
                                                cause_code_p);     
    DS_SYSTEM_HEAP_MEM_FREE(nw_bk);
    return return_value;
  }
  else if(pdp_type == DS_PROFILE_PDP_IPV4V6)
  {
    flow_cntxv4 = ds_bearer_flow_mgr_find_flow_context(bearer_context_p,
                                                       DS_FLOW_V4_FLOW_TYPE);
    if(flow_cntxv4 == NULL)
    {
      DS_ERROR_LOG_0("No previous NW flow context associated flow manager");      
      *cause_code_p = DS_BEARER_FLOW_INTERNAL_INSUFFICIENT_RESOURCES;
      return_value = ds_pdn_flow_manager_add_filter_to_tft(pdn_context_p,
                                                           bearer_context_p,
                                                           nw_tft,
                                                           mode_info_p,
                                                           cause_code_p,
                                                           TRUE);            
      DS_SYSTEM_HEAP_MEM_FREE(nw_bk);
      return return_value;        
    }/*flow_cntxv4 == NULL*/
    
    flow_cntxv6 = ds_bearer_flow_mgr_find_flow_context(bearer_context_p,
                                                       DS_FLOW_V6_FLOW_TYPE);
    if(flow_cntxv6 == NULL)
    {
      DS_ERROR_LOG_0("No previous NW flow context associated flow manager");      
      *cause_code_p = DS_BEARER_FLOW_INTERNAL_INSUFFICIENT_RESOURCES;
      return_value = ds_pdn_flow_manager_add_filter_to_tft(pdn_context_p,
                                                           bearer_context_p,
                                                           nw_tft,
                                                           mode_info_p,
                                                           cause_code_p,
                                                           TRUE);            
      DS_SYSTEM_HEAP_MEM_FREE(nw_bk);
      return return_value;        
    }/*flow_cntxv4 == NULL*/

    nw_bk->is_add = FALSE; 

   /*-------------------------------------------------------------------
      Check if the network fltr has any fltr ID or fltr preced conflicts.
    -------------------------------------------------------------------*/
    if(ds_pdn_flow_manager_detect_conflict_in_tft(pdn_context_p,
                                                  bearer_context_p,
                                                  flow_cntxv4,
                                                  nw_tft,
                                                  nw_bk,
                                                  cause_code_p,
                                                  FALSE) == FALSE)
    {
      DS_ERROR_LOG_0("Unresolved Conflict in TFT");
      DS_SYSTEM_HEAP_MEM_FREE(nw_bk);
      return FALSE;
    }
    
    /*-------------------------------------------------------------------
       Replace the filters from V4 and V6 flow  with new filters 
    -------------------------------------------------------------------*/        
    if(ds_pdn_flow_manager_replace_filter_in_flow(pdn_context_p,
                                                  bearer_context_p,
                                                  flow_cntxv4,
                                                  nw_tft,
                                                  nw_bk,
                                                  mode_info_p,
                                                  cause_code_p) != TRUE)
    { 
      DS_SYSTEM_HEAP_MEM_FREE(nw_bk);
      return FALSE;
    } 

    if(ds_pdn_flow_manager_replace_filter_in_flow(pdn_context_p,
                                                  bearer_context_p,
                                                  flow_cntxv6,
                                                  nw_tft,
                                                  nw_bk,
                                                  mode_info_p,
                                                  cause_code_p) != TRUE)
    { 
      DS_SYSTEM_HEAP_MEM_FREE(nw_bk);
      return FALSE;
    }
    
    /*-------------------------------------------------------------------
      Resolve any filter conflict.
    ---------------------------------------------------------------------*/
    if(ds_pdn_flow_manager_handle_fltr_conflict(pdn_context_p,
                                                bearer_context_p,
                                                flow_cntxv6,
                                                nw_bk) == FALSE)
    {
      DS_ERROR_LOG_0("unable to handle filter conflict");
      DS_SYSTEM_HEAP_MEM_FREE(nw_bk);
      return FALSE;
    }    

    DS_SYSTEM_HEAP_MEM_FREE(nw_bk);
    return TRUE;
  }
  
  DS_SYSTEM_HEAP_MEM_FREE(nw_bk);
  return FALSE;
} /* ds_pdn_flow_manager_replace_filter_in_tft */

/*===========================================================================
FUNCTION DS_PDN_FLOW_MANAGER_DELETE_FILTER_FROM_FLOW

DESCRIPTION
  Delete Filter ID from a flow .

PARAMETERS
  pdn_context_p         -  PDN information
  bearer_context_p      -  Bearer information
  flow_context_p        -  Flow context.
  nw_tft                -  TFT information
  nw_bk                 -  Network book keeping info.
  mode_info_p           -  Mode specific Qos.

DEPENDENCIES
  None.

RETURN VALUE
boolean - return TRUE or FALSE
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_flow_manager_delete_filter_from_flow
(
 ds_pdn_mgr_s                                 *pdn_context_p,
 ds_bearer_mgr_s                              *bearer_context_p,
 ds_flow_type                                 *flow_context_p,
 tft_type_T                                   *nw_tft,
 ds_pdn_tft_nw_modify_qos_bk_s                *nw_bk,
 emm_modify_bearer_context_request_ind_s_type *mode_info_p,
 ds_bearer_flow_internal_cause_code_enum_type *cause_code_p
)
{
  uint8        orig_rx_filter_cnt=0;
  uint8        orig_tx_filter_cnt=0;
  ds_ipfltr_ctrl_param_type   fltr_ctrl_param = {0};
  int16                       err_no;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(cause_code_p == NULL)
  {
    return FALSE;
  }
  else
  {
    *cause_code_p = DS_BEARER_FLOW_INTERNAL_INSUFFICIENT_RESOURCES;
  }

  if(!flow_context_p)
  {
    return FALSE;
  }
  
  /*-------------------------------------------------------------------------
    Configure the QOS flow spec
  -------------------------------------------------------------------------*/
  if(ds_pdn_tft_configure_nw_bk_flow_spec(nw_bk) == FALSE)
  {
    return FALSE;
  }

  if(nw_bk->flow_cntx_fltr_id != 0)
  {
    /*-------------------------------------------------------------------
      Retrieve current filter info for flow context.
    ---------------------------------------------------------------------*/
    memscpy(&nw_bk->curr_qos, sizeof(qos_spec_type),
            flow_context_p->qos_info_ptr, sizeof(qos_spec_type));   

    nw_bk->curr_qos.rx.fltr_template.list_ptr = nw_bk->curr_rx_fltr;
    memscpy(nw_bk->curr_qos.rx.fltr_template.list_ptr,
            sizeof(ip_filter_type)*MAX_PACKET_FILTERS,
            flow_context_p->qos_info_ptr->rx.fltr_template.list_ptr,
            sizeof(ip_filter_type)*MAX_PACKET_FILTERS);
    
    nw_bk->curr_qos.tx.fltr_template.list_ptr = nw_bk->curr_tx_fltr;
    memscpy(nw_bk->curr_qos.tx.fltr_template.list_ptr,
            sizeof(ip_filter_type)*MAX_PACKET_FILTERS,
            flow_context_p->qos_info_ptr->tx.fltr_template.list_ptr,
            sizeof(ip_filter_type)*MAX_PACKET_FILTERS);    

    /*-------------------------------------------------------------------
      Delete the filters.
    ---------------------------------------------------------------------*/
    if(ds_flow_delete_filter_from_flow(flow_context_p,
                                       nw_bk->flow_cntx_fltr_id,
                                       &(nw_bk->curr_qos),
                                       &(nw_bk->mod_qos))==FALSE)
    {
      return FALSE;
    }
    
    /*-------------------------------------------------------------------
      Update the filter pointers and number of filters.
    ---------------------------------------------------------------------*/
    nw_bk->curr_qos.rx.fltr_template.list_ptr = nw_bk->mod_rx_fltr;
    nw_bk->curr_qos.tx.fltr_template.list_ptr = nw_bk->mod_tx_fltr;
    orig_rx_filter_cnt = nw_bk->curr_qos.rx.fltr_template.num_filters;
    orig_tx_filter_cnt = nw_bk->curr_qos.tx.fltr_template.num_filters;
    nw_bk->curr_qos.rx.fltr_template.num_filters = 
                                nw_bk->mod_qos.rx.fltr_template.num_filters;
    nw_bk->curr_qos.tx.fltr_template.num_filters = 
                                nw_bk->mod_qos.tx.fltr_template.num_filters;
    /*-------------------------------------------------------------------
      Update the TX Filter Mask.
    ---------------------------------------------------------------------*/
    if((nw_bk->curr_qos.tx.fltr_template.num_filters > 0) &&
       (orig_tx_filter_cnt > nw_bk->curr_qos.tx.fltr_template.num_filters))
    {
      nw_bk->curr_qos.field_mask |=
            (qos_spec_field_mask_type)QOS_MODIFY_MASK_TX_FLTR_MODIFY;  
    }
    /*-------------------------------------------------------------------
      Update the RX Filter Mask.
    ---------------------------------------------------------------------*/ 
    if((nw_bk->curr_qos.rx.fltr_template.num_filters > 0) &&
       (orig_rx_filter_cnt > nw_bk->curr_qos.rx.fltr_template.num_filters) )
    {
      nw_bk->curr_qos.field_mask |=
            (qos_spec_field_mask_type)QOS_MODIFY_MASK_RX_FLTR_MODIFY;  
    } 
    /*---------------------------------------------------------------------
      Convert NW Qos to App Qos
    ---------------------------------------------------------------------*/
    if(mode_info_p != NULL)
    {
      if(ds_bearer_flow_manager_convert_data_rate(bearer_context_p,
                                                  &mode_info_p->sdf_qos,
                                                  &mode_info_p->neg_qos,
                                                  &nw_bk->curr_qos,
                                                  cause_code_p) == FALSE)
      {
        return FALSE;
      }
      // Need to check data rate before configuring the field mask.
      if(nw_bk->curr_qos.rx.flow_template.req_flow.field_mask)                        
      {
        if (orig_rx_filter_cnt != 0)
        {
          nw_bk->curr_qos.field_mask |= ((qos_spec_field_mask_type)
                                            QOS_MASK_RX_FLOW);
        }
      }
      if(nw_bk->curr_qos.tx.flow_template.req_flow.field_mask)                        
      {
        if(orig_tx_filter_cnt != 0)
        {		
          nw_bk->curr_qos.field_mask |= ((qos_spec_field_mask_type)
                                            QOS_MASK_TX_FLOW);
        } 
      } 
    }/* mode_info_p != NULL */
    /*-----------------------------------------------------------------------
      If deletion of filter has made num_filters 0 in some direction. 
      We need to Update IP_FLOW_MASK_NONE so that frameowrk deletes all filters.
    -----------------------------------------------------------------------*/
    if( (orig_rx_filter_cnt > 0) &&
        (nw_bk->curr_qos.rx.fltr_template.num_filters == 0))
    {
      DS_MSG1(MSG_LEGACY_HIGH,
              "Deleted all (%d) filters in RX direction", orig_rx_filter_cnt);
      nw_bk->curr_qos.field_mask |= QOS_MASK_RX_FLOW;
      nw_bk->curr_qos.rx.flow_template.req_flow.field_mask = 
            (ipflow_field_mask_type)IPFLOW_MASK_NONE;
    }
    
    if( (orig_tx_filter_cnt > 0) &&
        (nw_bk->curr_qos.tx.fltr_template.num_filters == 0))
    {
      DS_MSG1(MSG_LEGACY_HIGH,
              "Deleted all (%d) filters in TX direction", orig_tx_filter_cnt);
      nw_bk->curr_qos.field_mask |= QOS_MASK_TX_FLOW;
      nw_bk->curr_qos.tx.flow_template.req_flow.field_mask = 
            (ipflow_field_mask_type)IPFLOW_MASK_NONE;
    }

    if(ds_pdn_nv_manager_is_rel_12_or_above_supported())
    {
      if((nw_bk->curr_qos.tx.fltr_template.num_filters == 0) && 
         (!bearer_context_p->is_default))
      {
         DS_ERROR_LOG_0("Resultant tft does not have any uplink TX filters");
         *cause_code_p = DS_BEARER_FLOW_INTERNAL_SEMANTIC_ERR_IN_FILTER;  
         return FALSE;
      }
    }
        
    /*-------------------------------------------------------------------
      Disable Filtering on the Flow.
    ---------------------------------------------------------------------*/
   fltr_ctrl_param.ds_fwk_inst_ptr = ds_fwk_get_inst_ptr_from_index(
                                                 pdn_context_p->fw_index);
   fltr_ctrl_param.enable = FALSE;
   fltr_ctrl_param.client_id = FLTR_CLIENT_QOS_OUTPUT;
   fltr_ctrl_param.fi_handle = 
               flow_context_p->qos_info_ptr->tx.fltr_template.fltr_handle;
    
   if(ds_ipfltr_control(&fltr_ctrl_param, &err_no) == -1)
   {
     DS_ERROR_LOG_0("flow filtering control failed");
   }

    /*-----------------------------------------------------------------------
      Invoke the Flow Modify.
    -----------------------------------------------------------------------*/
    if(ds_flow_modify_flow_context(flow_context_p, &nw_bk->curr_qos) == FALSE)
    {
      DS_ERROR_LOG_0("modify Flow failed");
      /*-------------------------------------------------------------------
        Enable Filtering on the Flow.
      ---------------------------------------------------------------------*/
      fltr_ctrl_param.ds_fwk_inst_ptr = ds_fwk_get_inst_ptr_from_index(
                                                    pdn_context_p->fw_index);
      fltr_ctrl_param.enable = TRUE;
      fltr_ctrl_param.client_id = FLTR_CLIENT_QOS_OUTPUT;
      fltr_ctrl_param.fi_handle = 
                  flow_context_p->qos_info_ptr->tx.fltr_template.fltr_handle;
       
      if(ds_ipfltr_control(&fltr_ctrl_param, &err_no) == -1)
      {
        DS_ERROR_LOG_0("flow filtering control failed");
      }
      return FALSE;
    }

    flow_context_p->state = DS_FLOW_STATE_PENDING_CONFIRM;
   
    /*-------------------------------------------------------------------
      Update the filter pointers and filter count to the original cnt
    ---------------------------------------------------------------------*/
    nw_bk->curr_qos.rx.fltr_template.list_ptr  = nw_bk->curr_rx_fltr;
    nw_bk->curr_qos.tx.fltr_template.list_ptr  = nw_bk->curr_tx_fltr;
    /*-------------------------------------------------------------------
      Retrieve the filter count before modification
    ---------------------------------------------------------------------*/
    nw_bk->curr_qos.rx.fltr_template.num_filters = orig_rx_filter_cnt;
    nw_bk->curr_qos.tx.fltr_template.num_filters = orig_tx_filter_cnt;
  
    /*-------------------------------------------------------------------
      Update Precedence and Filter ID information.
    ---------------------------------------------------------------------*/
    if(ds_pdn_flow_manager_update_filter_id_precedence(
                                              pdn_context_p,
                                              bearer_context_p,
                                              flow_context_p,
                                              nw_bk->flow_cntx_fltr_id,
                                              &(nw_bk->curr_qos),
                                              TRUE) == FALSE)
    {
      DS_ERROR_LOG_0("Error updating filter Id and precedence");
      return FALSE;
    }
  }/* nw_bk->flow_cntx_fltr_id != 0*/ 
  
  return TRUE;
} /* ds_pdn_flow_manager_delete_filter_from_flow */

/*===========================================================================
FUNCTION DS_PDN_FLOW_MANAGER_PROCESS_DELETE_TFT

DESCRIPTION
  This function is invoked to handle the Delete Existing TFT operation.

PARAMETERS
  pdn_context_p     -  PDN context info.
  bearer_context_p  -  Bearer context info
  nw_tft_p          -  TFT information.
  cause_code_p      -  Cause code in case of failure.

DEPENDENCIES
  None.

RETURN VALUE
  boolean  - Returns True if success.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_flow_manager_process_delete_tft
(
  ds_pdn_mgr_s                                 *pdn_context_p,
  ds_bearer_mgr_s                              *bearer_context_p,
  tft_type_T                                   *nw_tft,
  ds_bearer_flow_internal_cause_code_enum_type *cause_code_p
)
{
  uint8     i = 0;
  
  if((nw_tft->operation_code == DS_PDN_TFT_OPCODE_DEL_TFT) &&
     (nw_tft->num_filters > 0))
  {
    DS_ERROR_LOG_0("non-empty TFT with op-code Delete-existing TFT recieved, "
                   "Reject modify");
    *cause_code_p = DS_BEARER_FLOW_INTERNAL_SYNTACTICAL_ERR_IN_TFT;
    return FALSE;
  }

  if(!bearer_context_p->is_default)
  {
    return FALSE;
  }

  if(!ds_pdn_flow_manager_free_nw_flow_context(bearer_context_p))
  {
    *cause_code_p = DS_BEARER_FLOW_INTERNAL_INSUFFICIENT_RESOURCES;
    return FALSE;
  }

  //Reset the capability of the default flow from PS_FLOW_CAPABILITY_DATA_DISALLOWED.
  //No packet on default flow will be dropped.   
  for (i = 0; i < DS_PDN_INSTANCE_MAX; i++)
  {
    ds_pdn_mgr_inst_type_s *inst_p = pdn_context_p->inst_ptr[i];
    if(inst_p != NULL &&
       inst_p->flow != NULL)
    {              
      ds_flow_reset_capability(inst_p->flow->default_flow_ptr,
                               DS_FLOW_CAPABILITY_DATA_DISALLOWED); 
    }
  }
  
  return TRUE;
}/* ds_pdn_flow_manager_process_delete_tft */

/*===========================================================================
FUNCTION DS_PDN_FLOW_MANAGER_REMOVE_FILTER_FROM_TFT_HELPER

DESCRIPTION
  remove filters to existing TFT.

PARAMETERS
  pdn_context_p         -  PDN information
  bearer_context_p      -  Bearer information
  flow_context_p        -  Flow information.
  nw_tft                -  TFT information.
  nw_bk                 -  Network book keeping info.
  mode_info_p           -  Mode specific Qos.
  cause_code_p          -  Cause code NW TFT is not accurate

DEPENDENCIES
  None.

RETURN VALUE
boolean - return DS3G_FAILURE or DS3G_SUCCESS
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_flow_manager_remove_filter_from_tft_helper
(
 ds_pdn_mgr_s                                 *pdn_context_p,
 ds_bearer_mgr_s                              *bearer_context_p,
 ds_flow_type                                 *flow_context_p,
 tft_type_T                                   *nw_tft,
 ds_pdn_tft_nw_modify_qos_bk_s                *nw_bk,
 emm_modify_bearer_context_request_ind_s_type *mode_info_p,
 ds_bearer_flow_internal_cause_code_enum_type *cause_code_p
)
{
  ds_bearer_flow_manager_s *bflow_manager_p = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------
    Check if the network fltr has any fltr ID or fltr preced conflicts.
  -------------------------------------------------------------------*/
  if(ds_pdn_flow_manager_detect_conflict_in_tft(pdn_context_p,
                                                bearer_context_p,
                                                flow_context_p,
                                                nw_tft,
                                                nw_bk,
                                                cause_code_p,
                                                TRUE) == FALSE)
  {
    DS_ERROR_LOG_0("Unresolved Conflict in TFT");
    return FALSE;
  }   
  
  /*-----------------------------------------------------------------------
    Check for Additional TFT Semantic Errors
  -----------------------------------------------------------------------*/
  bflow_manager_p = ds_bearer_flow_manager_find_entry(bearer_context_p);
  if(bflow_manager_p)
  {
    if((nw_bk->non_match_fltr_id == 0)&&
       ((bflow_manager_p->fi_identifier &   
         (nw_bk->bearer_fltr_id|nw_bk->flow_cntx_fltr_id))
                 == (bflow_manager_p->fi_identifier)))
    {
      DS_MSG0(MSG_LEGACY_HIGH, "Network deleting all filters,tft is empty");
      
      if(bearer_context_p->is_default)
      {
        //In this case this is equivalent to a Delete TFT operation.
        return ds_pdn_flow_manager_process_delete_tft(pdn_context_p,
                                                      bearer_context_p,
                                                      nw_tft,
                                                      cause_code_p);
      }
      else
      {
        *cause_code_p = DS_BEARER_FLOW_INTERNAL_SEMANTIC_ERR_IN_TFT;
        return FALSE;     
      }      	
    }
    
    /*-----------------------------------------------------------------------
     5) TFT operation = "Delete packet filters from existing TFT" with a packet 
     filter list also including packet filters in addition to the packet filter identifiers.
    -----------------------------------------------------------------------*/    
    else if(pdn_context_p->nw_mode == SYS_SYS_MODE_LTE &&
            nw_tft->tft_total_length != nw_tft->num_filters + 1)
    {
      DS_ERROR_LOG_0("Network TFT IE has packet filters, in addition to "
   	                 "filter identifiers. Syntactical  error");
      *cause_code_p = DS_BEARER_FLOW_INTERNAL_SYNTACTICAL_ERR_IN_TFT;
      return FALSE;
    }	
    else if((nw_bk->non_match_fltr_id > 0)&&
            ((nw_bk->bearer_fltr_id|nw_bk->flow_cntx_fltr_id) > 0))      	 
    {
      DS_MSG0(MSG_LEGACY_HIGH, 
              "Network TFT IE has filter other than those installed"
              " on this flow. Ignore");            
    }
  }
  else
  {
    *cause_code_p = DS_BEARER_FLOW_INTERNAL_INSUFFICIENT_RESOURCES;
    return FALSE;
  }
  
  /*-------------------------------------------------------------------
    Check if NW is deleting non-existing filters.
  -------------------------------------------------------------------*/
  if((nw_bk->bearer_fltr_id == 0) && (nw_bk->flow_cntx_fltr_id == 0))
  {
    DS_ERROR_LOG_0("Network trying to delete non-existing filters");
    return TRUE;
  }
  
  /*-------------------------------------------------------------------
    Check if NW is deleting UE requested filters.
  -------------------------------------------------------------------*/
  if(nw_bk->bearer_fltr_id != 0)
  {
    DS_ERROR_LOG_0("Network trying to delete filters set up by UE");
  }
  
  /*-------------------------------------------------------------------
    Delete the filters.
  ---------------------------------------------------------------------*/
  if(ds_pdn_flow_manager_delete_filter_from_flow(pdn_context_p,
                                                 bearer_context_p,
                                                 flow_context_p,
                                                 nw_tft, 
                                                 nw_bk,
                                                 mode_info_p,
                                                 cause_code_p) == FALSE)
  {
    DS_ERROR_LOG_0("Could not delete filter from Flow");
    return FALSE; 
  }
  /*-------------------------------------------------------------------
     Resolve any filter conflict.
  ---------------------------------------------------------------------*/
  if(ds_pdn_flow_manager_handle_fltr_conflict(pdn_context_p,
                                              bearer_context_p,
                                              flow_context_p,
                                              nw_bk) == FALSE)
  {
    DS_ERROR_LOG_0("unable to handle filter conflict");
    return FALSE;
  }

  return TRUE;
}/* ds_pdn_flow_manager_remove_filter_from_tft_helper */

/*===========================================================================
FUNCTION DS_PDN_FLOW_MANAGER_REMOVE_FILTER_FROM_TFT

DESCRIPTION
  Remove filters from existing TFT.

PARAMETERS
  pdn_context_p         -  PDN information.
  bearer_context_p      -  Bearer information.
  nw_tft                -  TFT information.
  cause_code_p          -  Cause code NW TFT is not accurate.

DEPENDENCIES
  None.

RETURN VALUE
boolean - return TRUE or FALSE
  
SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_pdn_flow_manager_remove_filter_from_tft
(
  ds_pdn_mgr_s                                 *pdn_context_p,
  ds_bearer_mgr_s                              *bearer_context_p,
  tft_type_T                                   *nw_tft,
  emm_modify_bearer_context_request_ind_s_type *mode_info_p,
  ds_bearer_flow_internal_cause_code_enum_type *cause_code_p
)
{
  ds_flow_type                    *flow_cntxv4 = NULL;
  ds_flow_type                    *flow_cntxv6 = NULL; 
  ds_pdn_tft_nw_modify_qos_bk_s   *nw_bk;
  ds_profile_pdp_type_e            pdp_type;
  ds_bearer_flow_manager_s        *bflow_manager_p = NULL;
  boolean                          return_value = FALSE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
   
  /*-----------------------------------------------------------------------
    Verify Input params
  -----------------------------------------------------------------------*/
  if( !ds_pdn_mgr_validate_pdn_ptr(pdn_context_p) ||
      !ds_bearer_mgr_validate_bearer_ptr(bearer_context_p) ||
      (nw_tft == NULL) || (cause_code_p== NULL) )
  {
    if(cause_code_p != NULL)
    {
      *cause_code_p = DS_BEARER_FLOW_INTERNAL_INSUFFICIENT_RESOURCES;
    }
    return FALSE;
  }
  
  /*-----------------------------------------------------------------------
     Check for TFT Semantic Errors
  -----------------------------------------------------------------------*/
  if(ds_pdn_tft_check_mod_fltr_semantic_error(nw_tft) == TRUE)
  {
    *cause_code_p = DS_BEARER_FLOW_INTERNAL_SYNTACTICAL_ERR_IN_TFT;
    return FALSE;
  }

  /*-----------------------------------------------------------------------
     Allocate Heap Memory
  -----------------------------------------------------------------------*/
  DS_SYSTEM_HEAP_MEM_ALLOC(nw_bk,
                                       sizeof(ds_pdn_tft_nw_modify_qos_bk_s),
                           ds_pdn_tft_nw_modify_qos_bk_s*);
  if(nw_bk == NULL)
  {
    *cause_code_p = DS_BEARER_FLOW_INTERNAL_INSUFFICIENT_RESOURCES;
    return FALSE;
  }
  memset(nw_bk, 0,sizeof(ds_pdn_tft_nw_modify_qos_bk_s));

  pdp_type = ds_apn_table_get_pdn_pdp_type(pdn_context_p);
  
  if(pdp_type == DS_PROFILE_PDP_IPV4)
  {   
    flow_cntxv4 = ds_bearer_flow_mgr_find_flow_context(bearer_context_p,
                                                       DS_FLOW_V4_FLOW_TYPE);
    if(flow_cntxv4 == NULL)
    {
      DS_ERROR_LOG_0("No previous NW flow context associated flow manager");      
      *cause_code_p = DS_BEARER_FLOW_INTERNAL_INSUFFICIENT_RESOURCES;
      DS_SYSTEM_HEAP_MEM_FREE(nw_bk);
      return FALSE;        
    }/*flow_cntxv4 == NULL*/

    /*-------------------------------------------------------------------
      Invoke the remove filter from tft helper.
    -------------------------------------------------------------------*/
    return_value = ds_pdn_flow_manager_remove_filter_from_tft_helper(
                                                pdn_context_p,
                                                bearer_context_p,
                                                flow_cntxv4,
                                                nw_tft,
                                                nw_bk,
                                                mode_info_p,
                                                cause_code_p);     
    DS_SYSTEM_HEAP_MEM_FREE(nw_bk);
    return return_value;
  }
  else if(pdp_type == DS_PROFILE_PDP_IPV6)
  {    
    flow_cntxv6 = ds_bearer_flow_mgr_find_flow_context(bearer_context_p,
                                                       DS_FLOW_V6_FLOW_TYPE);
    if(flow_cntxv6 == NULL)
    {
      DS_ERROR_LOG_0("No previous NW flow context associated flow manager");      
      *cause_code_p = DS_BEARER_FLOW_INTERNAL_INSUFFICIENT_RESOURCES;           
      DS_SYSTEM_HEAP_MEM_FREE(nw_bk);
      return return_value;        
    }/*flow_cntxv4 == NULL*/

    nw_bk->is_add = FALSE; 

    /*-------------------------------------------------------------------
      Invoke the remove filter from tft helper.
    -------------------------------------------------------------------*/
    return_value = ds_pdn_flow_manager_remove_filter_from_tft_helper(
                                                pdn_context_p,
                                                bearer_context_p,
                                                flow_cntxv6,
                                                nw_tft,
                                                nw_bk,
                                                mode_info_p,
                                                cause_code_p);     
    DS_SYSTEM_HEAP_MEM_FREE(nw_bk);
    return return_value;
  }
  else if(pdp_type == DS_PROFILE_PDP_IPV4V6)
  {
    flow_cntxv4 = ds_bearer_flow_mgr_find_flow_context(bearer_context_p,
                                                       DS_FLOW_V4_FLOW_TYPE);
    if(flow_cntxv4 == NULL)
    {
      DS_ERROR_LOG_0("No previous NW flow context associated flow manager");      
      *cause_code_p = DS_BEARER_FLOW_INTERNAL_INSUFFICIENT_RESOURCES;
      DS_SYSTEM_HEAP_MEM_FREE(nw_bk);
      return return_value;        
    }/*flow_cntxv4 == NULL*/
    
    flow_cntxv6 = ds_bearer_flow_mgr_find_flow_context(bearer_context_p,
                                                       DS_FLOW_V6_FLOW_TYPE);
    if(flow_cntxv6 == NULL)
    {
      DS_ERROR_LOG_0("No previous NW flow context associated flow manager");      
      *cause_code_p = DS_BEARER_FLOW_INTERNAL_INSUFFICIENT_RESOURCES;
      DS_SYSTEM_HEAP_MEM_FREE(nw_bk);
      return return_value;        
    }/*flow_cntxv4 == NULL*/

   /*-------------------------------------------------------------------
      Check if the network fltr has any fltr ID or fltr preced conflicts.
    -------------------------------------------------------------------*/
    if(ds_pdn_flow_manager_detect_conflict_in_tft(pdn_context_p,
                                                  bearer_context_p,
                                                  flow_cntxv4,
                                                  nw_tft,
                                                  nw_bk,
                                                  cause_code_p,
                                                  TRUE) == FALSE)
    {
      DS_ERROR_LOG_0("Unresolved Conflict in TFT");
      DS_SYSTEM_HEAP_MEM_FREE(nw_bk);
      return FALSE;
    }

   /*---------------------------------------------------------------------
      Check for Additional TFT Semantic Errors
    ---------------------------------------------------------------------*/
    bflow_manager_p = ds_bearer_flow_manager_find_entry(bearer_context_p);
    if(bflow_manager_p)
    {
      if((bflow_manager_p->fi_identifier & 
          (nw_bk->bearer_fltr_id|nw_bk->flow_cntx_fltr_id))
                   == (bflow_manager_p->fi_identifier))
      {
        DS_ERROR_LOG_0("Network deleting all filters,tft is empty");
        if(bearer_context_p->is_default)
        {
          //Deleting all the installed filters is allowed only on the default bearer.
          //In this case this is equivalent to a Delete TFT operation.
          DS_SYSTEM_HEAP_MEM_FREE(nw_bk);
          return ds_pdn_flow_manager_process_delete_tft(pdn_context_p,
                                                        bearer_context_p,
                                                        nw_tft,
                                                        cause_code_p);
        }
        else
        {
          *cause_code_p = DS_BEARER_FLOW_INTERNAL_SEMANTIC_ERR_IN_TFT;
          DS_SYSTEM_HEAP_MEM_FREE(nw_bk);
          return FALSE;
        }
      }
    /*-----------------------------------------------------------------------
      5) TFT operation = "Delete packet filters from existing TFT" with a 
      packet filter list also including packet filters in addition to the
      packet filter identifiers.
   -----------------------------------------------------------------------*/          
      else if(pdn_context_p->nw_mode == SYS_SYS_MODE_LTE &&
     	        nw_tft->tft_total_length != nw_tft->num_filters + 1)
      {
        DS_ERROR_LOG_0("Network TFT IE has packet filters, in addition to "
 	                     "filter identifiers. Syntactical  error");
        *cause_code_p = DS_BEARER_FLOW_INTERNAL_SYNTACTICAL_ERR_IN_TFT;
        DS_SYSTEM_HEAP_MEM_FREE(nw_bk);
        return FALSE;
      }
      else if((nw_bk->non_match_fltr_id > 0)&&
              ((nw_bk->bearer_fltr_id|nw_bk->flow_cntx_fltr_id) > 0))      	 
      {
        DS_MSG0(MSG_LEGACY_HIGH,
                "Network TFT IE has filter other than those installed"
                " on this flow. Ignore");            
      }
    }
    else
    {
      *cause_code_p = DS_BEARER_FLOW_INTERNAL_INSUFFICIENT_RESOURCES;
      DS_SYSTEM_HEAP_MEM_FREE(nw_bk);
      return FALSE;
    }
    /*------------------------------------------------------------------
       Check if NW is deleting non-existing filters.
     -------------------------------------------------------------------*/
    if((nw_bk->bearer_fltr_id == 0) && (nw_bk->flow_cntx_fltr_id == 0))
    {
      DS_ERROR_LOG_0("Network trying to delete non-existing filters");
      DS_SYSTEM_HEAP_MEM_FREE(nw_bk);
      return TRUE;
    }
    /*-------------------------------------------------------------------
      Check if NW is deleting UE requested filters.
    -------------------------------------------------------------------*/
    if(nw_bk->bearer_fltr_id != 0)
    {
      DS_ERROR_LOG_0("Network trying to delete filters set up by UE");      
    }    
    
    /*-------------------------------------------------------------------
       Replace the filters from V4 and V6 flow  with new filters 
    -------------------------------------------------------------------*/        
    if(ds_pdn_flow_manager_delete_filter_from_flow(pdn_context_p,
                                                  bearer_context_p,
                                                  flow_cntxv4,
                                                  nw_tft,
                                                  nw_bk,
                                                  mode_info_p,
                                                  cause_code_p) != TRUE)
    { 
      DS_SYSTEM_HEAP_MEM_FREE(nw_bk);
      return FALSE;
    } 

    if(ds_pdn_flow_manager_delete_filter_from_flow(pdn_context_p,
                                                  bearer_context_p,
                                                  flow_cntxv6,
                                                  nw_tft,
                                                  nw_bk,
                                                  mode_info_p,
                                                  cause_code_p) != TRUE)
    { 
      DS_SYSTEM_HEAP_MEM_FREE(nw_bk);
      return FALSE;
    }
    
    /*-------------------------------------------------------------------
      Resolve any filter conflict.
    ---------------------------------------------------------------------*/
    if(ds_pdn_flow_manager_handle_fltr_conflict(pdn_context_p,
                                                bearer_context_p,
                                                flow_cntxv4,
                                                nw_bk) == FALSE)
    {
      DS_ERROR_LOG_0("unable to handle filter conflict");
      DS_SYSTEM_HEAP_MEM_FREE(nw_bk);
      return FALSE;
    }    

    DS_SYSTEM_HEAP_MEM_FREE(nw_bk);
    return TRUE;
  }
  
  DS_SYSTEM_HEAP_MEM_FREE(nw_bk);
  return FALSE;
} /* ds_pdn_flow_manager_remove_filter_from_tft */

/*===========================================================================
FUNCTION DS_PDN_FLOW_MANAGER_UPDATE_FLOW_IN_TFT

DESCRIPTION
  update the flow values in a TFT.

PARAMETERS
  pdn_context_p         -  PDN information
  bearer_context_p      -  Bearer information
  nw_tft_p              -  TFT information
  mode_info_p           -  Mode specific Qos.
  cause_code_p          -  Cause code 
  check_tft             -  Indicates if the TFT needs to checked.

DEPENDENCIES
  None.

RETURN VALUE
boolean - return DS3G_FAILURE or DS3G_SUCCESS
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_flow_manager_update_flow_in_tft
(
 ds_pdn_mgr_s                                 *pdn_context_p,
 ds_bearer_mgr_s                              *bearer_context_p,
 tft_type_T                                   *nw_tft,
 emm_modify_bearer_context_request_ind_s_type *mode_info_p,
 ds_bearer_flow_internal_cause_code_enum_type *cause_code_p,
 boolean                                       check_tft
)
{
  ds_flow_type                 *flow_cntxv4 = NULL;
  ds_flow_type                 *flow_cntxv6 = NULL;
  ds_bearer_flow_manager_list_type *bflow_list_p = NULL;
  parameter_filter_type_T      *param_type = NULL;
  byte                          param_data = 0x00;
  uint32                        temp_filter_id =0;
  uint16                        filter_id =0;
  int                           cnt = 0;
  int                           inc = 0;
  boolean                       skip_tft = FALSE;
  ds_profile_pdp_type_e         pdp_type;
  ds_bearer_flow_manager_s     *bflow_manager_p = NULL;  
  qos_spec_type                 app_qos = {0};
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(check_tft)
  {
    DS_MSG0(MSG_LEGACY_HIGH, "Processing check tft in updated flow ");
    if(nw_tft->valid == FALSE)
    {
      DS_ERROR_LOG_0("TFT not valid");  
      skip_tft = TRUE;
    }
    else
    {
      if(nw_tft->operation_code != (DS_PDN_TFT_OPCODE_NO_OP))
      {
        *cause_code_p = DS_BEARER_FLOW_INTERNAL_QOS_NOT_ACCEPTED;
        return FALSE;
      }
      skip_tft = FALSE;
    }
  }

  if(!skip_tft)
  {
    /*-----------------------------------------------------------------------
       Check for TFT Syntactic Errors
    -----------------------------------------------------------------------*/
    if(nw_tft->e_bit != DS_FLOW_E_BIT_ENABLE ||
       nw_tft->num_filters != 0)
    {
      DS_ERROR_LOG_0("invalid TFT input");
      *cause_code_p = DS_BEARER_FLOW_INTERNAL_SYNTACTICAL_ERR_IN_TFT;
      return FALSE;
    }
 
    /*-----------------------------------------------------------------------
       Extract the filter identifier information.
    -----------------------------------------------------------------------*/
    for(cnt =0; cnt < MAX_PACKET_FILTERS; cnt++)
    {
      param_type = &(nw_tft->param_list[cnt]);
      inc = 0;
      if(param_type->identifier != (byte)DS_PDN_TFT_PACKET_FILTER_IDENTIFIER)
      {
        DS_ERROR_LOG_1("received unsupported param type:=d",
                       param_type->identifier);
      }
      else
      {
        /* we should receive in byte multiples. */
        if((param_type->content_length == 0) || 
           (param_type->content_length > MAX_FILTER_CONTENT_LENGTH))
        {
          DS_ERROR_LOG_1("received unsupported content len:=d",
                         param_type->content_length);
          *cause_code_p = DS_BEARER_FLOW_INTERNAL_SYNTACTICAL_ERR_IN_TFT;
          return FALSE;
        }
        else
        {
          while(inc < param_type->content_length)
          {
            param_data = param_type->filter_contents[inc];
            filter_id  = (param_data & 0xF);
            inc++;
            if(ds_pdn_tft_is_filter_identifier_requested(
                                     filter_id, &temp_filter_id) == FALSE)
            {
              ds_pdn_tft_set_filter_identifier_bit(filter_id,&temp_filter_id);
            }
            else
            {
              DS_ERROR_LOG_0("Invalid NW input.FI-ID repeated");
              *cause_code_p = DS_BEARER_FLOW_INTERNAL_SYNTACTICAL_ERR_IN_TFT;
              return FALSE;
            }
          }/* while param_type->content_len*/
        }/* else good params */
      }/* else param identifier is FILTER ID*/ 
    }/* for cnt < MAX_PACKET_FILTERS*/
  }/* skip_tft == FALSE */
  
  if( (temp_filter_id == 0) && (!skip_tft) )
  {
    DS_ERROR_LOG_0("Did not receive filter information");
    *cause_code_p = DS_BEARER_FLOW_INTERNAL_SYNTACTICAL_ERR_IN_TFT;
    return FALSE;
  }

  if(!check_tft)
  {    
    flow_cntxv4 = ds_bearer_flow_mgr_find_flow_context(
                               bearer_context_p, DS_FLOW_V4_FLOW_TYPE);
    flow_cntxv6 = ds_bearer_flow_mgr_find_flow_context(
                               bearer_context_p, DS_FLOW_V6_FLOW_TYPE);
    pdp_type = ds_apn_table_get_pdn_pdp_type(pdn_context_p);

    if(((pdp_type == DS_PROFILE_PDP_IPV4 || pdp_type == DS_PROFILE_PDP_IPV4V6) &&
        (flow_cntxv4 == NULL)) ||
       ((pdp_type == DS_PROFILE_PDP_IPV6 || pdp_type == DS_PROFILE_PDP_IPV4V6) &&
        (flow_cntxv6 == NULL)))
    {
      DS_ERROR_LOG_0("No previous NW flow context associated flow manager");
      *cause_code_p = DS_BEARER_FLOW_INTERNAL_QOS_NOT_ACCEPTED;
      return FALSE; 
    }             
  }/* !check_tft */

  /*-------------------------------------------------------------------
    Upating the data rate in the bearer.
  ---------------------------------------------------------------------*/
  bflow_manager_p = ds_bearer_flow_manager_find_entry(bearer_context_p);
  if(!bflow_manager_p)
  {
    return FALSE;
  }

  if(FALSE == ds_bearer_flow_manager_convert_data_rate(bearer_context_p,
                                                       &mode_info_p->sdf_qos,
                                                       &mode_info_p->neg_qos,
                                                       &app_qos,
                                                       cause_code_p))
  {
    DS_ERROR_LOG_0("Data Rate not updated in the flow");
    return FALSE;
  }
  
  /*---------------------------------------------------------------------
    Update all the Flows with the updated flow rate
    Need to optimize for the case, there will be some flows in going
    down state and will not be intereseted in flow rate change.
  ---------------------------------------------------------------------*/          
  bflow_list_p = list_peek_front(&(bflow_manager_p->flow_context));
  while(bflow_list_p != NULL &&
  	     bflow_list_p->flow_context_p != NULL)
  {
    ds_flow_context_set_granted_flow(bflow_list_p->flow_context_p);
    bflow_list_p = list_peek_next(&(bflow_manager_p->flow_context),
                                  &(bflow_list_p->bflow_link));
  }
  
  return TRUE;
}/* ds_pdn_flow_manager_update_flow_in_tft */

/*===========================================================================
FUNCTION DS_PDN_FLOW_MANAGER_PROCESS_TFT_FOR_NW_BEARER_MOD

DESCRIPTION
  Verify parameters in the NW bearer modification request.
  Check if the filter identifier, filter precedence are unique. Update the TFT
  appropriately.

PARAMETERS
  pdn_context_p         -  PDN information
  bearer_context_p      -  Bearer information
  nw_tft_p              -  TFT information
  app_qos_p             -  Qos in App format
  mode_info_p           -  Mode specific Qos.
  cause_code_p          -  Cause code NW TFT is not accurate

DEPENDENCIES
  None.

RETURN VALUE
boolean - return FALSE or TRUE
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_flow_manager_process_tft_for_nw_bearer_mod
(
  ds_pdn_mgr_s                                 *pdn_context_p,
  ds_bearer_mgr_s                              *bearer_context_p,
  tft_type_T                                   *nw_tft_p,
  emm_modify_bearer_context_request_ind_s_type *mode_info_p,
  ds_bearer_flow_internal_cause_code_enum_type *cause_code_p,
  boolean                                       allow_flow_create
)
{

  ds_bearer_flow_manager_s    *bflow_manager_p = NULL;  
  boolean                      rval = TRUE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  if((pdn_context_p == NULL) || (bearer_context_p == NULL) || 
     (nw_tft_p == NULL) || (cause_code_p == NULL))
  {
    if(cause_code_p != NULL)
    {
      *cause_code_p = DS_BEARER_FLOW_INTERNAL_INSUFFICIENT_RESOURCES;
    }
    
    return FALSE;
  }  

  /*-----------------------------------------------------------------------
    Get Bearer Flow Manager
  -----------------------------------------------------------------------*/
  bflow_manager_p = ds_bearer_flow_manager_find_entry(bearer_context_p);
  if(!bflow_manager_p)
  {
    *cause_code_p = DS_BEARER_FLOW_INTERNAL_INSUFFICIENT_RESOURCES;
    return FALSE;
  }
  
  DS_MSG1(MSG_LEGACY_HIGH, "Received op code %d from NW",
          nw_tft_p->operation_code);
  
  switch(nw_tft_p->operation_code)
  {
    case DS_PDN_TFT_OPCODE_CREATE:
      /* This will need to throw away the old tft, delete all the flows.
         and bring up the new flow. */      
      *cause_code_p = DS_BEARER_FLOW_INTERNAL_FEATURE_NOT_SUPPORTED;   
      rval = ds_pdn_flow_manager_add_filter_to_tft( pdn_context_p,
                                                    bearer_context_p,
                                                    nw_tft_p,
                                                    mode_info_p,
                                                    cause_code_p,
                                                    TRUE );
      break;

    case DS_PDN_TFT_OPCODE_DEL_TFT:
      *cause_code_p = DS_BEARER_FLOW_INTERNAL_SEMANTIC_ERR_IN_TFT;      
      rval = ds_pdn_flow_manager_process_delete_tft(pdn_context_p,
                                                    bearer_context_p,
                                                    nw_tft_p,
                                                    cause_code_p);             
      break;

    /* Add the TFT to the flow context */
    case DS_PDN_TFT_OPCODE_ADD:
      if(ds_pdn_flow_manager_add_filter_to_tft(pdn_context_p,
                                               bearer_context_p,
                                               nw_tft_p, 
                                               mode_info_p, 
                                               cause_code_p,
                                               allow_flow_create) == FALSE)
      {
        return FALSE;
      }
      break;
      
    /* Replace given TFT filter set */
    case DS_PDN_TFT_OPCODE_REPLACE:
      if(ds_pdn_flow_manager_replace_filter_in_tft(pdn_context_p,
                                                   bearer_context_p,
                                                   nw_tft_p, 
                                                   mode_info_p,
                                                   cause_code_p) == FALSE)
      {
        return FALSE;
      }        
      break;
      
    /* Delete given TFT filter set */
    case DS_PDN_TFT_OPCODE_DEL_FILTER:
      if(ds_pdn_flow_manager_remove_filter_from_tft(pdn_context_p,
                                                    bearer_context_p,
                                                    nw_tft_p, 
                                                    mode_info_p,
                                                    cause_code_p) == FALSE)
      {
        return FALSE;
      }
      break;

    case DS_PDN_TFT_OPCODE_NO_OP:
         /* Handle no opcode. Check if the E bit is set*/
      if(ds_pdn_flow_manager_update_flow_in_tft(pdn_context_p,
                                                bearer_context_p,
                                                nw_tft_p, 
                                                mode_info_p, 
                                                cause_code_p,
                                                FALSE) == FALSE)
      {
        return FALSE;
      }
      break;
      
   case DS_PDN_TFT_OPCODE_SPARE:
     DS_MSG0(MSG_LEGACY_HIGH, 
             "OP code DS_3GPP_TFT_OPCODE_SPARE, No operation as of now,"
             " returning Failure");
     return FALSE;
     break;
     
   default:
    return rval;
  }/* switch */

  return rval;
} /* ds_pdn_flow_manager_process_tft_for_nw_bearer_mod*/

/*===========================================================================
FUNCTION DS_PDN_FLOW_MANAGER_ACCEPT_MODIFY_ENABLE_FILTER

DESCRIPTION
  This function accepts the filter modification and enable the filtering.
  This happens immediately after filter modify is completed for LTE

PARAMETERS


DEPENDENCIES
  None.

RETURN VALUE
boolean - return TRUE or FALSE
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_flow_manager_accept_modify_enable_filter
(
  ds_bearer_mgr_s             *bearer_context_p
)
{
  ds_bearer_flow_manager_s    *bflow_manager_p = NULL;
  ds_flow_type                *flow_context_p = NULL;
  ds_ipfltr_ctrl_param_type    fltr_ctrl_param = {0};
  int16                        err_no;
  ds_bearer_flow_manager_list_type *bflow_list_p = NULL;
  	
  /*-----------------------------------------------------------------------
    Get Bearer Flow Manager
  -----------------------------------------------------------------------*/
  bflow_manager_p = ds_bearer_flow_manager_find_entry(bearer_context_p);

  if(!bflow_manager_p)
  {
    return FALSE;
  }

  /*---------------------------------------------------------------------
    Find NW FlowContext in the list
  ---------------------------------------------------------------------*/          
  bflow_list_p = list_peek_front(&(bflow_manager_p->flow_context));
  while(bflow_list_p && bflow_list_p->flow_context_p)
  {
    flow_context_p = bflow_list_p->flow_context_p;
    if(flow_context_p->state == DS_FLOW_STATE_PENDING_CONFIRM)
    {
    /*-------------------------------------------------------------------
        Modify Flow Accepted.
      ---------------------------------------------------------------------*/
      if(ds_flow_modify_flow_accepted(flow_context_p) != TRUE)
      {
        DS_ERROR_LOG_0("flow modify result failed");
        return FALSE;
      }
     
    /*-------------------------------------------------------------------
        Updated the Granted Flow in case of Network Initiated Qos Modification scenarios
        ---------------------------------------------------------------------*/
      ds_flow_context_set_granted_flow(flow_context_p);

      if(!ds_pdn_mgr_validate_pdn_ptr(bearer_context_p->pdn_mgr_ptr))
    	{
        DS_ASSERT(0);
        return FALSE;
      }
      
      fltr_ctrl_param.ds_fwk_inst_ptr = ds_fwk_get_inst_ptr_from_index(
                                    bearer_context_p->pdn_mgr_ptr->fw_index);
      fltr_ctrl_param.enable = TRUE;
      fltr_ctrl_param.client_id = FLTR_CLIENT_QOS_OUTPUT;
      fltr_ctrl_param.fi_handle = 
                  flow_context_p->qos_info_ptr->tx.fltr_template.fltr_handle;
       
      if(ds_ipfltr_control(&fltr_ctrl_param, &err_no) == -1)
      {
        DS_ERROR_LOG_0("flow filtering control failed");
      }

      flow_context_p->state = DS_FLOW_STATE_UP; 
    }
    
    bflow_list_p = list_peek_next(&(bflow_manager_p->flow_context),
                                  &(bflow_list_p->bflow_link));
  } 

  return TRUE;
}/* ds_pdn_flow_manager_accept_modify_enable_filter */

/*===========================================================================
FUNCTION DS_PDN_FLOW_MANAGER_BEARER_MODIFICATION_IND

DESCRIPTION
  Handle the bearer modification Indication.

PARAMETERS
  mode_info_ptr   - NAS passed mode information.

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
===========================================================================*/
void ds_pdn_flow_manager_bearer_modification_ind
(
  emm_modify_bearer_context_request_ind_s_type   *info_ptr
)
{
  ds_pdn_mgr_s                 *pdn_mgr_p = NULL;
  ds_bearer_mgr_s              *bearer_mgr_p = NULL;
  ds_bearer_flow_manager_s     *bflow_manager_p = NULL;
  ds_flow_type                 *flow_context_p = NULL;
  ds_bearer_flow_internal_cause_code_enum_type
                                icause_code = DS_BEARER_FLOW_INTERNAL_CC_INVALID;
  emm_modify_bearer_context_accept_T
  	                             accept_response = {{0}};
  emm_modify_bearer_context_rej_T
  	                             reject_response = {{0}};
  qos_spec_type                 app_qos = {0};
  byte                         *apn = NULL;
  ds_eps_bearer_mgr_flow_dir_e  bearer_flow_dir;
  ds_eps_bearer_mgr_flow_dir_e  modified_bearer_flow_dir;
  uint32                        rat_mask = DS_3GPP_THROT_RAT_MASK_GLOBAL;
                                         /* RATs on which throttling needs to
                                                                               be unblocked */  
  boolean                       return_value = TRUE;
  ds_local_err_val_e_type       local_err = DS_LOCAL_ERR_VAL_NONE;
  ds_bearer_flow_manager_list_type *bflow_list_p = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  if(info_ptr == NULL)
  {
    DS_ASSERT(0);
    return;
  }

  bearer_mgr_p = ds_bearer_mgr_get_bearer_from_eps_id(info_ptr->eps_bearer_id);
  if(!ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p))
  {
    local_err = DS_LOCAL_ERR_VAL_1;
    goto func_exit;
  }

  pdn_mgr_p = bearer_mgr_p->pdn_mgr_ptr;
  if(!ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p))
  {
    local_err = DS_LOCAL_ERR_VAL_2;
    goto func_exit;
  }

  if(pdn_mgr_p->nw_mode != SYS_SYS_MODE_LTE)
  {
    local_err = DS_LOCAL_ERR_VAL_3;
    goto func_exit;
  }

  DS_MSG1(MSG_LEGACY_HIGH, "Bearer modifcation ind for bearer id %d",
          bearer_mgr_p->index);

  /*   Caching the previous Bearer Flow Direction    */
  bearer_flow_dir = ds_eps_bearer_mgr_get_flow_direction(bearer_mgr_p);

  // Later, check if user initiated modify needs to be handled for delete flow, with SDF ID

  if(pdn_mgr_p->state == DS_PDN_MGR_STATE_GOING_DOWN)
  {
    local_err = DS_LOCAL_ERR_VAL_4;
    reject_response.esm_cause.valid = TRUE;
    reject_response.esm_cause.esm_cause = NAS_ESM_INSUFFICIENT_RESOURCES;
    goto func_exit;
  }

  // Dedicated bearer modification handling
  if( (info_ptr->sdf_qos.valid != TRUE) &&
      (info_ptr->ul_dl_tft.valid != TRUE) &&
      (info_ptr->apn_ambr.valid != TRUE) )
  {
    DS_MSG0(MSG_LEGACY_HIGH, "QOS/TFT/AMBR not specified for bearer");
    goto success_path;
  }

  // Invalid TFT case
  if(info_ptr->ul_dl_tft.valid != TRUE)
  {
    DS_MSG0(MSG_LEGACY_HIGH,"TFT not valid.");
    
    if(info_ptr->apn_ambr.valid == TRUE)
    {
      if(ds_pdn_flow_manager_update_apn_ambr(&info_ptr->apn_ambr, 
                                             pdn_mgr_p) == FALSE)
      {
        local_err = DS_LOCAL_ERR_VAL_9;
        reject_response.esm_cause.valid = TRUE;
        reject_response.esm_cause.esm_cause = NAS_ESM_INSUFFICIENT_RESOURCES;
        goto func_exit;
      }
    }

    if(info_ptr->sdf_qos.valid == TRUE)
    {
      DS_MSG0(MSG_LEGACY_HIGH,"QoS without TFT for dedicated bearer.");

      
      DS_SYSTEM_HEAP_MEM_ALLOC(app_qos.rx.fltr_template.list_ptr,
                               (MAX_PACKET_FILTERS*sizeof(ip_filter_type)),
                               ip_filter_type*);
      if(app_qos.rx.fltr_template.list_ptr == NULL)
      {
        local_err = DS_LOCAL_ERR_VAL_10;
        goto func_exit;
      }
      memset(app_qos.rx.fltr_template.list_ptr, 0, 
             (MAX_PACKET_FILTERS*sizeof(ip_filter_type)));
      
      DS_SYSTEM_HEAP_MEM_ALLOC(app_qos.tx.fltr_template.list_ptr,
                               (MAX_PACKET_FILTERS*sizeof(ip_filter_type)),
                               ip_filter_type*);            
      if(app_qos.tx.fltr_template.list_ptr == NULL)
      {
        local_err = DS_LOCAL_ERR_VAL_11;
        goto func_exit;
      }    
      memset(app_qos.tx.fltr_template.list_ptr, 0, 
             (MAX_PACKET_FILTERS*sizeof(ip_filter_type)));

      bflow_manager_p = ds_bearer_flow_manager_find_entry(bearer_mgr_p);
      if(!bflow_manager_p)
      {
        local_err = DS_LOCAL_ERR_VAL_5;
        goto func_exit;
      }
    
      bflow_list_p = list_peek_front(&bflow_manager_p->flow_context);
      if(!bflow_list_p || !bflow_list_p->flow_context_p)
      {
        local_err = DS_LOCAL_ERR_VAL_6;
        goto func_exit;
      }

      if(ds_bearer_flow_manager_convert_data_rate(bearer_mgr_p,
                                                  &info_ptr->sdf_qos,
                                                  &info_ptr->neg_qos,
                                                  &app_qos,
                                                  &icause_code) == FALSE)
      {
        local_err = DS_LOCAL_ERR_VAL_7;
        ds_bearer_flow_manager_convert_cause_code(&reject_response.esm_cause,
                                                  &icause_code);
        goto func_exit;
      }      

      ds_eps_bearer_mgr_send_qos_indication(bearer_mgr_p);

      while(bflow_list_p != NULL && bflow_list_p->flow_context_p != NULL)
      {        
        flow_context_p = bflow_list_p->flow_context_p;
        ds_flow_context_set_granted_flow(flow_context_p);
        
        if(ds_flow_context_get_filter_spec(flow_context_p, 
                                           FALSE, 
                                           &app_qos) == FALSE)
        {
          local_err = DS_LOCAL_ERR_VAL_12;
          goto func_exit;
        }
        
        if(app_qos.rx.fltr_template.num_filters > 0 ||
           app_qos.tx.fltr_template.num_filters > 0)
        {
          if(ds_flow_modify_flow_context(flow_context_p, &app_qos) == TRUE)
          {
            if(ds_flow_modify_flow_accepted(flow_context_p) == FALSE)
            {
              DS_ERROR_LOG_0("Flow modify accepted failed");
            }
          }          
        }

        bflow_list_p = list_peek_next(&bflow_manager_p->flow_context,
                                      &bflow_list_p->bflow_link);
      }
    }

    goto success_path;
  }/* TFT not valid */

  // Dedicated bearer modification with TFT
  if(info_ptr->sdf_qos.valid == FALSE)
  {
    return_value = ds_pdn_flow_manager_process_tft_for_nw_bearer_mod(
                                  pdn_mgr_p,
                                  bearer_mgr_p,
                                  &info_ptr->ul_dl_tft,
                                  NULL,
                                  &icause_code,
                                  TRUE);
  }
  else
  {
    return_value = ds_pdn_flow_manager_process_tft_for_nw_bearer_mod(
                                  pdn_mgr_p,
                                  bearer_mgr_p,
                                  &info_ptr->ul_dl_tft,
                                  info_ptr,
                                  &icause_code,
                                  TRUE);
  }

  if(return_value == FALSE)
  {
    ds_bearer_flow_manager_convert_cause_code(&reject_response.esm_cause,
                                              &icause_code);
  }

  /*-------------------------------------------------------------------------
      Update the APN AMBR if NW passed this value. 
    -------------------------------------------------------------------------*/
  if(return_value == TRUE)
  {
    if (bearer_mgr_p != NULL)
    {
      ds_eps_bearer_mgr_send_qos_indication(bearer_mgr_p);
    }

    if (info_ptr->apn_ambr.valid == TRUE)
    {
      /*--------------------------------------------------------------------
       Store the APN AMBR in Mode specific context
      ---------------------------------------------------------------------*/
      ds_pdn_flow_manager_update_apn_ambr(&info_ptr->apn_ambr, pdn_mgr_p); 
    } /* if apn_ambr is valid */
  }

  ds_pdn_flow_manager_accept_modify_enable_filter(bearer_mgr_p);

success_path:

#ifdef FEATURE_DATA_REL10
  if(ds_pdn_nv_manager_get_nv_lte_rel_10())
  {
   /*---------------------------------------------------------------------
    EPS Bearer context modification got successfull processed by UE.
    Clear APN throttling state machine if it was globally blocked.
    ----------------------------------------------------------------------*/
    apn = (byte*)pdn_mgr_p->pdp_profile->context.apn;
    
    DS_MSG_SPRINTF_1(MSG_LEGACY_HIGH,
                     "Trying to unblock potentially blocked APN: %s",
                     (char*)apn);
    /*---------------------------------------------------------------------
      Unblock the APN on Global RAT and also LTE RAT if throttling per RAT is
      enabled
    ----------------------------------------------------------------------*/
    if (ds_pdn_nv_manager_get_enable_pdn_throt_per_rat()||
        ds_pdn_nv_manager_is_rel_12_or_above_supported())
    {
      rat_mask |= DS_3GPP_THROT_RAT_MASK_LTE;
    }

    ds_3gpp_pdn_throt_handle_nw_inited_unblock_apn(apn, rat_mask);

  } //get_nv_lte_rel_10()
#endif /*FEATURE_DATA_REL10*/

  bearer_mgr_p->hc_config = info_ptr->hc_config;

  ds_pdn_cp_data_process_bearer_modify_req(bearer_mgr_p->index);

  /*   Caching the new Bearer Flow Direction    */
  modified_bearer_flow_dir = ds_eps_bearer_mgr_get_flow_direction(bearer_mgr_p);

  if(modified_bearer_flow_dir != bearer_flow_dir)
  {
    DS_MSG2(MSG_LEGACY_HIGH, "Modify Ind, flow_dir: %d modified_flow_dir: %d",
            bearer_flow_dir, modified_bearer_flow_dir);
    ds_eps_bearer_mgr_re_register_wm_with_pdcp(bearer_mgr_p,
                                               bearer_flow_dir,
                                               modified_bearer_flow_dir);
  }

func_exit:
	
  // Accept response
  if(return_value == TRUE &&
     local_err == DS_LOCAL_ERR_VAL_NONE)
  {
    ds_pdn_mgr_update_pco(bearer_mgr_p,
                          &info_ptr->protocol_config_options,
                          &info_ptr->ext_protocol_conf_opt);

    apn = (byte*)pdn_mgr_p->pdp_profile->context.apn;
    if (strlen((const char*)(apn)) == 0)
    {
      ds_apn_table_get_resolved_apn_name(pdn_mgr_p,(char**)&apn);
    }
    
    ds_pdn_apn_rate_ctrl_assign_apn_index_to_bearer(apn);

    DS_MSG0(MSG_LEGACY_HIGH, "Bearer modification accepted !");
    
    accept_response.esm_hdr = info_ptr->esm_hdr;
    accept_response.eps_bearer_id = info_ptr->eps_bearer_id;

    accept_response.message_header.message_id = EMM_MODIFY_BEARER_CONTEXT_ACCEPT;
    accept_response.message_header.message_set = MS_MM_DS;      
    accept_response.message_header.message_len_lsb =
      (sizeof( emm_modify_bearer_context_accept_T ) - sizeof(IMH_T)) % 256;
    accept_response.message_header.message_len_msb =
      (sizeof( emm_modify_bearer_context_accept_T ) - sizeof(IMH_T)) / 256;
    
    ds_pdn_mgr_ds_to_nas_send_msg((void*)&accept_response); 
    return;
  }

  if(local_err != DS_LOCAL_ERR_VAL_NONE)
  {
    DS_ERROR_LOG_1("Bearer modification indication failed with err =d",
                   local_err);
  }

  // Reject response
  DS_MSG0(MSG_LEGACY_HIGH, "Bearer modification rejected !");
  
  reject_response.esm_hdr = info_ptr->esm_hdr;
  reject_response.eps_bearer_id = info_ptr->eps_bearer_id;
  reject_response.esm_local_cause.valid = FALSE;

  reject_response.message_header.message_id = EMM_MODIFY_BEARER_CONTEXT_REJ;
  reject_response.message_header.message_set = MS_MM_DS;      
  reject_response.message_header.message_len_lsb =
    (sizeof( emm_modify_bearer_context_rej_T ) - sizeof(IMH_T)) % 256;
  reject_response.message_header.message_len_msb =
    (sizeof( emm_modify_bearer_context_rej_T ) - sizeof(IMH_T)) / 256;
  
  ds_pdn_mgr_ds_to_nas_send_msg((void*)&reject_response); 
  return;
}/* ds_pdn_flow_manager_bearer_allocation_ind */

/*===========================================================================
FUNCTION DS_PDN_FLOW_MANAGER_PROCESS_FILTER_ID_CONFLICT

DESCRIPTION
  Handle the filter ID conflict.

PARAMETERS
  info_ptr   - Flow command passed information.

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
===========================================================================*/
void ds_pdn_flow_manager_process_filter_id_conflict
(
  ds_pdn_flow_manager_cmd_info_type   *cmd_info_ptr
)
{
  ds_flow_type      *flow_context_p = NULL;
  tft_type_T         nw_tft_p = {0};

  flow_context_p = (ds_flow_type*)(cmd_info_ptr->flow_context_p);
  if(flow_context_p)
  {
    flow_context_p->ignore_fi_id = cmd_info_ptr->fltr_id_conflict;

    DS_MSG1(MSG_LEGACY_HIGH, 
            "Handle Filter ID conflict for flow 0x:%x", flow_context_p);

    if(flow_context_p->state == DS_FLOW_STATE_DOWN)
    {
      DS_ERROR_LOG_0("Flow is already down");
      return;
    }

    ds_flow_go_null_ind(flow_context_p);

    ds_pdn_flow_manager_bearer_modify_req(flow_context_p, &nw_tft_p);    
  }  

  return;
}/* ds_pdn_flow_manager_process_filter_id_conflict */

/*===========================================================================
FUNCTION DS_PDN_FLOW_MANAGER_HANDLE_FILTER_PRECED_CONFLICT

DESCRIPTION
  Handle the filter precedence conflict.

PARAMETERS
  info_ptr   - Flow command passed information.

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
===========================================================================*/
void ds_pdn_flow_manager_process_filter_preced_conflict
(
  ds_pdn_flow_manager_cmd_info_type   *cmd_info_ptr
)
{
  if(cmd_info_ptr == NULL ||
     !ds_bearer_mgr_validate_bearer_ptr(cmd_info_ptr->bearer_context_p))
  {
    return;
  }

  DS_MSG1(MSG_LEGACY_HIGH,
          "Processing preced conflict on bearer %d",
          cmd_info_ptr->bearer_context_p->index);
  
  ds_pdn_flow_manager_teardown_bearer(cmd_info_ptr->bearer_context_p,
                                      cmd_info_ptr->preced_conflict);   
  return;
}

/*===========================================================================
FUNCTION DS_PDN_FLOW_MANAGER_INST_DOWN_HDLR

DESCRIPTION
  The instance has gone out of use. clean up any pending flow contexts and
  update the TFT information to inform network about filter cleanup.

PARAMETERS
  pdn_context_p     -  Pointer to PDN context.
  inst_p                  -  Instance pointer

DEPENDENCIES
  None.

RETURN VALUE
  None
  
SIDE EFFECTS
  None.

===========================================================================*/
/*lint -save -e655 Bitwise operation on compatible enums */
void ds_pdn_flow_manager_inst_down_hdlr
(
  ds_pdn_mgr_s               *pdn_context_p,
  ds_pdn_mgr_inst_type_s     *inst_p
)
{
  ds_pdn_flow_manager_s   *flow_manager_p = NULL;
  ds_bearer_mgr_s         *bearer_context_p = NULL;
  int                      cnt = 0;
  uint8                    flow_type = 0;
  /*-------------------------------------------------------------------------*/

  if( !ds_pdn_mgr_validate_pdn_ptr(pdn_context_p) || !inst_p)
  {
    return;
  }
  
  flow_manager_p = ds_pdn_flow_mgr_get_entry(pdn_context_p->index);
  if(flow_manager_p == NULL)
  {
    return;
  }

  if(pdn_context_p->num_active_bearer_context <= 1)
  {
    DS_MSG0(MSG_LEGACY_HIGH, "No dedicated bearers associated with PDN");
    return;
  }
    
  if(inst_p->type == DS_PDN_INSTANCE_IPV4)
  {
    flow_type = DS_FLOW_V4_FLOW_TYPE;
  }
  else if(inst_p->type == DS_PDN_INSTANCE_IPV6)
  {
    flow_type  = DS_FLOW_V6_FLOW_TYPE;
  }
  else
  {
    return;
  }
  
  for(cnt=0; cnt<DS_3GPP_MAX_BEARER_CONTEXT_PER_PDN; cnt++)
  {
    bearer_context_p = pdn_context_p->bearer_ptr_tbl[cnt];
    if(ds_bearer_mgr_validate_bearer_ptr(bearer_context_p))
    {
      if(cnt != DS_3GPP_DEFAULT_BEARER_INDEX)
      {
        ds_bearer_flow_manager_inst_down_hdlr(pdn_context_p,
                                              bearer_context_p,
                                              flow_type);
      }/* not the default bearer */
    }/* if bearer pointer != NULL */
  }/* for bearer cnt */
  
  return;
} /* ds_pdn_flow_manager_inst_down_hdlr */


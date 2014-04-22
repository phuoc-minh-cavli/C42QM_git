/*===========================================================================
                      DS_BEARER_MANAGER.C
DESCRIPTION
This file defines Bearer manager APIs.

Copyright (c) 2018 - 2020 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/3gpp/pdnmgr/src/ds_bearer_manager.c#18 $
  $DateTime: 2023/03/23 03:53:38 $$Author: pwbldsvc $

when           who    what, where, why
--------    ---    ----------------------------------------------------------
08/09/17    sd      First checkin
===========================================================================*/

#include "ds_pdn_defs.h"
#include "ds_pdn_manager.h"
#include "ds_pdn_nv_manager.h"
#include "ds_bearer_manager.h"
#include "ds_eps_pdn_manager.h"
#include "ds_bearer_flow_manager.h"
#include "ds_gprs_bearer_manager.h"
#include "ds_pdn_cp_data_hdlr.h"
#include "ds_pdn_hdlr.h"
#include "secapi.h"
#include "ran.h"
#include "ds_3gpp_srv_req_throt_sm.h"
#include "ds_pdn_flow_manager.h"
#include "ds_fwki.h"
#include "ds_3gpp_rab_reestab_sm_hdlr.h"
#ifdef FEATURE_MODE_TLB
#include "ds_eps_tlb.h"
#endif /* FEATURE_MODE_TLB */
#include "ds_pdn_psm_hdlr.h"

static ds_bearer_mgr_s *ds_bearer_manager[DS_3GPP_MAX_BEARER_CONTEXT] = {NULL};

static ds_bearer_mgr_s *ds_bearer_manager_cache[DS_3GPP_MAX_BEARER_CONTEXT] = 
                                                                        {NULL};

// System level bearer manager info
static ds_bearer_mgr_info_type *ds_bearer_info = NULL;

#define DS_BEARER_SMALL_ITEM_POOL_MASK             (0x1)
#define DS_BEARER_LARGE_ITEM_POOL_MASK             (0x2)
#define DS_BEARER_DUP_ITEM_POOL_MASK               (0x4)

#ifdef TEST_FRAMEWORK
#error code not present
#endif
/*===========================================================================
FUNCTION DS_BEARER_MGR_INIT

DESCRIPTION
  This function finitialises Bearer Manager module.
 
PARAMETERS
  is_init          : Is it part of power up init

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void ds_bearer_mgr_init
(
  boolean is_init
)
{  
  char rab_retry_timer_path[] = 
               "/nv/item_files/data/3gpp/access_barring_rab_retry_timer";
  char rab_retry_counter_path[] = 
               "/nv/item_files/data/3gpp/access_barring_rab_retry_counter"; 

  if(is_init)
  {
    DS_SYSTEM_HEAP_MEM_ALLOC(ds_bearer_info,
  	                        sizeof(ds_bearer_mgr_info_type),
  	                        ds_bearer_mgr_info_type*);
    if(ds_bearer_info != NULL)
    {
      memset(ds_bearer_info,0,sizeof(ds_bearer_mgr_info_type));
    }
  }
  if(ds_bearer_info == NULL)
  {
    DS_ASSERT(0);
    return;
  }
  

  if(ds_pdn_nv_manager_read_efs_nv(rab_retry_timer_path,
                                &ds_bearer_info->access_barring_rab_retry_timer,
                                   sizeof(uint16)) != NV_DONE_S)
  {
    ds_bearer_info->access_barring_rab_retry_timer = 
                                   DS_BEARER_MGR_RAB_REESTABLISH_MIN_RETRY_TIME;
  }
  else
  {
    if(ds_bearer_info->access_barring_rab_retry_timer < 
                                   DS_BEARER_MGR_RAB_REESTABLISH_MIN_RETRY_TIME)
    {
      ds_bearer_info->access_barring_rab_retry_timer = 
                                   DS_BEARER_MGR_RAB_REESTABLISH_MIN_RETRY_TIME;
    }
  }
   
  if(ds_pdn_nv_manager_read_efs_nv(rab_retry_counter_path,
                               &ds_bearer_info->access_barring_rab_retry_counter,
                                   sizeof(uint16)) != NV_DONE_S)
  {
    ds_bearer_info->access_barring_rab_retry_counter = 
                                   DS_BEARER_MGR_ACCESS_BARR_RAB_RETRY_COUNTER;
  }

  if(is_init)
  {
    ds_bearer_info->tmr_rab_reestab = 
  	              ps_timer_alloc(ds_bearer_mgr_reorig_dorm_timer_cb,   0);
    rex_init_crit_sect(&ds_bearer_info->ul_data_crit_sect);
  }
  if (PS_TIMER_INVALID_HANDLE == ds_bearer_info->tmr_rab_reestab)
  {
    DS_ERROR_LOG_0("Invalid PS timer handle");
  }

  ds_eps_bearer_mgr_init();
#ifdef FEATURE_MODE_TLB
  dsEpsTlb_init();
#endif /* FEATURE_MODE_TLB */
#if 0
#ifdef FEATURE_DATA_LTE  
    dsmsgrrcv_reg_hdlr(  
      LTE_RRC_DEACTIVATE_REQ,
      (dsmsgrcv_msg_hdlr_f)ds_bearer_cntxt_lte_deactivate_req_hdlr
    );
#endif /* FEATURE_DATA_LTE */
#endif

  return;  
} /* ds_bearer_mgr_init */

/*===========================================================================
FUNCTION DS_BEARER_MGR_GET_INSTANCE_PTR

DESCRIPTION
  This function fetches the Bearer Manager Pointer corresponding to the
  given Bearer Instance.
 
PARAMETERS
  Bearer Instance

DEPENDENCIES
  None.

RETURN VALUE
  Bearer Manager Pointer.

SIDE EFFECTS
  None.
===========================================================================*/
ds_bearer_mgr_s* ds_bearer_mgr_get_instance_ptr
(
  uint8 instance
)
{
  return ((instance < DS_3GPP_MAX_BEARER_CONTEXT) ? 
            ds_bearer_manager[instance] : NULL);
} /* ds_bearer_mgr_get_instance_ptr */

/*===========================================================================
FUNCTION DS_BEARER_MGR_GET_CAHCED_INSTANCE_PTR

DESCRIPTION
  This function fetches the cahced Bearer Manager Pointer corresponding to the
  given Bearer Instance.
 
PARAMETERS
  Bearer Instance

DEPENDENCIES
  None.

RETURN VALUE
  Bearer Manager Pointer.

SIDE EFFECTS
  None.
===========================================================================*/
ds_bearer_mgr_s* ds_bearer_mgr_get_cached_instance_ptr
(
  uint8 instance
)
{
  return ((instance < DS_3GPP_MAX_BEARER_CONTEXT) ? 
            ds_bearer_manager_cache[instance] : NULL);
} /* ds_bearer_mgr_get_cached_instance_ptr */

/*===========================================================================
FUNCTION DS_BEARER_MGR_VALIDATE_BEARER_PTR

DESCRIPTION
  This function checks if the bearer ptr has been allocated dynamic memory

PARAMETERS
  bearer_mgr_p to be validated
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - if valid
  FALSE - otherwise

SIDE EFFECTS
  None
===========================================================================*/
boolean ds_bearer_mgr_validate_bearer_ptr
(
  ds_bearer_mgr_s *bearer_mgr_p
)
{
  if ( bearer_mgr_p != NULL && 
       bearer_mgr_p->cookie == BEARER_MGR_VALID_COOKIE )
  {
    return TRUE;
  }

  return FALSE;
} /*ds_bearer_mgr_validate_bearer_ptr*/

/*===========================================================================
FUNCTION ds_bearer_mgr_free_dyn_mem

DESCRIPTION
  This function frees dynamic bearer context

PARAMETERS
  bearer_p       - bearer reference to be freed
    
DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None
===========================================================================*/
void ds_bearer_mgr_free_dyn_mem
(
  ds_bearer_mgr_s *bearer_p
)
{
  boolean cache_bearer_ptr = FALSE;
  
  if(bearer_p)
  {    
    DS_MSG1(MSG_LEGACY_HIGH, "Freeing dyn allocated bearer %d", bearer_p->index);

    rex_enter_crit_sect(&ds_bearer_info->ul_data_crit_sect);

    if(bearer_p->eps_bearer)
    {
      /* If WWAN tech is suspended and we are registered with PDCP, cache 
         the bearer pointer to dereg after tech resume */
      if(ds_pdn_mgr_is_wwan_tech_suspended() == TRUE &&
         bearer_p->eps_bearer->is_registered_pdcp == TRUE)
      {
        DS_MSG0(MSG_LEGACY_HIGH, "Caching the bearer during tech suspend state");
        cache_bearer_ptr = TRUE;
      }

      DS_SYSTEM_HEAP_MEM_FREE(bearer_p->eps_bearer);
      ds_pdn_cp_data_free_bearer_info(bearer_p->index);
    }
    
    if(bearer_p->gprs_bearer)
    {
      DS_SYSTEM_HEAP_MEM_FREE(bearer_p->gprs_bearer);
    }

    // Free BP timer
    if(bearer_p->tmr_hndl_dyn_back_pressure != PS_TIMER_INVALID_HANDLE)
    {
      PS_TIMER_FREE_HANDLE(bearer_p->tmr_hndl_dyn_back_pressure);
    }

    ds_bearer_manager[bearer_p->index] = NULL;

    if(cache_bearer_ptr == TRUE)
    {
      // This cached bearer will be cleaned up after tech resume
      ds_bearer_manager_cache[bearer_p->index] = bearer_p;
    }
    else
    {      
      // clean up tx and rx WM
      dsm_queue_destroy(&(bearer_p->tx_wm.wm_item));      
      dsm_queue_destroy(&(bearer_p->rx_wm.wm_item));

      // Reset the cookie
      bearer_p->cookie = 0;
    
      DS_SYSTEM_HEAP_MEM_FREE(bearer_p);
    }
    
    rex_leave_crit_sect(&ds_bearer_info->ul_data_crit_sect);        
  }
}


/*===========================================================================
FUNCTION DS_BEARER_MGR_ALLOCATE_NEW_BEARER

DESCRIPTION
  This function is used to return a bearer from the pool of available bearer
  contexts.

PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE
  ds_bearer_mgr_s* - Bearer ptr

SIDE EFFECTS
  None.
===========================================================================*/
ds_bearer_mgr_s* ds_bearer_mgr_allocate_new_bearer
(
  void
)
{
  uint8                loop_index;
  ds_bearer_mgr_s*     bearer_context_p = NULL;
  sys_sys_mode_e_type  mode = ds_pdn_hdlr_get_current_network_mode();
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for (loop_index = 0; loop_index < DS_3GPP_MAX_BEARER_CONTEXT; loop_index++)
  {
    bearer_context_p = ds_bearer_manager[loop_index];

    if(!ds_bearer_mgr_validate_bearer_ptr(bearer_context_p))
    {
      DS_SYSTEM_HEAP_MEM_ALLOC(bearer_context_p,sizeof(ds_bearer_mgr_s),
	  	                        ds_bearer_mgr_s*);
      ds_bearer_manager[loop_index] = bearer_context_p;
	  
      if (bearer_context_p == NULL)
      {
        DS_ERROR_LOG_0("Heap mem allocation failure");
        return NULL;
      }
      
      memset(bearer_context_p,0,sizeof(ds_bearer_mgr_s));
      bearer_context_p->cookie = BEARER_MGR_VALID_COOKIE;
      bearer_context_p->index = loop_index;
#ifdef FEATURE_MODE_TLB
      bearer_context_p->rx_sig = (ps_sig_enum_type)
             ((uint32)PS_3GPP_UM_RX_Q_SIGNAL_0 + (loop_index));
#endif /* FEATURE_MODE_TLB */
      
      DS_MSG1(MSG_LEGACY_HIGH,
      	       "Bearer Context dyn memory allocated at index %d", 
      	       loop_index);

      if(mode == SYS_SYS_MODE_LTE)
      {
        DS_SYSTEM_HEAP_MEM_ALLOC(bearer_context_p->eps_bearer,
			                     sizeof(ds_eps_bearer_manager_s),
			                     ds_eps_bearer_manager_s*);
		
        if(bearer_context_p->eps_bearer == NULL)
        {
          DS_ERROR_LOG_0("Heap mem allocation failure");
          ds_bearer_mgr_free_dyn_mem(bearer_context_p);
          return NULL;
        }

        memset(bearer_context_p->eps_bearer, 0,
        	      sizeof(ds_eps_bearer_manager_s));

        ds_pdn_cp_data_alloc_bearer_info(bearer_context_p);
      }
      else if(mode == SYS_SYS_MODE_GSM)
      {
        DS_SYSTEM_HEAP_MEM_ALLOC(bearer_context_p->gprs_bearer,
			                     sizeof(ds_gprs_bearer_manager_s),
			                     ds_gprs_bearer_manager_s*);
        if(bearer_context_p->gprs_bearer == NULL)
        {
          DS_ERROR_LOG_0("Heap mem allocation failure");
          ds_bearer_mgr_free_dyn_mem(bearer_context_p);
          return NULL;
        }

        memset(bearer_context_p->gprs_bearer, 0,
        	      sizeof(ds_gprs_bearer_manager_s));        
      }
           
      /* Initialize SDF ID to be invalid */
      bearer_context_p->sdf_id = 0xFFFF;

      /* Initialize apn index for APN Rate Control context to -1 */
      bearer_context_p->apn_index = -1;
     
      bearer_context_p->tmr_hndl_dyn_back_pressure = PS_TIMER_INVALID_HANDLE;
     
// Do data path inits here including WM
      dsm_queue_init(&(bearer_context_p->tx_wm.wm_item),
                     0xFFFFFFFF,  /* No MAX queue size */
                     &(bearer_context_p->tx_wm.wm_queue));
      
      dsm_queue_init(&(bearer_context_p->rx_wm.wm_item),
                     0xFFFFFFFF,  /* No MAX queue size */
                     &(bearer_context_p->rx_wm.wm_queue));

      bearer_context_p->tx_ptr = NULL;

      break;
   	}
  }

  if (loop_index == DS_3GPP_MAX_BEARER_CONTEXT)
  {
    DS_ERROR_LOG_0("MAX Bearer contexts have been allocated");
    bearer_context_p = NULL;
  }

  return bearer_context_p;
} /* ds_bearer_cntxt_allocate_new_bearer */

/*===========================================================================
FUNCTION  DS_BEARER_MGR_BRING_UP_BEARER

DESCRIPTION
  This function handles both the reorigination from dormancy as well as new
  call bring up. 

PARAMETERS
  Bearer ptr

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_bearer_mgr_bring_up_bearer
(
  ds_bearer_mgr_s                 *bearer_mgr_p
)
{
  ds_pdn_mgr_s              *pdn_mgr_p;
  boolean                    ret_val = FALSE;
  ds_bearer_flow_manager_s  *bflow_p = NULL;
  ds_bearer_flow_manager_list_type  *bflow_list_p = NULL;  // this will change
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Get the PDN context */
  pdn_mgr_p = bearer_mgr_p->pdn_mgr_ptr;

  DS_MSG2(MSG_LEGACY_HIGH,
  	       "Bearer bring up. Bearer state: %d, Bearer instance %d",
          bearer_mgr_p->state, bearer_mgr_p->index);
  
  if( bearer_mgr_p->state == DS_BEARER_MGR_STATE_UP_DORMANT_REESTAB ||
      bearer_mgr_p->state == DS_BEARER_MGR_STATE_PEND_RAB_REG ||
      bearer_mgr_p->state == DS_BEARER_MGR_STATE_UP_DORMANT ) 
  {
    return TRUE; 
  }

  /* It could be that bearer UP can happen during call cleanup and may lead to
        New PDN Request which is not the expectation */        
  if( bearer_mgr_p->state == DS_BEARER_MGR_STATE_GOING_DOWN ) 
  {
    return TRUE;
  }
  
  if( bearer_mgr_p->state == DS_BEARER_MGR_STATE_DOWN ||
      bearer_mgr_p->state == DS_BEARER_MGR_STATE_PEND_REDIAL ) 
  {
    if(pdn_mgr_p->nw_mode == SYS_SYS_MODE_LTE)
    {
      ret_val = ds_eps_bearer_mgr_bring_up_pdn(bearer_mgr_p);
    }
    else if(pdn_mgr_p->nw_mode == SYS_SYS_MODE_GSM)
    {
      ret_val = ds_gprs_bearer_mgr_bring_up_pdp(bearer_mgr_p);
    }

    if(!ret_val)
    {      
      DS_ERROR_LOG_0("Error bringing up Bearer");

      if(bearer_mgr_p->is_default)
      {
        ds_flow_unbind_default_flows(pdn_mgr_p);
        bearer_mgr_p->state = DS_BEARER_MGR_STATE_DOWN;
      }
      else
      {
        bflow_p = ds_bearer_flow_manager_find_entry(bearer_mgr_p);
        if(bflow_p)
        {
          bflow_list_p = list_peek_front(&bflow_p->flow_context);
          if(bflow_list_p && bflow_list_p->flow_context_p)
          {
            ds_pdn_flow_manager_update_precedence_bit_mask(
                        pdn_mgr_p, bflow_list_p->flow_context_p, FALSE, FALSE, TRUE);

            ds_flow_release_flow_context(bflow_list_p->flow_context_p);
          }
        }
      }

      ds_pdn_mgr_bearer_down_ind(pdn_mgr_p, bearer_mgr_p, 
      	                          DS_NET_DOWN_REASON_NOT_SPECIFIED);
    }
    else
    {
      bearer_mgr_p->state = DS_BEARER_MGR_STATE_COMING_UP;
    }
  }

  return ret_val;
}


#ifdef FEATURE_DATA_IMS
/*===========================================================================
FUNCTION DS_BEARER_MGR_IS_IMCN_CALL_UP

DESCRIPTION
  This fn is used during call orig to find out if there is already a call
  with the IM_CN_subsystem flag set to TRUE

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the IMCN call is up, False otherwise

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_bearer_mgr_is_IMCN_call_up
(
  void
)
{
  uint8                 bearer_index;
  ds_bearer_mgr_s*      bearer_mgr_p;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Check the bearer cntx table to see if there if any call has the IM CN flag 
    set.
  -------------------------------------------------------------------------*/                                          
  for(bearer_index=0;bearer_index < DS_3GPP_MAX_BEARER_CONTEXT;bearer_index++)
  {  
    bearer_mgr_p = ds_bearer_manager[bearer_index]; 
    
    if(!ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p))
    {
      continue;
    }
    
    /*-----------------------------------------------------------------------
      If a call is in UP state and IM CN flag is set, set im_cn_flag = TRUE.
    -----------------------------------------------------------------------*/
    if( (bearer_mgr_p->state == DS_BEARER_MGR_STATE_UP ||
    	    bearer_mgr_p->state == DS_BEARER_MGR_STATE_UP_DORMANT ||
    	    bearer_mgr_p->state == DS_BEARER_MGR_STATE_UP_DORMANT_REESTAB) &&
        (bearer_mgr_p->im_cn_subsystem_flag == TRUE) )
    {
      return TRUE;
    }
    else if((ds_pdn_mgr_validate_pdn_ptr(bearer_mgr_p->pdn_mgr_ptr)) &&
            (bearer_mgr_p->state == DS_BEARER_MGR_STATE_COMING_UP) &&
            (bearer_mgr_p->pdn_mgr_ptr->pdp_profile->im_cn_flag == TRUE))
    {
    /*-----------------------------------------------------------------------
        Check the profile to see if there if any call in pending state
        with the IM CN flag.      
        If the call is in coming up state, that means a call with IM CN
        flag is pending.
      -----------------------------------------------------------------------*/                                               
      return TRUE;
    }
  } /* for .. */

  return FALSE;
} /* ds_bearer_mgr_is_IMCN_call_up() */
#endif /* FEATURE_DATA_IMS */

/*===========================================================================
FUNCTION DS_BEARER_MGR_GET_BEARER_FROM_SDF_ID

DESCRIPTION
  This fn is used to find the bearer for a given sdf_id

PARAMETERS
  sdf_id

DEPENDENCIES
  None.

RETURN VALUE
  Bearer ptr if present, else NULL.

SIDE EFFECTS
  None.
===========================================================================*/
ds_bearer_mgr_s* ds_bearer_mgr_get_bearer_from_sdf_id
(
  uint16   sdf_id
)
{
  uint8                 bearer_index;
  ds_bearer_mgr_s*      bearer_mgr_p;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  for(bearer_index=0;bearer_index < DS_3GPP_MAX_BEARER_CONTEXT;bearer_index++)
  {  
    bearer_mgr_p = ds_bearer_manager[bearer_index]; 
    
    if(!ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p))
    {
      continue;
    }
    
    if( bearer_mgr_p->sdf_id == sdf_id )
    {
      return bearer_mgr_p;
    }
  }

  return NULL;
} /* ds_bearer_mgr_get_bearer_from_sdf_id() */

/*===========================================================================
FUNCTION DS_BEARER_MGR_GET_BEARER_FROM_EPS_ID

DESCRIPTION
  This fn is used to find the bearer for a given eps_id

PARAMETERS
  eps_id

DEPENDENCIES
  None.

RETURN VALUE
  Bearer ptr if present, else NULL.

SIDE EFFECTS
  None.
===========================================================================*/
ds_bearer_mgr_s* ds_bearer_mgr_get_bearer_from_eps_id
(
  uint8   eps_id
)
{
  uint8                 bearer_index;
  ds_bearer_mgr_s*      bearer_mgr_p;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  for(bearer_index=0;bearer_index < DS_3GPP_MAX_BEARER_CONTEXT;bearer_index++)
  {  
    bearer_mgr_p = ds_bearer_manager[bearer_index]; 
    
    if( !ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p) ||
    	   bearer_mgr_p->eps_bearer == NULL )
    {
      continue;
    }
    
    if( bearer_mgr_p->bearer_id == eps_id )
    {
      return bearer_mgr_p;
    }
  }

  return NULL;
} /* ds_bearer_mgr_get_bearer_from_sdf_id() */

/*===========================================================================
FUNCTION DS_BEARER_MGR_GET_CACHED_BEARER_FROM_EPS_ID

DESCRIPTION
  This fn is used to find the cached bearer for a given eps_id

PARAMETERS
  eps_id

DEPENDENCIES
  None.

RETURN VALUE
  Bearer ptr if present, else NULL.

SIDE EFFECTS
  None.
===========================================================================*/
ds_bearer_mgr_s* ds_bearer_mgr_get_cached_bearer_from_eps_id
(
  uint8   eps_id
)
{
  uint8                 bearer_index;
  ds_bearer_mgr_s*      bearer_mgr_p;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  for(bearer_index=0;bearer_index < DS_3GPP_MAX_BEARER_CONTEXT;bearer_index++)
  {  
    bearer_mgr_p = ds_bearer_manager_cache[bearer_index]; 
    
    if( !ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p) )
    {
      continue;
    }
    
    if( bearer_mgr_p->bearer_id == eps_id )
    {
      return bearer_mgr_p;
    }
  }

  return NULL;
} /* ds_bearer_mgr_get_cached_bearer_from_eps_id() */

/*===========================================================================
FUNCTION DS_BEARER_MGR_GET_BEARER_FROM_NSAPI

DESCRIPTION
  This fn is used to find the bearer for a given NSAPI

PARAMETERS
  eps_id

DEPENDENCIES
  None.

RETURN VALUE
  Bearer ptr if present, else NULL.

SIDE EFFECTS
  None.
===========================================================================*/
ds_bearer_mgr_s* ds_bearer_mgr_get_bearer_from_nsapi
(
  uint8   naspi
)
{
  uint8                 bearer_index;
  ds_bearer_mgr_s*      bearer_mgr_p;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  for(bearer_index=0;bearer_index < DS_3GPP_MAX_BEARER_CONTEXT;bearer_index++)
  {  
    bearer_mgr_p = ds_bearer_manager[bearer_index]; 
    
    if( !ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p) )
    {
      continue;
    }
    
    if( bearer_mgr_p->bearer_id == naspi )
    {
      return bearer_mgr_p;
    }
  }

  return NULL;
} /* ds_bearer_mgr_get_bearer_from_nsapi() */

/*===========================================================================
FUNCTION DS_BEARER_MGR_SET_UL_DATA_BIT_MASK

DESCRIPTION
  This fn is used to set the bearer id bit mask for UL data in bearer info.

PARAMETERS
  bearer ptr

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_bearer_mgr_set_ul_data_bit_mask
(
  ds_bearer_mgr_s  *bearer_mgr_p
)
{
  ds_bearer_info->uplink_data_bitmask |= 
             ((uint8)0x1 << bearer_mgr_p->index);
  return;
}

/*===========================================================================
FUNCTION DS_BEARER_MGR_UNSET_UL_DATA_BIT_MASK

DESCRIPTION
  This fn is used to clear the bearer id bit mask for UL data in bearer info.

PARAMETERS
  bearer ptr

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_bearer_mgr_unset_ul_data_bit_mask
(
  ds_bearer_mgr_s  *bearer_mgr_p
)
{
  ds_bearer_info->uplink_data_bitmask &= 
             ~((uint8)0x1 << bearer_mgr_p->index);
  return;
}

/*===========================================================================
FUNCTION DS_BEARER_MGR_QUERY_UL_DATA_BIT_MASK

DESCRIPTION
  This fn is used to query the bearer id bit mask for UL data in bearer info.

PARAMETERS
  bearer ptr

DEPENDENCIES
  None.

RETURN VALUE
  boolean.

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_bearer_mgr_query_ul_data_bit_mask
(
  ds_bearer_mgr_s  *bearer_mgr_p
)
{
  if(ds_bearer_info->uplink_data_bitmask & ((uint8)0x1 << bearer_mgr_p->index))
 	{
   	return TRUE;
 	}
  
  return FALSE;
}

/*===========================================================================
FUNCTION DS_BEARER_MGR_GET_UL_DATA_CRIT_SECT

DESCRIPTION
  This fn is used to get the critical section lock for UL data path.

PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE
  Reference for rex type.

SIDE EFFECTS
  None.
===========================================================================*/
rex_crit_sect_type* ds_bearer_mgr_get_ul_data_crit_sect
(
  void
)
{
  return &ds_bearer_info->ul_data_crit_sect;
}

/*===========================================================================
FUNCTION DS_BEARER_MGR_INFO_CLEANUP

DESCRIPTION
  This fn is used to clear the UE based params in bearer info.

PARAMETERS
  bearer ptr

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_bearer_mgr_info_cleanup
(
  ds_bearer_mgr_s  *bearer_mgr_p
)
{  
  ds_bearer_info->access_barring_rab_retry = 0;
  bearer_mgr_p->rab_reestab_retry_cnt = 0;

  ds_bearer_mgr_unset_ul_data_bit_mask(bearer_mgr_p); 
  if(ds_bearer_info->uplink_data_bitmask == 0)
  {
    if(ds_bearer_info->timer_running)
    {
      ps_timer_cancel(ds_bearer_info->tmr_rab_reestab);
      ds_bearer_info->timer_running = FALSE;
    }
  }

  if(bearer_mgr_p->srv_rsp_exp == TRUE)
  {
    ds_bearer_info->service_request_pending_flag = FALSE;
    bearer_mgr_p->srv_rsp_exp = FALSE; 
  }
  
  return;
}

/*===========================================================================
FUNCTION DS_BEARER_MGR_GENERATE_16BIT_RAND_NUM_BY_RANGE

DESCRIPTION
  This fn is used to generate 16bit random number within a specified range.

PARAMETERS
  High and low limits of the range.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
word ds_bearer_mgr_generate_16bit_rand_num_by_range
(
  word  lo_val,
  word  hi_val
)
{
  dword random_num;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  (void)secapi_get_random( SECAPI_PKCS_RANDOM,
                           (uint8 *)&random_num,
                           sizeof(uint32) );
  return (ran_dist(random_num, lo_val, hi_val));
} /* ds_bearer_mgr_generate_16bit_rand_num_by_range */

/*===========================================================================
FUNCTION DS_BEARER_MGR_RAB_RE_ESTAB_IND_HDLR

DESCRIPTION
  This function handles rab reject indication for 3GPP modes. This will in
  turn dispatch any mode specific functions
  
PARAMETERS  
  esm_drb_setup_ind
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  TRUE if success, FALSE otherwise.
   
SIDE EFFECTS 
  None.  
===========================================================================*/
void ds_bearer_mgr_rab_re_estab_ind_hdlr
(
  emm_drb_setup_ind_s_type  *info_ptr
)
{
  ds_bearer_mgr_s                    *bearer_mgr_p = NULL;
  sys_sys_mode_e_type                 network_mode;
  ds_pdn_mgr_s                       *pdn_mgr_p = NULL;
  uint32                              ran_num;
  void                               *sm_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Get the bearer context from bearer id
  -------------------------------------------------------------------------*/
  bearer_mgr_p =  ds_bearer_mgr_get_bearer_from_eps_id(info_ptr->eps_bearer_id);
  if(!ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p))
  {
    return;
  }

  pdn_mgr_p = bearer_mgr_p->pdn_mgr_ptr;
  if (!ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p))
  {
    return;
  }

  if(bearer_mgr_p->state == DS_BEARER_MGR_STATE_UP_DORMANT)
  {
    bearer_mgr_p->state = DS_BEARER_MGR_STATE_UP_DORMANT_REESTAB;
  }
  
  DS_MSG2(MSG_LEGACY_HIGH, "RAB reestab ind bearer inst %d, state %d",
          bearer_mgr_p->index, bearer_mgr_p->state);
  
  /* RAB re-establishment should be received only in the below states. */
  if( bearer_mgr_p->state != DS_BEARER_MGR_STATE_UP_DORMANT_REESTAB &&
      bearer_mgr_p->state != DS_BEARER_MGR_STATE_PEND_RAB_REG)
  {
    return;
  }
  
  /*-------------------------------------------------------------------------
   * Reset the service_request_pending_flag if the response if obtained 
   * as a result of trying to reorig using this bearer
  -------------------------------------------------------------------------*/
  if(bearer_mgr_p->srv_rsp_exp == TRUE)
  {
    ds_bearer_info->service_request_pending_flag = FALSE;
    bearer_mgr_p->srv_rsp_exp = FALSE;
  }

   /*--------------------------------------------------------------------------
      Get the throttling state macine and reset it. NULL checking need not be
      performed here as there is no dereferencing of the opaque handle in this
      function. 
      As per Latest VZW Spec, VZW LTE Data Retry Reqs spec:
      3.3.3 Per System Nature of Throttling, throttling SM needs to be cleared 
      on receiving unsolicited RAB_REESTAB_IND from NW.
    --------------------------------------------------------------------------*/
  sm_ptr = ds_3gpp_srv_req_get_throt_sm(FALSE);
  ds_3gpp_srv_req_throt_reset(sm_ptr);

  /*-------------------------------------------------------------------------
    Unset the bitmask corresponding to this bearer 
  -------------------------------------------------------------------------*/
  ds_bearer_mgr_unset_ul_data_bit_mask(bearer_mgr_p);

  ds_3gpp_rab_reestab_transition_state_ev(DS_3GPP_PROC_RAB_REESTAB_IND,
                                       NULL);

  /*-----------------------------------------------------------------------
    Reset the rab_reestab_retry counter for this bearer.
  -----------------------------------------------------------------------*/
  bearer_mgr_p->rab_reestab_retry_cnt = 0;

  if(ds_bearer_info->uplink_data_bitmask == 0)
  {
    if(ds_bearer_info->timer_running == TRUE)
    {
      ps_timer_cancel(ds_bearer_info->tmr_rab_reestab);
      ds_bearer_info->timer_running = FALSE;
    }
  }
  else
  {
    if(ds_bearer_info->timer_running == FALSE)
    {
      /*-----------------------------------------------------------------------    
        Set the timer with the random value between 
        DS_BEARER_CNTXT_RAB_REESTABLISH_MIN_RETRY_TIME and 
        DS_BEARER_CNTXT_RAB_REESTABLISH_MIN_RETRY_TIME * 4 to send out RAB
        reestablishment request again. Once the timer expires, the dormant call
        reorig function will be called and RAB reestablishment is tried again.
      -----------------------------------------------------------------------*/
      ran_num = ds_bearer_mgr_generate_16bit_rand_num_by_range(
                      DS_BEARER_MGR_RAB_REESTABLISH_MIN_RETRY_TIME,
                      DS_BEARER_MGR_RAB_REESTABLISH_MIN_RETRY_TIME * 4);

      ps_timer_start(ds_bearer_info->tmr_rab_reestab, (int64)ran_num);
      ds_3gpp_rab_reestab_transition_state_ev(
                               DS_3GPP_PROC_RAB_REESTAB_RAND_RETRY,
                                           (void*)ran_num);
      ds_bearer_info->timer_running = TRUE;
    }
   }
  /*---------------------------------------------------------------------------
   * Now that we have the Reorigination done, we should reset the each enqueue 
   * function pointer to NULL so that sucessive enqueuing of packet to the WM 
   * do not cause a reorig request from being sent to NAS
  ---------------------------------------------------------------------------*/
  (bearer_mgr_p->tx_wm.wm_item).each_enqueue_func_ptr = NULL;
  
  /*------------------------------------------------------------------------ 
    Reset the access barring rab retry 
  ------------------------------------------------------------------------*/
  ds_bearer_info->access_barring_rab_retry = FALSE;

  /*---------------------------------------------------------------------------
    Before proceeding, we have to make sure that there is no initiation to tear
    down the call. If there has been an attempt to tear down the call, we just
    have to return failure indicating CM that the lower layers will not be set
    up
  ---------------------------------------------------------------------------*/
  if(bearer_mgr_p->state == DS_BEARER_MGR_STATE_GOING_DOWN)
  {
    DS_MSG1(MSG_LEGACY_HIGH, 
    	       "RAB reestab ind after tear down for bearer inst %d",
            bearer_mgr_p->index);
    return;
  }

  network_mode = pdn_mgr_p->nw_mode;
  if(network_mode == SYS_SYS_MODE_LTE)
  {
    ds_eps_bearer_mgr_rab_re_estab_ind_hdlr(bearer_mgr_p, info_ptr);
  }
  else
  {
    DS_ERROR_FATAL("RAB reest in incorrect sys mode");
    return;
  }

  bearer_mgr_p->drb_is_up = TRUE;

  if(bearer_mgr_p->eps_bearer != NULL)
  {
    bearer_mgr_p->eps_bearer->rb_id = info_ptr->rb_id;
  }

  ds_pdn_cp_data_process_rab_reestab(bearer_mgr_p->index);
  
  return;
} /* ds_bearer_cntxt_call_rab_re_estab_ind_hdlr */

/*===========================================================================
FUNCTION ds_bearer_mgr_reorig_dorm_call_cb

DESCRIPTION
  This function sends a command to DS task to trigger dormant re-orig. Called
  when data is enqueued into the Um Tx watermark.
  
PARAMETERS  
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.

===========================================================================*/
void ds_bearer_mgr_reorig_dorm_call_cb
(
  dsm_watermark_type *wm_p,
  void               *call_back_data
)
{
  ds_command_type     *cmd_ptr = NULL;
  ds_bearer_mgr_s     *bearer_mgr_p = NULL;
  ds_bearer_mgr_s    **data_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  bearer_mgr_p = (ds_bearer_mgr_s*)call_back_data;

  if(!ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p))
  {
    return;
  }  
  
  if(bearer_mgr_p->reorig_cmd_enqueued == FALSE)
  {
    DS_MSG0(MSG_LEGACY_HIGH,
    	       "Each enqueue function call back. Posting re-orig cmd.");

    /*------------------------------------------------------------------------
     * This function is executed in the PS context.
     * So all we do here is to put a command to DS and exit
    ------------------------------------------------------------------------*/
    cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_bearer_mgr_s*));
    if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
    {
      DS_ASSERT(0);
      return;
    }

    cmd_ptr->hdr.cmd_id = DS_CMD_PDN_DORM_REORIG;
    data_ptr = (ds_bearer_mgr_s**)cmd_ptr->cmd_payload_ptr;
    *data_ptr = bearer_mgr_p;

    /*------------------------------------------------------------------------
      Here, we set the reorig_cmd_enqueued flag to true. This is to avoid
      multiple reorig commands from being enqueued for the same bearer when
      a bunch of packets are enqueued into the WM.

      This flag is cleared when we process this command which gives rise to a
      question as to what happens if a packet is subsequently enqueued into
      the WM. It is handled as follows: As soon as there is a reorig request,
      we disable flow on the phys link. Thus subsequent enqueuing of packets
      does not happen on the same bearer until the flow on the phys link is
      enabled. This way, we ensure that we do not overrun the DS command buf.

      Put the command only after setting reorig_cmd_enqueued flag to avoid 
      race condition between PS and DS task, race condition will happen if 
      DS processes the command before setting reorig_cmd_enqueued
    ------------------------------------------------------------------------*/
    bearer_mgr_p->reorig_cmd_enqueued = TRUE;
    ds_put_cmd(cmd_ptr);

  }
  else
  {
    DS_MSG0(MSG_LEGACY_HIGH,"Reorig cmd already enqueued");
  }

  return;
} /* ds_bearer_mgr_reorig_dorm_call_cb() */

/*===========================================================================
  FUNCTION DS_BEARER_MGR_REORIG_DORM

  DESCRIPTION
    This function is responsible for reoriginating the call from dormancy.
    This function ends up calling RAB reestab.
  
  PARAMETERS   
   *bearer_mgr_p - Pointer to the bearer context that we are working on
                     
  DEPENDENCIES
    None.
  
  RETURN VALUE 
    None.
  
  SIDE EFFECTS 
    None.  
===========================================================================*/
void ds_bearer_mgr_reorig_dorm
(
  ds_bearer_mgr_s  *bearer_mgr_p
)
{
  ds_pdn_mgr_s    *pdn_mgr_p = NULL;
  ds_pdn_ioctl_3gpp_lte_reestab_calltype_enum_type              
                   v4_inst_reestab_call_type = 
                                        DS_PDN_3GPP_LTE_REESTAB_CALLTYPE_NORMAL;
  ds_pdn_ioctl_3gpp_lte_reestab_calltype_enum_type              
                   v6_inst_reestab_call_type = 
                                        DS_PDN_3GPP_LTE_REESTAB_CALLTYPE_NORMAL;  
  emm_rab_reestab_req_s_type rab_params = {{0}};
#ifdef FEATURE_MODE_TLB
  ds_eps_tlb_per_subs_info_type *eps_tlb_hndl = NULL;
#endif /* FEATURE_MODE_TLB */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(!ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p))
  {
    return;
  }

  DS_MSG2(MSG_LEGACY_HIGH,
  	       "ds_bearer_mgr_reorig_dorm for bearer %d in state %d",
  	       bearer_mgr_p->index, bearer_mgr_p->state);

#ifdef FEATURE_MODE_TLB
  /*-------------------------------------------------------------------------
    Check whether this is loopback data.
  -------------------------------------------------------------------------*/
  eps_tlb_hndl = ds_eps_tlb_get_info_hndl();
  if( ds_eps_tlb_validate_info_hndl(eps_tlb_hndl) )
  {
    if( eps_tlb_hndl->tlb_state.mode == LTE_TLB_MODE_I )
    {
      ds_eps_tlb_send_ul_data_mode_I( &bearer_mgr_p->tx_wm.wm_item, 
                                      (void*)bearer_mgr_p );
      return;
    }
    else if( eps_tlb_hndl->tlb_state.mode == LTE_TLB_MODE_H )
    {
      ds_eps_tlb_send_ul_data_mode_H( &bearer_mgr_p->tx_wm.wm_item, 
                                      (void*)bearer_mgr_p );
      return;
    }
  }
#endif /* FEATURE_MODE_TLB */

  /*-------------------------------------------------------------------------
    Decide whether Data should be sent over DRB or SRB
  -------------------------------------------------------------------------*/
  if (ds_pdn_cp_data_send_over_srb 
        (&bearer_mgr_p->tx_wm.wm_item, (void*)bearer_mgr_p) != 
         DS_PDN_CP_DATA_DRB_SETUP_REQD)
  {
    return;
  }
#ifdef FEATURE_NBIOT_NTN
  if (ds_pdn_mgr_get_ntn_gnss_status())
  {
    DS_ERROR_LOG_0("System is in GNSS failure state, returning");
    return;
  }
#endif /* FEATURE_NBIOT_NTN */
  pdn_mgr_p = bearer_mgr_p->pdn_mgr_ptr;
  if(!ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p))
  {
    return;
  }

  /*-------------------------------------------------------------------------
    RAB-reestablishment can happen on the bearer only if it is DORMANT.
    If the bearer is in any other state, then we do nothing for the reorig
    request.
  -------------------------------------------------------------------------*/
  if(bearer_mgr_p->state == DS_BEARER_MGR_STATE_UP_DORMANT)
  {
    if(ds_pdn_hdlr_get_ps_data_suspend_flag() == TRUE)
    {
      if( (!ds_pdn_hdlr_get_sent_ps_data_avail_flag()) && 
           ds_pdn_hdlr_is_ps_data_avail_ind_allowed() )
      {
        DS_MSG1(MSG_LEGACY_HIGH,
        	       "UL data on bearer %d while PS data is suspended, "
        	       "sending data_available",
                bearer_mgr_p->index);
        ds_bearer_mgr_send_ps_data_avail();
      }

      /*--------------------------------------------------------------
          Disable flow
          --------------------------------------------------------------*/      
      ds_bearer_mgr_disable_tx_flow(bearer_mgr_p, DS_BEARER_CONTROL_MASK);          
      return;
    }
    else if(pdn_mgr_p->nw_mode == SYS_SYS_MODE_GSM)
    {      
      /*--------------------------------------------------------------------
          RAB reestablishment can happen only for LTE.
          For all other modes,eg Gsm, we don't establish Rab and hence
          return.
        --------------------------------------------------------------------*/
      DS_ERROR_LOG_0("System mode is GSM, returning");
      return;
    }
        
    if (ds_pdn_nv_manager_get_max_rab_reestab_retry_cnt() == 
    	                                   RAB_REESTAB_RETRY_CNT_INVALID)
    {
    /*-------------------------------------------------------------------------
      Disable flow on this bearer so that no further packets can be queued.
      Similarly the socket app should also not write data into this bearer.
      This way, we prevent more reorig commands from being put into DS task.
    -------------------------------------------------------------------------*/ 
    ds_bearer_mgr_disable_tx_flow(bearer_mgr_p, DS_BEARER_CONTROL_MASK);          
    }

    /*-------------------------------------------------------------------------
      In the uplink_data_bitmask, we set the bit field corresponding to this
      bearer. This way we prevent a reorig request from being sent out due to
      the same bearer.
    -------------------------------------------------------------------------*/
    ds_bearer_mgr_set_ul_data_bit_mask(bearer_mgr_p);
    
    /*-------------------------------------------------------------------------
      If the timer to re-establish RABs is already running or if a service
      request to reestab rab has already been sent out, then we do nothing.
      We return from this function.
    -------------------------------------------------------------------------*/
    if( (ds_bearer_info->service_request_pending_flag == TRUE ) || 
        (ds_bearer_info->timer_running == TRUE)  )
    {
      DS_MSG2(MSG_LEGACY_HIGH,
      	       "Rab Reest req pend: %d or Timer_running: %d. Ignoring reorig.",
              ds_bearer_info->service_request_pending_flag,
              ds_bearer_info->timer_running);
      return;
    }
    
    /*-------------------------------------------------------------------------
      Set the state of the bearer to RAB_REESTAB.
    -------------------------------------------------------------------------*/
    bearer_mgr_p->state = DS_BEARER_MGR_STATE_UP_DORMANT_REESTAB;

    /*-------------------------------------------------------------------------
      Clear the RAB reestablish timer. This will be started again either in
      rab_re_est_ind or in rab_re_est_rej handler or in the registered timer
      call back
    -------------------------------------------------------------------------*/    
    ps_timer_cancel(ds_bearer_info->tmr_rab_reestab);
    ds_bearer_info->timer_running = FALSE;   

    /*-------------------------------------------------------------------------
      Initialize rab_params.
    -------------------------------------------------------------------------*/
    if (pdn_mgr_p->nw_mode == SYS_SYS_MODE_LTE)
    {
      if(!pdn_mgr_p->lte_params)
      {
        DS_ERROR_LOG_0("NULL LTE params.");
        return;
      }
      
      /* MO Exception Handling */
      if (ds_pdn_cp_data_pending_mo_ex_data_avail(bearer_mgr_p->index))
      {
        DS_MSG0(MSG_LEGACY_HIGH, 
        	       "RAB Re-establishment is for MO Exception data");
        rab_params.mo_exception_data = TRUE;
      }

      rab_params.eps_bearer_id = bearer_mgr_p->bearer_id;
      /*--------------------------------------------------------------------- 
        Determine RAB Re-establishment call type
        ---------------------------------------------------------------------*/
      v4_inst_reestab_call_type = pdn_mgr_p->lte_params->rab_reestab_call_type
                                                 [DS_PDN_INSTANCE_IPV4]; 
      v6_inst_reestab_call_type = pdn_mgr_p->lte_params->rab_reestab_call_type
                                                 [DS_PDN_INSTANCE_IPV6];
      /*--------------------------------------------------------------------- 
        If the RE-Estab Calltype is VOLTE in atleast one of the Ifaces then
        request for Re-Estab Call type as VOLTE
        ---------------------------------------------------------------------*/
      if ((v4_inst_reestab_call_type == DS_PDN_3GPP_LTE_REESTAB_CALLTYPE_VOLTE)||
          (v6_inst_reestab_call_type == DS_PDN_3GPP_LTE_REESTAB_CALLTYPE_VOLTE))
      {
        rab_params.call_type = SYS_LTE_REESTAB_CALLTYPE_VOLTE;
      }
      else
      {
        rab_params.call_type = SYS_LTE_REESTAB_CALLTYPE_NORMAL;
      }
      
      DS_MSG2(MSG_LEGACY_HIGH,
      	       "Requesting reestab for bearer %d with Re-estab Call type %d",
              bearer_mgr_p->index, rab_params.call_type);
    }

    ds_3gpp_rab_reestab_transition_state_ev(DS_3GPP_PROC_RAB_REESTAB_REQ, NULL);

    rab_params.message_header.message_id = EMM_RAB_REESTAB_REQ;
    rab_params.message_header.message_set = MS_MM_DS;      
    rab_params.message_header.message_len_lsb =
      (sizeof( emm_rab_reestab_req_s_type ) - sizeof(IMH_T)) % 256;
    rab_params.message_header.message_len_msb =
      (sizeof( emm_rab_reestab_req_s_type ) - sizeof(IMH_T)) / 256;
    
    /* Set the connection ID as invalid */
    rab_params.esm_hdr.connection_id = ESM_INVALID_CON_ID;
    
    if(dssysmgr_get_cm_deep_sleep_status())
    {
      cm_lte_data_transport_req(NULL,NULL,CM_CLIENT_TYPE_DS,NULL);      
    }
    ds_pdn_mgr_ds_to_nas_send_msg((void*)&rab_params);  

    /*-------------------------------------------------------------------------
     * Set the service_request_pending_flag to TRUE so that we do not post 
     * another request while there is OTA service request still to be answered
    -------------------------------------------------------------------------*/
    ds_bearer_info->service_request_pending_flag = TRUE;

    /*-------------------------------------------------------------------------
     * Also mark the bearer context that was used to send out the service req
     * by setting the srv_rsp_exp to TRUE
    -------------------------------------------------------------------------*/
    bearer_mgr_p->srv_rsp_exp = TRUE;
  }
  
  return; 
} /* ds_bearer_mgr_reorig_dorm() */

/*===========================================================================
FUNCTION  DS_BEARER_MGR_PROC_REORIG_DORM_CMD

DESCRIPTION
  This function is called when we process the command to reestablish RAB
  due to enqueuing data into the watermarks.

PARAMETERS   
 *bearer_mgr_p - Pointer to the bearer context

DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
  
SIDE EFFECTS 
  None.
===========================================================================*/
void ds_bearer_mgr_proc_reorig_dorm_cmd
(
  ds_bearer_mgr_s   *bearer_mgr_p
)
{
  if(!ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p))
  {
    return;
  }
   
  /*Handle if suspend indication was recieved when there was no data in wm,
    will send data available indication to ds during enqueue*/
  if(ds_pdn_hdlr_get_ps_data_suspend_flag())
  {
    if ( !ds_pdn_hdlr_get_sent_ps_data_avail_flag() &&
         ds_bearer_mgr_is_tx_flow_enabled(bearer_mgr_p) )
    {
      ds_bearer_mgr_suspend_ind();
    }
  }

  /*-------------------------------------------------------------------------
   * Call the function to reorig
  -------------------------------------------------------------------------*/
  ds_bearer_mgr_reorig_dorm(bearer_mgr_p);
  
  /*-------------------------------------------------------------------------
    Set the reorig_cmd_enqueued to FALSE which indicates that we have 
    processed the reorig request on this bearer
  -------------------------------------------------------------------------*/
  bearer_mgr_p->reorig_cmd_enqueued = FALSE;  
  return;
} /*ds_bearer_cntxt_proc_dorm_reorig_cmd()*/

/*===========================================================================
FUNCTION  DS_BEARER_MGR_HANDLE_MAX_RAB_REESTAB_RETRY_CNT

DESCRIPTION
  This function handles the max rab_reestab_retry count for a partciular
  bearer. The uplink WM is flushed, phys_link flow is enabled and 
  uplink_data_bitmask would be unset for the bearer. The retry counter
  would be reset to zero.
  
PARAMETERS
  bearer_mgr_p    The bearer context pointer

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_bearer_mgr_handle_max_rab_reestab_retry_cnt
(
  ds_bearer_mgr_s* bearer_mgr_p
)
{
  DS_MSG1(MSG_LEGACY_HIGH, "Handle max rab_reestab_retry for bearer %d",
          bearer_mgr_p->index);

  /*------------------------------------------------------------------------
    Flush the uplink watermark 
  ------------------------------------------------------------------------*/
  dsm_empty_queue( &(bearer_mgr_p->tx_wm.wm_item) );
  ds_pdn_cp_data_flush_num_mo_ex_data_cnt(bearer_mgr_p->index);
 
  /*----------------------------------------------------------------------
    Enable flow for this bearer
  ----------------------------------------------------------------------*/
  ds_bearer_mgr_enable_tx_flow(bearer_mgr_p, DS_BEARER_CONTROL_MASK);

  ds_bearer_mgr_info_cleanup(bearer_mgr_p);

  return;
} /* ds_bearer_mgr_handle_max_rab_reestab_retry_cnt */

/*===========================================================================
FUNCTION DS_BEARER_MGR_GET_BEARER_INST_TO_REORIG

DESCRIPTION
  This function is used to get a bearer instance on which reorigination is
  to be requested. This is used in the timer_expired command processing to
  get a bearer to work on.

  This uses the bit mask to determine a bearer that is still waiting on RAB
  reestablishment and returns it to the caller.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  Pointer to the bearer context awaiting RAB reestab if bitmask is non null
  NULL if bitmask is NULL

SIDE EFFECTS
  None.
===========================================================================*/
ds_bearer_mgr_s* ds_bearer_mgr_get_bearer_inst_to_reorig
(
  void
)
{
  uint8               index;
  boolean             found_bearer_instance = FALSE;
  ds_bearer_mgr_s*    bearer_mgr_p = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Loop through all the bearers. Query the bit field corresponding to this 
    bearer in the uplink_data_bitmask. If the field is set, then return this
    bearer to the calling function.
  -------------------------------------------------------------------------*/
  for(index=0; index < DS_3GPP_MAX_BEARER_CONTEXT; index++)
  {
    bearer_mgr_p = ds_bearer_manager[index];
    
    if (!ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p))
    {
      continue;
    }
	
    found_bearer_instance = ds_bearer_mgr_query_ul_data_bit_mask(bearer_mgr_p);

    if (found_bearer_instance == TRUE)
    {
      if ((bearer_mgr_p->rab_reestab_retry_cnt >= 
           	       ds_pdn_nv_manager_get_max_rab_reestab_retry_cnt()) &&
          (ds_bearer_info->ds_3gpp_reg_in_progress == FALSE))
      {
         ds_bearer_mgr_handle_max_rab_reestab_retry_cnt(bearer_mgr_p);
      }
      else
      {
        DS_MSG1(MSG_LEGACY_HIGH,
        	       "ds_bearer_cntxt_get_bearer_inst_to_reorig, found bearer_index:%d",
        	       index);
        break;
      }
    }
  }
  	
  return(index < DS_3GPP_MAX_BEARER_CONTEXT? bearer_mgr_p: NULL);
} /* ds_bearer_mgr_get_bearer_inst_to_reorig() */

/*===========================================================================
FUNCTION DS_BEARER_MGR_CHECK_IF_MAX_RAB_RETRIES_PERFORMED

DESCRIPTION
  This function checks if maximum rab retries have been performed.
  The number is different if access_barring_rab_retry_counter is set
      
PARAMETERS
  ds_bearer_mgr_s* bearer_context_p

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
  
===========================================================================*/
boolean ds_bearer_mgr_check_if_max_rab_retries_performed
(
  ds_bearer_mgr_s   *bearer_mgr_p
)
{
  boolean ret_val = FALSE;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------- 
    If access barring retry is not enabled check for MAX_RAB_REQ_RETRY_COUNT
    However if access barring is enabled check for
    access_barring_rab_retry_counter
  --------------------------------------------------------------------------*/
  if((bearer_mgr_p->rab_reestab_retry_cnt 
              == ds_pdn_nv_manager_get_max_rab_reestab_retry_cnt()) &&
     (ds_bearer_info->ds_3gpp_reg_in_progress == FALSE) &&
     (!ds_bearer_info->access_barring_rab_retry))
  {
    DS_ERROR_LOG_0("Non access barring max rab retries attempted");
    ret_val = TRUE;
  }
  else if((ds_bearer_info->access_barring_rab_retry) && 
          (bearer_mgr_p->rab_reestab_retry_cnt 
           == ds_bearer_info->access_barring_rab_retry_counter))
  {
    DS_ERROR_LOG_0("Access barring max rab retries attempted");
    ret_val = TRUE;
  }

  return ret_val; 
}

/*===========================================================================
FUNCTION ds_bearer_mgr_reorig_dorm_timer_cb

DESCRIPTION
  This function is registered as the timer call back. When the timer to do
  Rab_reestab retry expires, this function is called. The processing is 
  serialized by posting a command to DS.
  
PARAMETERS  
   None
   
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.
===========================================================================*/
void ds_bearer_mgr_reorig_dorm_timer_cb
(
  void  *instance
)
{
  ds_command_type     *cmd_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*------------------------------------------------------------------------
   * This function is executed in the PS context.
   * So all we do here is to put a command to DS and exit
  ------------------------------------------------------------------------*/
  cmd_ptr = ds_allocate_cmd_buf(DS_CMD_PAYLOAD_SIZE_ZERO);
  if(cmd_ptr == NULL)
  {
    DS_ASSERT(0);
    return;
  }

  cmd_ptr->hdr.cmd_id = DS_CMD_PDN_DORM_REORIG_TMR_EXP;
  ds_put_cmd(cmd_ptr);

  return;
} /* ds_bearer_mgr_reorig_dorm_timer_cb() */

/*===========================================================================
FUNCTION DS_BEARER_MGR_PROC_REORIG_DORM_TMR_EXP_CMD()

DESCRIPTION
  This function is called when we process the command posted to DS as a result
  of dormancy timer expiry. This simply calls the function to reestab RAB.
      
PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.  
===========================================================================*/
void ds_bearer_mgr_proc_reorig_dorm_tmr_exp_cmd
(
  void
)
{
  uint32               ran_num;
  ds_bearer_mgr_s     *bearer_mgr_p;/* Bearer context pointer */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_MSG0(MSG_LEGACY_HIGH, "Reorig timer expiry cmd");

  /* Set the timer_running flag to FALSE */
  ds_bearer_info->timer_running = FALSE;
  ds_3gpp_rab_reestab_transition_state_ev(DS_3GPP_PROC_RAB_REESTAB_RAND_TIMER_EXP,
                                       NULL);

  /*-------------------------------------------------------------------------
    If the OTA request to reestablish RAB is already pending, then we restart
    the timer, provided there are RABs that need to be set up. This is 
    determined by the uplink_data_bitmask being non-null.
  -------------------------------------------------------------------------*/
  if(ds_bearer_info->service_request_pending_flag == TRUE)
  {
    if(ds_bearer_info->uplink_data_bitmask != 0)
    {
      if (!ds_bearer_info->access_barring_rab_retry)
      {
        ran_num = ds_bearer_mgr_generate_16bit_rand_num_by_range(
                  DS_BEARER_MGR_RAB_REESTABLISH_MIN_RETRY_TIME, 
                  DS_BEARER_MGR_RAB_REESTABLISH_MIN_RETRY_TIME * 4 );
      }
      else
      {
        DS_MSG1(MSG_LEGACY_HIGH,
        	       "Setting rab_retry_timer to ACCESS_BARRING RETRY VAL of %d",
                ds_bearer_info->access_barring_rab_retry_timer);
        ran_num = ds_bearer_info->access_barring_rab_retry_timer;
      }
  
      ps_timer_start(ds_bearer_info->tmr_rab_reestab, (int64)ran_num);
      ds_3gpp_rab_reestab_transition_state_ev(
                     DS_3GPP_PROC_RAB_REESTAB_RAND_RETRY,
                                         (void *)ran_num);
      ds_bearer_info->timer_running = TRUE;
    }
  }
  else
  {
    if(ds_bearer_info->uplink_data_bitmask != 0)
    {
      /*-----------------------------------------------------------------------
        Call the function to get a bearer context on which we can initiate a
       RAB reestablishment.
      -----------------------------------------------------------------------*/
      bearer_mgr_p = ds_bearer_mgr_get_bearer_inst_to_reorig();
      if(ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p))
      {
        if (ds_pdn_nv_manager_get_max_rab_reestab_retry_cnt() != 
        	                                 RAB_REESTAB_RETRY_CNT_INVALID)
        {          
          /*-----------------------------------------------------------------
            Rab re-estab retry cnt mechanism will be triggerred only when 
            reject cause is not local reject from NAS.
          -----------------------------------------------------------------*/
          if (ds_bearer_mgr_check_if_max_rab_retries_performed(bearer_mgr_p))
          {
            ds_bearer_mgr_handle_max_rab_reestab_retry_cnt(bearer_mgr_p);

            /*----------------------------------------------------------------- 
              Get the bearer context for the next bearer which has uplink 
              data to send.
            ------------------------------------------------------------------*/
            bearer_mgr_p = ds_bearer_mgr_get_bearer_inst_to_reorig();
          }

          /*--------------------------------------------------------------------
            Check if the bearer_context is not NULL and uplink_data_bitmask is
            set. This is required to handle the scenario when multiple bearers
            have data to send and one bearer reaches max_retry_cnt, in such a 
            case DS would send service_request for the next bearer which has 
            uplink bitmask set.
          --------------------------------------------------------------------*/
          if (ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p) && 
              ds_bearer_info->uplink_data_bitmask != 0)
          {
            DS_MSG1(MSG_LEGACY_HIGH,
            	       "TIMER CB FOR Rab-Reorig called; bearer %d",\
                    bearer_mgr_p->index);

            ds_bearer_mgr_reorig_dorm(bearer_mgr_p);

            /*---------------------------------------------------------------
                     Increment retry_cnt only if reject cause is not local
                    ---------------------------------------------------------------*/
            if (ds_bearer_info->ds_3gpp_reg_in_progress == FALSE)
            {
              bearer_mgr_p->rab_reestab_retry_cnt++;
            }
          }
        }
        else
        {
          DS_MSG1(MSG_LEGACY_HIGH,
                  "TIMER CB FOR Rab-Reorig called; bearer %d",\
                  bearer_mgr_p->index);         
          ds_bearer_mgr_reorig_dorm(bearer_mgr_p);
        }
      }
      else
      {
       /*-------------------------------------------------------------
            Technically we should not reach here. If we do, the case is such that
            the uplink_data_bitmask is non null but we could not find a bearer that
            needs RAB to be set up. This could possibly be due to corruption of
            the bitmask. We ignore the reorig req and set the bitmask to NULL.
            -------------------------------------------------------------*/
        DS_ERROR_LOG_0("Could not locate a bearer to reorig on. Ignoring cmd");
        ds_bearer_info->uplink_data_bitmask = 0;
      }
    }
  }
  
  
  /*------------------------------------------------------------------------ 
    Reset the access barring rab retry 
    ------------------------------------------------------------------------*/
  ds_bearer_info->access_barring_rab_retry = FALSE;
  
  /*-------------------------------------------------------------------------- 
    Reset emm_reg flag here, So the next rab_rej is handled gracefully
   --------------------------------------------------------------------------*/
  ds_bearer_info->ds_3gpp_reg_in_progress = FALSE;
 
  return;
} /* ds_bearer_mgr_proc_reorig_dorm_tmr_exp_cmd() */

/*===========================================================================
FUNCTION ds_bearer_mgr_rab_release_pdp_proto_stack

DESCRIPTION
  This function registers a non_empty_function callback pointer in the
  PS to PDCP watermark used for PDP-IP calls. When data appears on the
  watermark again, the callback will be invoked to initiate  RAB
  re-establishment
  
PARAMETERS  
  bearer_context_p   - bearer context pointer
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_bearer_mgr_rab_release_pdp_proto_stack
(
  ds_bearer_mgr_s        *bearer_mgr_p
)
{
  dsm_watermark_type*  tx_wm_p;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(!bearer_mgr_p)
  {
    return;
  }
  
  DS_MSG1(MSG_LEGACY_HIGH, "Uncoupling PDCP watermark for bearer %d",
          bearer_mgr_p->index);
  /*-------------------------------------------------------------------------  
    Get the PDCP state information for the call.
  -------------------------------------------------------------------------*/
  tx_wm_p = &(bearer_mgr_p->tx_wm.wm_item);

  /*-------------------------------------------------------------------------  
    Register each enqueue function ptr.
    Call id will be passed as parameter when watermark cb functions take a
    parameter.
  -------------------------------------------------------------------------*/  
  tx_wm_p->each_enqueue_func_ptr = ds_bearer_mgr_reorig_dorm_call_cb;
  tx_wm_p->each_enqueue_func_data = (void*)bearer_mgr_p;
} /* ds_bearer_mgr_rab_release_pdp_proto_stack() */

/*===========================================================================
FUNCTION DS_BEARER_MGR_REORIG_FROM_DORM_ON_PEND_DATA_IN_TX_WM

DESCRIPTION
  This function attempts to reoriginate from dormancy if there is pending data
  in Tx WM.
  
PARAMETERS  
  Bearer context ptr
 
DEPENDENCIES 
  None.
  
RETURN VALUE 
  void 
 
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_bearer_cntxt_reorig_from_dorm_on_pend_data_in_tx_wm
(
  ds_bearer_mgr_s  *bearer_mgr_p
)
{
  /*----------------------------------------------------------------------- 
    If there is pending data in Tx WM's trigger reorig request
    ----------------------------------------------------------------------*/

  if( (bearer_mgr_p->tx_wm.wm_item.current_cnt != 0) && 
      (ds_pdn_hdlr_get_ps_data_suspend_flag() == FALSE) )
  {
    ds_bearer_mgr_reorig_dorm(bearer_mgr_p); 
  }

  return;
} /*ds_bearer_cntxt_reorig_from_dorm_on_pend_data_in_tx_wm */

/*===========================================================================
FUNCTION DS_BEARER_MGR_GET_RETRY_TIMER_VAL

DESCRIPTION
  This function gets the retry timer value for rab re-establishment
  
PARAMETERS  
  consult_throt_sm - boolean flag indicating if we should consult throt sm
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  timer val (uint32)
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
uint32 ds_bearer_mgr_get_retry_timer_val
(
  boolean                consult_throt_sm
)
{

  void                 *sm_ptr;      
  uint32               throt_timer_val;
  uint32               timer_val;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  /*----------------------------------------------------------------------------
    First get a random number between a 100 and 400 ms
	  If access barring rab retry is supported simply set the rab retry timer to
    DS_BEARER_CNTXT_RAB_REESTABLISH_MIN_RETRY_TIME
  ----------------------------------------------------------------------------*/
  if (!ds_bearer_info->access_barring_rab_retry)
  {
    timer_val = ds_bearer_mgr_generate_16bit_rand_num_by_range(
                     DS_BEARER_MGR_RAB_REESTABLISH_MIN_RETRY_TIME,
                       DS_BEARER_MGR_RAB_REESTABLISH_MIN_RETRY_TIME * 4);
  }
  else
  {
    timer_val = ds_bearer_info->access_barring_rab_retry_timer;
  }

  if(consult_throt_sm == TRUE)
  {
    /*----------------------------------------------------------------------
      We have to consult the SM since throttling has kicked in.
      Get a handle to the system wide throttling object. This is an opaque
      handle.
      Call this function with argument FALSE meaning we need not create a sm
      if it does not already exists. Here we are interested in only retrieving
      the SM for getting the back off timer
    ----------------------------------------------------------------------*/
    sm_ptr = ds_3gpp_srv_req_get_throt_sm(FALSE);

    if(ds_3gpp_srv_req_throt_get_back_off_timer_val(
                                     sm_ptr,&throt_timer_val) == TRUE)
    {
      if (throt_timer_val != 0) 
      {
        /*----------------------------------------------------------------------
          Throttling algorithm returned a non zero timer
          Use the value as dictated by the back-off algorithm

          If the throttling algorithm returned a zero timer, the random number
          between 100 and 400 ms will be used.
        ----------------------------------------------------------------------*/
        timer_val = throt_timer_val;
      }
      
    } // got a timer value from the throt sm
  }

  return timer_val;
} /* ds_3gpp_bearer_cntxt_get_retry_timer_val */

/*===========================================================================
FUNCTION ds_bearer_mgr_rab_release_ind_hdlr

DESCRIPTION
  This function handles rab release procedure for 3GPP modes.
  
PARAMETERS  
  call_id    - CM call id
  end_param  - call end parameters
  subs_id    - Subscription Id
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  TRUE if success, FALSE otherwise.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_bearer_mgr_rab_release_ind_hdlr
(
  uint8  bearer_id
)
{
  ds_bearer_mgr_s     *bearer_mgr_p = NULL;
  boolean              ret_val = FALSE;
  dsm_watermark_type  *tx_wm_ptr = NULL;       /* Watermark pointer      */
  uint32               bearer_flow_mask = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  /*-------------------------------------------------------------------------
    Get the bearer context from the call ID.
  -------------------------------------------------------------------------*/
  bearer_mgr_p = ds_bearer_mgr_get_bearer_from_eps_id(bearer_id);

  /*-------------------------------------------------------------------------
    Verify that the call instance is valid.
  -------------------------------------------------------------------------*/
  if(ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p))
  {
    DS_MSG2(MSG_LEGACY_HIGH, "RAB Rel Hdlr: Bearer instance:%d, state %d", 
    	       bearer_mgr_p->index, bearer_mgr_p->state);

    ds_eps_bearer_mgr_dereg_pdcp(bearer_mgr_p);
    
    if(bearer_mgr_p->state != DS_BEARER_MGR_STATE_UP_DORMANT)
    {
      /*---------------------------------------------------------------------
           Call is not dormant. Go ahead and put it to dormancy
          ---------------------------------------------------------------------*/
      ds_bearer_mgr_rab_release_pdp_proto_stack(bearer_mgr_p);
  
      tx_wm_ptr = &(bearer_mgr_p->tx_wm.wm_item);
      
      if(tx_wm_ptr->current_cnt < tx_wm_ptr->hi_watermark)
      {
        if(bearer_mgr_p->tx_hi == FALSE)
        {
          bearer_flow_mask |= DS_BEARER_LL_MASK;          
        }
        else
        {
          DS_MSG0(MSG_LEGACY_HIGH,
          	       "WM level is between HI and LO, not enabling flow");
        }
      }

      if(ds_pdn_hdlr_get_ps_data_suspend_flag() == FALSE)
      {
        bearer_flow_mask |= DS_BEARER_CONTROL_MASK;
      }

      if(bearer_flow_mask != 0)
      {
        ds_bearer_mgr_enable_tx_flow(bearer_mgr_p, bearer_flow_mask);
      }

      /*------------------------------------------------------------------
          Change the state of the bearer context to reflect dormancy.
  
          However if the bearer is in PENDING RAB REG state, Post bearer up 
          indication so that we end up posting iface up ind. Also there is no
          need to re-orig immediately. This should happen before the bearer
          state is changed.
         --------------------------------------------------------------------*/
      if(bearer_mgr_p->state == DS_BEARER_MGR_STATE_PEND_RAB_REG)
      {
        /*-------------------------------------------------------------------
                 Post bearer up indication to PDN context.
             -------------------------------------------------------------------*/
        ds_pdn_mgr_bearer_up_ind(bearer_mgr_p->pdn_mgr_ptr, bearer_mgr_p);
        bearer_mgr_p->state = DS_BEARER_MGR_STATE_UP_DORMANT;
        ds_pdn_mgr_execute_cached_deactivation(bearer_mgr_p->pdn_mgr_ptr, bearer_mgr_p);
      }
      else
      {
        /*-------------------------------------------------------------------
              The Bearer is being released from a state other than PEND_RAB_REG
     
              If data is sitting in the Watermarks (PS to PDCP), trigger a rab 
              re-establishment immly. We could have lost Radio Link and
              transitioned to dormancy or the network explicily put us dormant
              when we had data to send. Just call the each enqueue function that
              would trigger rab re-estab 
              Change the state of the bearer context to reflect dormancy before 
              calling reorig_dorm. reorig_dorm checks if the bearer is in dormant
             -------------------------------------------------------------------*/
        bearer_mgr_p->state = DS_BEARER_MGR_STATE_UP_DORMANT;
        ds_bearer_cntxt_reorig_from_dorm_on_pend_data_in_tx_wm(bearer_mgr_p);
      } // if bearer in PEND_RAB_REG state
    }
    else
    {
      /*---------------------------------------------------------------------
           Even if a RAB is received in DORMANT state check for any pending data
           and trigger re origination
          ---------------------------------------------------------------------*/  
      ds_bearer_cntxt_reorig_from_dorm_on_pend_data_in_tx_wm(bearer_mgr_p);
    } /* Bearer state is not in UP DORMANT*/

    ds_eps_bearer_mgr_send_trm_priority_ind(bearer_mgr_p,
                                            LTE_TRM_PRIORITY_LOW );  
    bearer_mgr_p->drb_is_up = FALSE;
    ds_pdn_cp_data_process_rab_release(bearer_mgr_p->index);
    
    ret_val = TRUE;

    if(ds_bearer_mgr_is_data_pending() == FALSE)
    {
      ds_pdn_psm_ready_ind();
    }
  } /* bearer pointer is non-null*/

  return ret_val;
} /* ds_bearer_mgr_rab_release_ind_hdlr */

/*===========================================================================
FUNCTION ds_bearer_mgr_rab_re_estab_rej_hdlr

DESCRIPTION
  This function handles rab reject indication from NAS.
  
PARAMETERS  
  emm_drb_reestablish_reject_ind_s_type - RAB reject info ptr
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None
   
SIDE EFFECTS 
  None.  
===========================================================================*/
void ds_bearer_mgr_rab_re_estab_rej_hdlr
(
  emm_drb_reestablish_reject_ind_s_type  *rab_rej_info
)
{
  ds_bearer_mgr_s     *bearer_mgr_p = NULL; 
  ds_pdn_mgr_s        *pdn_mgr_p = NULL;
  uint32               ran_num;   
  void                *sm_ptr = NULL;
  ds_bearer_deact_params_s  info_ptr = {0};
  boolean                   deprior_started = FALSE;
  ds_3gpp_throt_rab_info_s  rab_throt_info = {0};
  ds_3gppi_throttle_sm_s*   core_sm_ptr = NULL;
  uint32                    flow_mask = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Get the bearer context from call id
  -------------------------------------------------------------------------*/
  bearer_mgr_p =  ds_bearer_mgr_get_bearer_from_eps_id(rab_rej_info->eps_bearer_id);
  if(!ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p))
  {
    return;
  }
  
  pdn_mgr_p = bearer_mgr_p->pdn_mgr_ptr;
  if (!ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p))
  {
    return;
  }
    
  DS_MSG2(MSG_LEGACY_HIGH, "RAB reestab reject for bearer %d, state %d",
          bearer_mgr_p->index, bearer_mgr_p->state);

  /*-------------------------------------------------------------------------
   * Reset the service_request_pending_flag if the response if obtained as a
   * result of trying to reorig using this bearer
  -------------------------------------------------------------------------*/
  if(bearer_mgr_p->srv_rsp_exp == TRUE)
  {
    ds_bearer_info->service_request_pending_flag = FALSE;
    bearer_mgr_p->srv_rsp_exp = FALSE;

    /*-------------------------------------------------------------------------
     * Perform throttling if the UE is not in AC 11_15 mode
    -------------------------------------------------------------------------*/
    if( !(ds_pdn_hdlr_is_ue_in_ac11to15() && 
    	     ds_pdn_nv_manager_is_rel_11_or_above_supported()) )
    {
      info_ptr.emm_failure_cause = rab_rej_info->emm_failure_cause;
      info_ptr.esm_local_cause = rab_rej_info->esm_local_cause;
      
      ds_eps_bearer_mgr_examine_if_throt_needed(&info_ptr, &rab_throt_info);
    }

    if(rab_throt_info.throttle_needed == TRUE)
    {
      /*----------------------------------------------------------------------
        Get a handle to the system wide throttling object. This is an opaque
        handle.
      ----------------------------------------------------------------------*/
      sm_ptr = ds_3gpp_srv_req_get_throt_sm(TRUE);

      /*----------------------------------------------------------------------
        Now feed the failure into the system wide throttling state machine.
        Get the timer value to use from the sm

        Call this function with argument TRUE meaning we need to create a sm
        if it does not already exists. 
      ----------------------------------------------------------------------*/
      ds_3gpp_srv_req_throt_feed_failure(sm_ptr,&rab_throt_info);
    }
    else
    {
      /*-------------------------------------------------------------------
       Some non lower layer failure(LLF) may happen but throttling is not
       required.
       Reset the lower layer failure counter if non-LLF happens
      --------------------------------------------------------------------*/
         
      if (!rab_throt_info.lower_layer_failure)
      {
        core_sm_ptr = ds_3gpp_srv_req_throt_get_core_sm_for_current_plmn();
        if (core_sm_ptr != NULL)
        {
            core_sm_ptr->lwr_lyr_failure_cnt = 0;
        }
      }
    }

  } /* Service request expected on this bearer */
  else
  {
    /*-----------------------------------------------------------------------
      We do not need to handle rab_reestab_rej if serv_req is not pending .  
      Alt: skip handling rab_reestab_rej if bearer state = UP_DORMANT_REESTAB                                                                                                                                         
    -----------------------------------------------------------------------*/
    if(ds_bearer_info->service_request_pending_flag == FALSE) 
    {
      /*---------------------------------------------------------------------
        Handling race condition scenario:         
        MH sends REESTAB_REQ while REESTAB_IND is in cmd Q
        MH processes the REESTAB_IND
        ESM sends RAB_REESTAB_REJ since the DRB is already setup.       
      ---------------------------------------------------------------------*/
      DS_ERROR_LOG_0("Ignoring RAB_REESTAB_REJ, "
                     "service_request_pending_flag is FALSE");
      return;
    }
  }

  /*-------------------------------------------------------------------------
    Reset the state of the bearer context to be DORMANT 
  -------------------------------------------------------------------------*/
  bearer_mgr_p->state = DS_BEARER_MGR_STATE_UP_DORMANT;
  (bearer_mgr_p->tx_wm.wm_item).each_enqueue_func_ptr =
               (wm_cb_type)ds_bearer_mgr_reorig_dorm_call_cb;
  (bearer_mgr_p->tx_wm.wm_item).each_enqueue_func_data =  
                                        (void*) bearer_mgr_p;

  /*--------------------------------------------------------------------- 
   Check for EMM cause code
   If EMM cause type is LTE_NAS_IRAT_CONN_EST_FAILURE and emm cause is
   LTE_RRC_CONN_EST_FAILURE_ACCESS_BARRED with an access barring time
   of 0 ms, DS should do an infinite retry mechanism at an interval of
   100 ms or greater
   ---------------------------------------------------------------------*/   
  if ((rab_rej_info->emm_failure_cause.cause_type == LTE_NAS_IRAT_CONN_EST_FAILURE) &&
      (rab_rej_info->emm_failure_cause.cause.est_status == 
       LTE_RRC_CONN_EST_FAILURE_ACCESS_BARRED) &&
      (rab_rej_info->emm_failure_cause.access_barring_rem_time == 
       ACCESS_BARRING_INFINITE_RETRY_TIME))
  {
    DS_MSG0(MSG_LEGACY_HIGH, "ACCESS BARRING RAB Reject, INFINITE RAB RETRY");
    ds_bearer_info->access_barring_rab_retry = TRUE;
  }
#ifdef FEATURE_NBIOT_NTN
  if ((rab_rej_info->emm_failure_cause.cause_type == LTE_NAS_IRAT_INVALID_STATE) &&
      (rab_rej_info->emm_failure_cause.cause.est_status == 
                               LTE_RRC_CONN_EST_FAILURE_NTN_GNSS_FIX_INVALID))
  {
    DS_MSG0(MSG_LEGACY_HIGH, " GNSS failure RAB Reject");
    ds_pdn_mgr_set_ntn_gnss_status(TRUE);
    return;
  }
#endif /* FEATURE_NBIOT_NTN */

  if(rab_rej_info->emm_failure_cause.deprior_started)
  {
    deprior_started = TRUE;
  }

  /*-------------------------------------------------------------------- 
   Check if the RAB_REJ is because RAU/TAU are in progress.
  If they are set emm_reg in progress to TRUE and no data is flushed            .
  ---------------------------------------------------------------------*/
  if((rab_rej_info->emm_failure_cause.cause_type == 
  	                                   LTE_NAS_IRAT_INVALID_STATE) &&
     (rab_rej_info->emm_failure_cause.cause.state == 
                                      EMM_INVALID_STATE_REG_IN_PROGRESS))
  {
    DS_MSG0(MSG_LEGACY_HIGH, "Rab Reject because EMM registration in progress");
    ds_bearer_info->ds_3gpp_reg_in_progress = TRUE;
  }

  /*-------------------------------------------------------------------------  
    Send Phy link down ind to PS IFACE.
  -------------------------------------------------------------------------*/
  ds_bearer_mgr_enable_tx_flow(bearer_mgr_p, DS_BEARER_CONTROL_MASK);

  if (ds_pdn_hdlr_get_ps_data_suspend_flag() == TRUE) 
  {
    /*-----------------------------------------------------------------------
      As we are suspended, no need to retry rab reestablishment.      
    -----------------------------------------------------------------------*/
    DS_MSG0(MSG_LEGACY_HIGH, "RAB_REESTAB_REJ while ds_3gpp_data_suspend");
    return;
  }

  if(ds_pdn_mgr_is_barring_timer_running())
  {
    flow_mask = ds_bearer_mgr_get_bearer_flow_mask(bearer_mgr_p);
    if((flow_mask & DS_BEARER_MO_DATA_BARRING_MASK) && 
       (flow_mask & DS_BEARER_MO_EX_DATA_BARRING_MASK))
    {
      DS_MSG1(MSG_LEGACY_HIGH,"barring timer is running with flow mask %d", flow_mask);
      return;
    }
    else if((flow_mask & DS_BEARER_MO_DATA_BARRING_MASK))
    {
       if (!ds_pdn_cp_data_pending_mo_ex_data_avail(bearer_mgr_p->index)) 
       {
         DS_MSG1(MSG_LEGACY_HIGH,"barring timer is running with flow mask %d", flow_mask);
         return;
       }
    }
  }
  /*-------------------------------------------------------------------------    
    If the bit mask is non-NULL and the timer is not running, then get the 
    timer value from the function. This function return the timer val based
    on the retry/throttle algorithm. In the absense of the algorithm, it
    returns a random number between 100 and 400 ms
  -------------------------------------------------------------------------*/
  if(ds_bearer_info->uplink_data_bitmask != 0)
  {
    /*-------------------------------------------------------------------------    
     Rab re-estab retry  timer will be set even if the retry cnt is Zero.
     this is to avoid immediate Rab re-estab req if there is any Up Link data
    ------------------------------------------------------------------------*/
    if((ds_bearer_info->timer_running == FALSE) && 
       (ds_bearer_info->service_request_pending_flag == FALSE))
    {

      ran_num = ds_bearer_mgr_get_retry_timer_val(
                                  rab_throt_info.throttle_needed);
      
      /*-------------------------------------------------------------------- 
      If ran num > 400 ms, it means that ran timer is the throttling timer 
      In this case set the state to RAB_THROTTLED and start the throttling 
      timer 
      else 
      Check for rrc barring deprioritization 
      If deprior is not started then start a random timer (100-400ms)
      else
      run a guard timer of 5 secs
      --------------------------------------------------------------------*/
      if (ran_num > (DS_BEARER_MGR_RAB_REESTABLISH_MIN_RETRY_TIME * 4))
      {
        ds_3gpp_rab_reestab_transition_state_ev(DS_3GPP_PROC_RAB_REESTAB_THROTTLED,
                                            (void *)ran_num);
      }
      else if(deprior_started)
      {
        DS_MSG0(MSG_LEGACY_HIGH,"Deprior started");
        ds_3gpp_rab_reestab_transition_state_ev(DS_3GPP_PROC_RAB_REESTAB_REJ_DEPRIOR,
                                            (void *)DS_3GPP_RAB_GUARD_TIMER_INTERVAL);
      }
      else
      {
        ran_num = ds_bearer_mgr_get_retry_timer_val(rab_throt_info.throttle_needed);

        DS_MSG1(MSG_LEGACY_HIGH, "Starting back off timer for %d msec",ran_num);
        
        ps_timer_start(ds_bearer_info->tmr_rab_reestab, (int64)ran_num);
        ds_3gpp_rab_reestab_transition_state_ev(
                                  DS_3GPP_PROC_RAB_REESTAB_RAND_RETRY,
                                              (void *)ran_num);
        ds_bearer_info->timer_running = TRUE;      
      }
    }
  }
  else
  {
    if(ds_bearer_info->timer_running == TRUE)
    {
      ps_timer_cancel(ds_bearer_info->tmr_rab_reestab);
      ds_bearer_info->timer_running = FALSE;   
    }
  }

  return;
} /* ds_bearer_mgr_rab_re_estab_rej_hdlr() */

/*===========================================================================
FUNCTION ds_bearer_mgr_suspend_ind

DESCRIPTION
  This function is to perform any mode specific operations on data suspend
  indication.
  
PARAMETERS  
  send_ps_data_avail - we are not allowed to send data_available_ind
  if we are OOS or in limited service
  subs_id            - Subscription Id
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_bearer_mgr_suspend_ind
(
  void
)
{
  ds_bearer_mgr_s        *bearer_mgr_p= NULL;
  uint8                   index = 0;  
  dsm_watermark_type     *tx_wm_ptr = NULL; /* Watermark pointer      */
  boolean                 send_ps_data_avail = FALSE;
/*-----------------------------------------------------------------------*/

  if(ds_pdn_cp_data_need_keep_flow_enabled_for_mox_data())
  {
    send_ps_data_avail = TRUE;
  }
  else
  {
    send_ps_data_avail = ds_pdn_hdlr_is_ps_data_avail_ind_allowed();
  }
  
  /*--------------------------------------------------------------------------
    Call the mode specific suspend handler for each bearer.
  --------------------------------------------------------------------------*/
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
      if(bearer_mgr_p->pdn_mgr_ptr->nw_mode == SYS_SYS_MODE_GSM)
      {        
        /*----------------------------------------------------------------
                Deregister UL and DL watermarks with SNDCP
              ----------------------------------------------------------------*/
        ds_gprs_mgr_dereg_wm_with_sndcp(bearer_mgr_p);        
        bearer_mgr_p->state = DS_BEARER_MGR_STATE_UP_DORMANT;
      }

      (bearer_mgr_p->tx_wm.wm_item).each_enqueue_func_ptr =
                         (wm_cb_type)ds_bearer_mgr_reorig_dorm_call_cb;
      (bearer_mgr_p->tx_wm.wm_item).each_enqueue_func_data =
                                                  (void*) bearer_mgr_p;

      /*---------------------------------------------------------------------
        If there is data in the tx watermark, send the data available
        indication to NAS right away.
      ---------------------------------------------------------------------*/
      tx_wm_ptr = &(bearer_mgr_p->tx_wm.wm_item);
      if ((!ds_pdn_hdlr_get_sent_ps_data_avail_flag())   &&
          (tx_wm_ptr->current_cnt > 0) &&
          (send_ps_data_avail)
         )
      {        
        ds_bearer_mgr_send_ps_data_avail();
      }

      /*---------------------------------------------------------------
        If sent_ps_data_avail is set to TRUE, disable flow on the
        bearer to stop too much data from accumulating in the
        watermark.
      ---------------------------------------------------------------*/
      if (!send_ps_data_avail || 
          (ds_pdn_hdlr_get_sent_ps_data_avail_flag() == TRUE))
      {
        /*-------------------------------------------------------------
             Disable flow.
             -------------------------------------------------------------*/
        ds_bearer_mgr_disable_tx_flow(bearer_mgr_p, DS_BEARER_CONTROL_MASK);
      }
      else
      {
        /*-------------------------------------------------------------
             We are allowed to send data_available_ind and there is no 
             data in watermarks.     
             -------------------------------------------------------------*/
        ds_bearer_mgr_enable_tx_flow(bearer_mgr_p, DS_BEARER_CONTROL_MASK);
      }
      
      DS_MSG2(MSG_LEGACY_HIGH, "Bearer_inst: %d, UM watermark cnt: %d",
              index , bearer_mgr_p->tx_wm.wm_item.current_cnt);

    }// if bearer is UP/UP_DORMANT/UP_DORMANT_REESTAB
  }//end for each bearer  
} /* ds_bearer_cntxt_suspend_ind */

/*===========================================================================
FUNCTION ds_bearer_mgr_resume_ind

DESCRIPTION
  This function is to perform any mode specific operations on data resume
  indication.
  
PARAMETERS  
  None
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_bearer_mgr_resume_ind
(
  void
)
{
  ds_bearer_mgr_s        *bearer_mgr_p= NULL;
  uint8                   index = 0;  
  dsm_watermark_type     *tx_wm_ptr = NULL; /* Watermark pointer      */
/*-----------------------------------------------------------------------*/
  
  /*--------------------------------------------------------------------------
    Call the mode specific suspend handler for each bearer.
  --------------------------------------------------------------------------*/
  for( index=0; index<DS_3GPP_MAX_BEARER_CONTEXT; index++ )
  {
    bearer_mgr_p = ds_bearer_mgr_get_instance_ptr(index);
    if(!ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p))
    {
      continue;
    }

    tx_wm_ptr = &(bearer_mgr_p->tx_wm.wm_item);
    if((bearer_mgr_p->state == DS_BEARER_MGR_STATE_UP_DORMANT_REESTAB) ||
       (bearer_mgr_p->state == DS_BEARER_MGR_STATE_UP_DORMANT) ||
       (bearer_mgr_p->state == DS_BEARER_MGR_STATE_UP))
    {
      
      if(bearer_mgr_p->pdn_mgr_ptr->nw_mode == SYS_SYS_MODE_LTE)
      {        
        if(bearer_mgr_p->state == DS_BEARER_MGR_STATE_UP)
        {
          tx_wm_ptr->each_enqueue_func_ptr = NULL;
          tx_wm_ptr->each_enqueue_func_data = NULL;
        }

        ds_bearer_mgr_enable_tx_flow(bearer_mgr_p, DS_BEARER_CONTROL_MASK);

        if(bearer_mgr_p->state == DS_BEARER_MGR_STATE_UP_DORMANT)
        {          
          /*--------------------------------------------------------
                  If data is sitting in the Watermarks (PS to PDCP), trigger 
                  rab re-establishment immly. 
                --------------------------------------------------------*/
          if( tx_wm_ptr->current_cnt > 0 )
          {
            DS_MSG1(MSG_LEGACY_HIGH, 
            	       "Data in WM on resume ind for bearer %d", index);
            ds_bearer_mgr_reorig_dorm(bearer_mgr_p); 
          }
          else
          {
            /*-----------------------------------------------------------------  
              Register each enqueue function ptr.
            -----------------------------------------------------------------*/
            tx_wm_ptr->each_enqueue_func_ptr =
                     (wm_cb_type)ds_bearer_mgr_reorig_dorm_call_cb;
            tx_wm_ptr->each_enqueue_func_data = (void*) bearer_mgr_p;
          }
        }
      }
      else if(bearer_mgr_p->pdn_mgr_ptr->nw_mode == SYS_SYS_MODE_GSM)
      {
        tx_wm_ptr->each_enqueue_func_ptr = NULL;
        tx_wm_ptr->each_enqueue_func_data = NULL;
        /*----------------------------------------------------------------
                Register UL and DL watermarks with SNDCP
              ----------------------------------------------------------------*/
        ds_gprs_bearer_mgr_resume_sndcp_reg_req(bearer_mgr_p);        
      }
    }// if bearer is UP/UP_DORMANT/UP_DORMANT_REESTAB
  }//end for each bearer  
} /* ds_bearer_mgr_resume_ind */

/*===========================================================================
FUNCTION  DS_BEARER_MGR_TEAR_DOWN_BEARER

DESCRIPTION
  This function handles bearer tear down as part of instance down handling. 

PARAMETERS
  Bearer ptr

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_bearer_mgr_tear_down_bearer
(
  ds_pdn_mgr_s       *pdn_mgr_p,
  ds_bearer_mgr_s    *bearer_mgr_p  
)
{
  if(!ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p) ||
  	  !ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p))
  {
    return;
  }
  
  if(pdn_mgr_p->num_active_bearer_context == 0)
  {
    DS_ERROR_LOG_0("No bearers associated to PDN");
    return;
  }

  DS_MSG2(MSG_LEGACY_HIGH,
          "Bearer tear down. Bearer state: %d, Bearer instance %d",
          bearer_mgr_p->state, bearer_mgr_p->index);

  // Prepare for tear down
  ds_bearer_mgr_unset_ul_data_bit_mask(bearer_mgr_p);
  bearer_mgr_p->rab_reestab_retry_cnt = 0;

  // OTA disconnect if required
  if(pdn_mgr_p->nw_mode == SYS_SYS_MODE_LTE)
  {
    ds_eps_bearer_mgr_tear_down_pdn_connection(pdn_mgr_p, bearer_mgr_p);
  }
  else if(pdn_mgr_p->nw_mode == SYS_SYS_MODE_GSM)
  {
    ds_gprs_mgr_tear_down_pdn_connection(bearer_mgr_p);
  }

  if(bearer_mgr_p->state != DS_BEARER_MGR_STATE_DOWN)
  {
    bearer_mgr_p->state = DS_BEARER_MGR_STATE_GOING_DOWN;
  }
  return;
}/* ds_bearer_mgr_tear_down_bearer */

/*===========================================================================

FUNCTION DS_BEARER_MGR_LOCAL_CLEANUP

DESCRIPTION
  This function peforms local call cleanup

PARAMETERS  
  Bearer Context ptr

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_bearer_mgr_local_cleanup 
(
  ds_bearer_mgr_s   *bearer_mgr_p
)
{
  /*-----------------------------------------------------------------------*/

  DS_MSG1(MSG_LEGACY_HIGH, "Performing local call cleanup for bearer %d", 
          bearer_mgr_p->index);

  ds_bearer_mgr_enable_tx_flow(bearer_mgr_p, DS_BEARER_CONTROL_MASK);

  if (bearer_mgr_p->is_default)
  {
    /*-------------------------------------------------------------------
      Error in setting up default bearer -> Make sure that we call flow 
      go null ind on default flows of all ifaces associated with the PDN 
      context          
    --------------------------------------------------------------------*/
    ds_flow_unbind_default_flows(bearer_mgr_p->pdn_mgr_ptr);     
    
  } 

  /*-------------------------------------------------------------------
    Deregister bearer with FWK/A2 in case it was registered before.
  --------------------------------------------------------------------*/
  ds_fwk_dereg_bearer_with_hw(bearer_mgr_p);

  bearer_mgr_p->state = DS_BEARER_MGR_STATE_DOWN;
  ds_pdn_mgr_bearer_down_ind(bearer_mgr_p->pdn_mgr_ptr,
  	                          bearer_mgr_p,
  	                          DS_NET_DOWN_REASON_NOT_SPECIFIED);
}/* ds_bearer_mgr_local_cleanup */

/*===========================================================================
FUNCTION DS_BEARER_MGR_ENABLE_TX_FLOW
 
DESCRIPTION
  This function processes the TX flow enable event posted to bearer manager.

PARAMETERS 
  Bearer pointer
  mask - Flow mask
 
DEPENDENCIES 
  None. 
  
RETURN VALUE 
  None
 
SIDE EFFECTS 
  None
===========================================================================*/
void ds_bearer_mgr_enable_tx_flow
(
  ds_bearer_mgr_s     *bearer_p,
  uint32               mask
)
{
  ds_pdn_event_info_u_type event_info;
  ds_pdn_mgr_instance_type_e  inst_type;

  if( ds_bearer_mgr_validate_bearer_ptr(bearer_p) == FALSE ||
      bearer_p->tx_flow_mask == DS_BEARER_ALL_FLOWS_ENABLED )
  {
    return;
  }

  memset(&event_info,0,sizeof(ds_pdn_event_info_u_type));

  DS_MSG2(MSG_LEGACY_HIGH, "Enabling flow on bearer %d , unset mask 0x%x",
          bearer_p->index, mask);

  bearer_p->tx_flow_mask &= ~(mask);

  if(bearer_p->tx_flow_mask == DS_BEARER_ALL_FLOWS_ENABLED)
  {
    event_info.bearer_flow_mask = bearer_p->tx_flow_mask;
    // Bearer flow event to DS FWK    
    for(inst_type = DS_PDN_INSTANCE_IPV4;
        inst_type < DS_PDN_INSTANCE_MAX;
        inst_type++)
    {
      ds_fwk_notify_events( bearer_p->pdn_mgr_ptr->fw_index,
                           ds_pdn_mgr_convert_pdn_inst_to_fw_ip_type(inst_type),
                            BEARER_FLOW_ENABLED_EV,
                            event_info);
     }
  }

#ifdef TEST_FRAMEWORK
	#error code not present
#endif

  DS_MSG1(MSG_LEGACY_HIGH, "Current flow mask 0x%x", bearer_p->tx_flow_mask);
  return;
}

/*===========================================================================
FUNCTION DS_BEARER_MGR_DISABLE_TX_FLOW
 
DESCRIPTION
  This function processes the TX flow disable event posted to bearer manager.

PARAMETERS 
  Bearer ptr
  mask - Flow mask
 
DEPENDENCIES 
  None. 
  
RETURN VALUE 
  None
 
SIDE EFFECTS 
  None
===========================================================================*/
void ds_bearer_mgr_disable_tx_flow
(
  ds_bearer_mgr_s   *bearer_p,
  uint32             mask
)
{
  ds_pdn_event_info_u_type event_info;
  ds_pdn_mgr_instance_type_e  inst_type;

  DS_MSG2(MSG_LEGACY_HIGH, "Disabling flow on bearer %d , set mask 0x%x",
          bearer_p->index , mask);
  
  memset(&event_info,0,sizeof(ds_pdn_event_info_u_type));
  if(( bearer_p->tx_flow_mask == DS_BEARER_ALL_FLOWS_ENABLED ) ||
      !(bearer_p->tx_flow_mask & mask))
  {
    event_info.bearer_flow_mask = mask;
    // Bearer flow event to DS FWK    
    for(inst_type = DS_PDN_INSTANCE_IPV4;
        inst_type < DS_PDN_INSTANCE_MAX;
        inst_type++)
    {
      ds_fwk_notify_events( bearer_p->pdn_mgr_ptr->fw_index,
                           ds_pdn_mgr_convert_pdn_inst_to_fw_ip_type(inst_type),
                            BEARER_FLOW_DISABLED_EV,
                            event_info); 
    }
  }

  bearer_p->tx_flow_mask |= mask;

#ifdef TEST_FRAMEWORK
  #error code not present
#endif
  DS_MSG1(MSG_LEGACY_HIGH, "Current flow mask 0x%x", bearer_p->tx_flow_mask);
  return;
}

#ifdef TEST_FRAMEWORK
#error code not present
#endif


/*===========================================================================
FUNCTION DS_BEARER_MGR_IS_TX_FLOW_ENABLED
 
DESCRIPTION
  This function returns the TX flow status of the bearer.

PARAMETERS 
  Bearer pointer
 
DEPENDENCIES 
  None. 
  
RETURN VALUE 
  boolean
 
SIDE EFFECTS 
  None
===========================================================================*/
boolean ds_bearer_mgr_is_tx_flow_enabled
(
  ds_bearer_mgr_s     *bearer_p
)
{
  return ds_bearer_mgr_is_tx_flow_enabled_ex(bearer_p, FALSE);
}

/*===========================================================================
FUNCTION DS_BEARER_MGR_IS_TX_FLOW_ENABLED_EX
 
DESCRIPTION
  This function returns the TX flow status of the bearer.
  with aditional check for mo_ex_data.
  will return TRUE when MO_EX data is pending and flow is disabled with
  only DS_BEARER_CP_DATA_BACKOFF_MASK.

PARAMETERS 
  Bearer pointer
  is_mo_ex_data
 
DEPENDENCIES 
  None. 
  
RETURN VALUE 
  boolean
 
SIDE EFFECTS 
  None

===========================================================================*/
boolean ds_bearer_mgr_is_tx_flow_enabled_ex
(
  ds_bearer_mgr_s     *bearer_p,
  boolean              is_mo_ex_data
)
{
  if(ds_bearer_mgr_validate_bearer_ptr(bearer_p))
  {
    if(bearer_p->tx_flow_mask == DS_BEARER_ALL_FLOWS_ENABLED)
    {
      return TRUE;
    }
	
#ifdef TEST_FRAMEWORK
	  #error code not present
#endif

    if(bearer_p->tx_flow_mask & 
         (DS_BEARER_LL_MASK|DS_BEARER_CONTROL_MASK|
  	  DS_BEARER_APN_RATE_CTRL_MASK|DS_BEARER_SPLMN_RATE_CTRL_MASK|
          DS_BEARER_MODIFICATION_IN_PROGRESS_MASK|DS_BEARER_MEM_FLOW_CTL_MASK|
          DS_BEARER_MO_EX_DATA_BARRING_MASK))
    {
      return FALSE;
    }

    if(bearer_p->tx_flow_mask & DS_BEARER_CP_DATA_BACKOFF_MASK )
    {
      if(is_mo_ex_data || 
         (bearer_p->drb_is_up && 
          (!(bearer_p->tx_flow_mask & DS_BEARER_TECH_SUSPENSION_MASK) &&
           !(bearer_p->tx_flow_mask & DS_BEARER_APN_RATE_CTRL_MOX_ONLY_MASK) &&
           !(bearer_p->tx_flow_mask & DS_BEARER_MO_DATA_BARRING_MASK)
#ifdef FEATURE_GPS_WWAN_PRIORITY_SEARCH
           &&!(bearer_p->tx_flow_mask & DS_BEARER_GPS_SEARCH_IN_PROGRESS_MASK)
#endif /*FEATURE_GPS_WWAN_PRIORITY_SEARCH*/
          )))     
      {
        return TRUE;
      }      
    }

    if(bearer_p->tx_flow_mask & 
        (DS_BEARER_TECH_SUSPENSION_MASK|DS_BEARER_APN_RATE_CTRL_MOX_ONLY_MASK|
         DS_BEARER_MO_DATA_BARRING_MASK 
#ifdef FEATURE_GPS_WWAN_PRIORITY_SEARCH
        | DS_BEARER_GPS_SEARCH_IN_PROGRESS_MASK
#endif /*FEATURE_GPS_WWAN_PRIORITY_SEARCH*/
        ))
    {
      if(is_mo_ex_data)     
      {
        return TRUE;
      }      
    }
  }

  return FALSE;
}

/*===========================================================================
FUNCTION DS_BEARER_MGR_WM_FLOW_DISABLE_CB

DESCRIPTION
  This function sends a flow disable message to ds task
  
PARAMETERS  
  Watermark pointer,
  Bearer instance
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.  
===========================================================================*/
void ds_bearer_mgr_wm_flow_disable_cb
(
 dsm_watermark_type *wm,
 void*               callback_data
)
{
  ds_bearer_mgr_s   *bearer_mgr_p = NULL;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  DS_MSG0(MSG_LEGACY_HIGH,"High WM CB !");

  bearer_mgr_p = (ds_bearer_mgr_s*) callback_data;
  if(!ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p))
  {
    return;
  }
 
  /* No need to post message if wm is already in hi state  */
  if (bearer_mgr_p->tx_hi) 
  {
    DS_ERROR_LOG_0("Not posting tx flow disable msg - state not lo");
    return;
  }

  
  /* Set tx wm state to hi  */
  bearer_mgr_p->tx_hi = TRUE;
  ds_bearer_mgr_disable_tx_flow(bearer_mgr_p, DS_BEARER_LL_MASK);

  return;
}/* ds_bearer_mgr_wm_flow_disable_cb */

/*===========================================================================
FUNCTION DS_BEARER_MGR_WM_FLOW_ENABLE_CB

DESCRIPTION
  This function sends a flow enable message to ds task
  
PARAMETERS  
  Watermark ptr
  bearer instance
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.  
===========================================================================*/
void ds_bearer_mgr_wm_flow_enable_cb
(
 dsm_watermark_type *wm,
 void*               callback_data
)
{
  ds_bearer_mgr_s                               *bearer_mgr_p = NULL;
  ds_pdn_mgr_s                                  *pdn_mgr_p = NULL;
  ds_bearer_mgr_um_wm_threshold_struct_type      um_wm_threshold = {0}; 
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  DS_MSG0(MSG_LEGACY_HIGH,"Low WM CB !");

  bearer_mgr_p = (ds_bearer_mgr_s*)callback_data;
  if(!ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p))
  {
    return;
  }

  pdn_mgr_p = bearer_mgr_p->pdn_mgr_ptr;
  if (!ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p))
  {
    return;
  }

  um_wm_threshold =  ds_pdn_nv_get_um_wm_threshold();  
 
  /* No need to post message if wm is already in lo state  */
  if (!bearer_mgr_p->tx_hi) 
  {
    DS_ERROR_LOG_0("Not posting tx flow enable msg - state not hi");
    return;
  }

  if (pdn_mgr_p->nw_mode == SYS_SYS_MODE_LTE)
  {
    if(bearer_mgr_p->tx_wm.wm_item.dont_exceed_cnt != 
                                         um_wm_threshold.lte_ul_dne)
    {
      dsm_set_dne(&bearer_mgr_p->tx_wm.wm_item,
                  um_wm_threshold.lte_ul_dne);
    }
  }    

  if (pdn_mgr_p->nw_mode == SYS_SYS_MODE_GSM)
  {
    if(bearer_mgr_p->tx_wm.wm_item.dont_exceed_cnt != 
                                        um_wm_threshold.gprs_ul_dne)
    {
      dsm_set_dne(&bearer_mgr_p->tx_wm.wm_item,
                  um_wm_threshold.gprs_ul_dne);
    }
  }    
 
  /* Set tx wm state to lo  */
  bearer_mgr_p->tx_hi = FALSE;
  ds_bearer_mgr_enable_tx_flow(bearer_mgr_p, DS_BEARER_LL_MASK);

  return;
} /* ds_bearer_mgr_wm_flow_enable_cb() */

/*===========================================================================
FUNCTION DS_BEARER_MGR_TMR_BACK_PRESSURE_EXP_HDLR

DESCRIPTION
  This is the timer exp hdlr for dynamic back pressure.

PARAMETERS
  user_data_ptr - user data.

DEPENDENCIES
  None.

RETURN VALUE
  None 

SIDE EFFECTS
  None.
===========================================================================*/
void ds_bearer_mgr_tmr_dyn_back_pressure_exp_hdlr
(
  void              *user_data_ptr
)
{
  uint32                 wm_hi_level = 0;
  ds_bearer_mgr_s       *bearer_mgr_p = NULL;
  ds_bearer_mgr_wm_s    *watermark_p    = NULL;
  uint32                 new_ack_cnt = 0;
  ds_pdn_mgr_s          *pdn_mgr_p = NULL;
  ds_bearer_mgr_tput_opt_struct_type tput_opt_params = {0};
  ds_bearer_mgr_um_wm_threshold_struct_type  um_wm_threshold = {0};

  DS_MSG0(MSG_LEGACY_HIGH, "TMR Dynamic BP exp hdlr");

  bearer_mgr_p = (ds_bearer_mgr_s *)user_data_ptr;

  /*------------------------------------------------------------------------
    The timer expiry processing is in PS task context, this code is to 
    prevent a race condition between PS and DS task when timer expires at
    the same time when network ends the data call.
  ------------------------------------------------------------------------*/
  if(!ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p))
  {
    return;
  }

  pdn_mgr_p = bearer_mgr_p->pdn_mgr_ptr;
  if (!ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p))
  {
    return;
  }
  
  tput_opt_params = ds_pdn_nv_get_back_pressure_params();
  um_wm_threshold = ds_pdn_nv_get_um_wm_threshold();

  /*------------------------------------------------------------------------
    Ensure that BP timer is restarted and BP remains disabled only if the 
    number of new ACKs received in the BP timer duration equals the threshold 
    ACK cnt. This ensures that low throughput background TCP DL traffic 
    doesn't trigger BP removal.
  ------------------------------------------------------------------------*/
  new_ack_cnt = (uint32)(bearer_mgr_p->tcp_dl_ack_counter - 
                                bearer_mgr_p->last_tcp_dl_ack_count);

  if (new_ack_cnt < tput_opt_params.tcp_threshold_ack_cnt)
  {
    DS_MSG3(MSG_LEGACY_HIGH,
    	       "Curr_ack_cnt %d, last_ack_cnt: %d. Enabling BP for bearer index %d",
            bearer_mgr_p->tcp_dl_ack_counter,
            bearer_mgr_p->last_tcp_dl_ack_count,
            bearer_mgr_p->index);

    /*----------------------------------------------------------------------
      Reset the Ack counters and enable back pressure
    ----------------------------------------------------------------------*/
    bearer_mgr_p->tcp_dl_ack_counter = 
                    bearer_mgr_p->last_tcp_dl_ack_count = 0;

    bearer_mgr_p->tx_wm.wm_item.hiwater_func_ptr = 
    	                      ds_bearer_mgr_wm_flow_disable_cb;

    /*------------------------------------------------------------------------
      Reset HI_WM to original value
    ------------------------------------------------------------------------*/
    if (pdn_mgr_p->nw_mode == SYS_SYS_MODE_LTE)
    {
      wm_hi_level = um_wm_threshold.lte_ul_hi;
    }
    else if (pdn_mgr_p->nw_mode == SYS_SYS_MODE_GSM)
    {
      wm_hi_level = um_wm_threshold.gprs_ul_hi;
    }
    else
    {
      DS_ERROR_LOG_1("Invalid call_mode: =d ", pdn_mgr_p->nw_mode);
      return;
    }
      
    watermark_p = &(bearer_mgr_p->tx_wm);
    if (NULL != watermark_p)
    {
      dsm_set_hi_wm(&(watermark_p->wm_item), wm_hi_level);
    }
  }
  else
  {
    /* Disable Back Pressure */
    ds_bearer_mgr_disable_back_pressure(bearer_mgr_p);
    
    /* Start the timer */
    ds_bearer_mgr_start_bp_timer(bearer_mgr_p);
  }

  return;
} /* ds_bearer_mgr_tmr_dyn_back_pressure_exp_hdlr */

/*===========================================================================
FUNCTION DS_BEARER_MGR_DISABLE_BACK_PRESSURE

DESCRIPTION
  This function disables back pressure.
  
PARAMETERS  
  *bearer_mgr_p  - bearer_mgr pointer
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
  
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_bearer_mgr_disable_back_pressure
(
  ds_bearer_mgr_s  *bearer_mgr_p
)
{
  uint32 wm_dne_level = 0;
  ds_bearer_mgr_wm_s  *watermark_p = NULL;
  ds_pdn_mgr_s        *pdn_mgr_p = NULL;
  ds_bearer_mgr_um_wm_threshold_struct_type  um_wm_threshold;

  pdn_mgr_p = bearer_mgr_p->pdn_mgr_ptr;
  if (!ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p))
  {
    return;
  }

  DS_MSG1(MSG_LEGACY_HIGH, "Disable back pressure for bearer index: %d",
                           bearer_mgr_p->index);
  /*-------------------------------------------------------------------------
    Disable back pressure as UL Acks are being received
  -------------------------------------------------------------------------*/
  bearer_mgr_p->tx_wm.wm_item.hiwater_func_ptr = NULL;
  um_wm_threshold = ds_pdn_nv_get_um_wm_threshold();

  /*-------------------------------------------------------------------------
    Set HI_WM to DNE
  -------------------------------------------------------------------------*/
  if (pdn_mgr_p->nw_mode == SYS_SYS_MODE_LTE)
  {
    wm_dne_level = um_wm_threshold.lte_ul_dne;
  }
  else if (pdn_mgr_p->nw_mode == SYS_SYS_MODE_GSM)
  {
    wm_dne_level = um_wm_threshold.gprs_ul_dne;
  }
  else
  {
    DS_ERROR_LOG_1("Invalid call_mode: =d", pdn_mgr_p->nw_mode);
    return;
  }

  /*-------------------------------------------------------------------------
    Update the last_tcp_ack_count and start the timer
  -------------------------------------------------------------------------*/
  bearer_mgr_p->last_tcp_dl_ack_count = bearer_mgr_p->tcp_dl_ack_counter;
  
  watermark_p = &(bearer_mgr_p->tx_wm);
  if (NULL != watermark_p)
  {
    dsm_set_hi_wm(&(watermark_p->wm_item), wm_dne_level);
  }

  return;
} /* ds_bearer_cntxt_disable_back_pressure */

/*===========================================================================
FUNCTION  DS_BEARER_MGR_RESET_SRV_REQ_THROT

DESCRIPTION
  This function resets service request throttling SM. This is called during
  RAT change. 

PARAMETERS
  sys_sys_mode_e_type     - new mode.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_bearer_mgr_reset_srv_req_throt
(
  sys_sys_mode_e_type    target_rat
)
{
  uint32                    timerval = 0;
  
  /*---------------------------------------------------------------------
    Clear the service request throttling on RAT change
  ---------------------------------------------------------------------*/	
  ds_3gpp_srv_req_throt_go_null(FALSE);

  timerval = ds_bearer_mgr_get_retry_timer_val(TRUE);
	   
  DS_MSG1(MSG_LEGACY_HIGH,
          "Reset Srv Req throt: throttle timer is running "
          "throttle timer value: %d", timerval);

  if( (ds_bearer_info->timer_running == TRUE) && (timerval > 0) )
  {       
    /*---------------------------------------------------------------------
      Clear the RAB reestablish timer. This will be started again either in
      rab_re_est_ind or in rab_re_est_rej handler or in the registered timer
      call back.
      post DS3GPP_REORIG_DORM_TMR_EXP_CMD command for requesting rab 
      re-establishment on conncted handover               
    -----------------------------------------------------------------------*/
      ps_timer_cancel(ds_bearer_info->tmr_rab_reestab);
      ds_bearer_info->timer_running = FALSE;

      ds_bearer_info->access_barring_rab_retry = FALSE;
      
    if( target_rat == SYS_SYS_MODE_GSM )
    {  
      return;
    }

    ds_bearer_mgr_proc_reorig_dorm_tmr_exp_cmd();
  }
} /* ds_3gpp_bearer_cntxt_reset_srv_req_throt */         		

/*===========================================================================
FUNCTION DS_BEARER_MGR_START_BP_TIMER

DESCRIPTION
  This function starts the BP removal timer
  
PARAMETERS  
  *bearer_mgr_p  - bearer_mgr pointer
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
  
SIDE EFFECTS 
  None.  
===========================================================================*/
void ds_bearer_mgr_start_bp_timer
(
  ds_bearer_mgr_s  *bearer_mgr_p
)
{
  ps_timer_handle_type               tmr_hndl_dyn_back_pressure;
  ds_bearer_mgr_tput_opt_struct_type tput_opt_params;

  tput_opt_params = ds_pdn_nv_get_back_pressure_params();  
  tmr_hndl_dyn_back_pressure = bearer_mgr_p->tmr_hndl_dyn_back_pressure;

  if (TRUE == ps_timer_is_running(tmr_hndl_dyn_back_pressure))
  {
    DS_MSG0(MSG_LEGACY_LOW,
    	       "BP timer is already running, not starting again");
    return;
  }

  if (PS_TIMER_SUCCESS != 
      ps_timer_start(tmr_hndl_dyn_back_pressure, 
                     (1000 * tput_opt_params.timer_val)))
  {
    DS_ERROR_LOG_1("Failed to start timer for bearer index: =d",
                   bearer_mgr_p->index);
  }

  return;
}/* ds_bearer_mgr_start_bp_timer */

/*===========================================================================
FUNCTION DS_BEARER_MGR_SEND_PS_DATA_AVAIL

DESCRIPTION
  This function sends PS data available indication to NAS
  
PARAMETERS  
  None
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
  
SIDE EFFECTS 
  None.  
===========================================================================*/
void ds_bearer_mgr_send_ps_data_avail
(
  void
)
{
  mm_ps_data_avail_req_T  ps_data_avail_req = {NULL};
/*-----------------------------------------------------------------------*/

  ps_data_avail_req.message_header.message_id = MM_PS_DATA_AVAIL_REQ;
  ps_data_avail_req.message_header.message_set = MS_MM_DS;      
  ps_data_avail_req.message_header.message_len_lsb =
    (sizeof( mm_ps_data_avail_req_T ) - sizeof(IMH_T)) % 256;
  ps_data_avail_req.message_header.message_len_msb =
    (sizeof( mm_ps_data_avail_req_T ) - sizeof(IMH_T)) / 256;
   
  ds_pdn_mgr_ds_to_nas_send_msg((void*)&ps_data_avail_req);   

  ds_pdn_hdlr_set_sent_ps_data_avail_flag(TRUE);
  return;
}

/*===========================================================================
FUNCTION DS_BEARER_MGR_IS_ACTIVE_BEARER_CNTXT

DESCRIPTION
  This function returns TRUE if there is an active bearer context in the
  system. Else, it returns FALSE.

PARAMETERS

DEPENDENCIES
  None.

RETURN VALUE
  Boolean.

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_bearer_mgr_is_active_bearer_cntxt
(
  void
)
{
  uint8                 bearer_index;
  ds_bearer_mgr_s*      bearer_mgr_p = NULL;
  ds_bearer_mgr_state_e state = DS_BEARER_MGR_STATE_DOWN;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  for(bearer_index=0;bearer_index < DS_3GPP_MAX_BEARER_CONTEXT;bearer_index++)
  {  
    bearer_mgr_p = ds_bearer_manager[bearer_index]; 
    
    if(!ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p))
    {
      continue;
    }
    
    state = bearer_mgr_p->state;
    
    if (state == DS_BEARER_MGR_STATE_UP ||
    state == DS_BEARER_MGR_STATE_UP_DORMANT ||
    state == DS_BEARER_MGR_STATE_UP_DORMANT_REESTAB ||
    state == DS_BEARER_MGR_STATE_PEND_RAB_REG)
    {
      return TRUE;
    }
  }
  return FALSE;
} /* ds_bearer_mgr_is_active_bearer_cntxt() */

/*===========================================================================
FUNCTION DS_BEARER_MGR_IS_DEF_BEARER_COMING_UP

DESCRIPTION
  This function checks if the default bearer corresponding to the
  given PDN is in COMING UP state.

PARAMETERS
  pdn_cntx_p - PDN Context Ptr

DEPENDENCIES
  None.

RETURN VALUE
  TRUE, if Bearer in COMING state
  FALSE, otherwise

SIDE EFFECTS 
  None.

===========================================================================*/
boolean ds_bearer_mgr_is_def_bearer_coming_up
(
  ds_pdn_mgr_s  *pdn_mgr_p
)
{
  ds_bearer_mgr_s    *def_bearer_mgr_ptr = NULL;
  boolean            ret_val = FALSE;
  /*---------------------------------------------------------*/
  def_bearer_mgr_ptr = ds_pdn_mgr_get_default_bearer(pdn_mgr_p);

  if(ds_bearer_mgr_validate_bearer_ptr(def_bearer_mgr_ptr) && 
     (def_bearer_mgr_ptr->state == DS_BEARER_MGR_STATE_COMING_UP))
  {
    ret_val = TRUE;
  } 
  return ret_val;
} /* ds_bearer_cntxt_is_def_bearer_coming_up */

/*===========================================================================
FUNCTION DS_BEARER_MGR_POST_BEARER_UP_EVENT

DESCRIPTION
  This function post bearer up event to FWK which will be given to clients.

PARAMETERS
  Bearer pointer

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS 
  None.
===========================================================================*/
void ds_bearer_mgr_post_bearer_up_event
(
  ds_bearer_mgr_s  *bearer_mgr_p
)
{
  ds_pdn_event_info_u_type     event_info;
  ds_pdn_mgr_instance_type_e   inst_type;
  /*------------------------------------------------------------------------*/

  memset(&event_info, 0, sizeof(ds_pdn_event_info_u_type));

  if(!ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p) ||
  	  !ds_pdn_mgr_validate_pdn_ptr(bearer_mgr_p->pdn_mgr_ptr))
  {
    return;
  }
  	
  for(inst_type = DS_PDN_INSTANCE_IPV4;
      inst_type < DS_PDN_INSTANCE_MAX;
      inst_type++)
  {
    ds_fwk_notify_events(bearer_mgr_p->pdn_mgr_ptr->fw_index,
                         ds_pdn_mgr_convert_pdn_inst_to_fw_ip_type(inst_type),
                         BEARER_UP_EV,
                         event_info);  
  }

  return;
}

/*===========================================================================
FUNCTION DS_BEARER_MGR_IS_DORMANT_REORIG_REQUIRED

DESCRIPTION
  This function checks if the bearer is dormant and re-origination is required.

PARAMETERS
  Bearer pointer

DEPENDENCIES
  None.

RETURN VALUE
  TRUE, if Bearer is dormant and needs re-origination.
  FALSE, otherwise

SIDE EFFECTS 
  None.

===========================================================================*/
boolean ds_bearer_mgr_is_dormant_reorig_required
(
  ds_bearer_mgr_s  *bearer_mgr_p
)
{
  if(!ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p) ||
  	  ds_pdn_cp_data_is_enabled() == TRUE)
  {
    return FALSE;
  }
  
  if(bearer_mgr_p->state == DS_BEARER_MGR_STATE_UP_DORMANT)
  {
    return TRUE;
  }
  
  return FALSE;
} /* ds_bearer_mgr_is_dormant_reorig_required */

/*===========================================================================
FUNCTION DS_BEARER_MGR_IS_BEARER_WRITABLE

DESCRIPTION
  This function checks if the bearer is writable for UL data.

PARAMETERS
  Bearer pointer

DEPENDENCIES
  None.

RETURN VALUE
  TRUE, if Bearer is writable.
  FALSE, otherwise

SIDE EFFECTS 
  None.
===========================================================================*/
boolean ds_bearer_mgr_is_bearer_writable
(
  ds_bearer_mgr_s  *bearer_mgr_p
)
{
  if(ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p) == FALSE)
  {
    DS_MSG0(MSG_LEGACY_HIGH, "return false");
    return FALSE;
  }

  if(( ds_pdn_cp_data_is_enabled() == TRUE ||
  	    bearer_mgr_p->state == DS_BEARER_MGR_STATE_UP ) &&
  	  (ds_bearer_mgr_is_tx_flow_enabled(bearer_mgr_p)))
  {
    return TRUE;
  }
  
  return FALSE;
} /* ds_bearer_mgr_is_dormant_reorig_required */


/*===========================================================================
FUNCTION DS_BEARER_MGR_GET_RAB_REEST_TIMER

DESCRIPTION
  This function gets the bearer info ptr

PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE
  bearer info ptr

SIDE EFFECTS 
  None.
===========================================================================*/
boolean ds_bearer_mgr_get_rab_reest_timer()
{
  return ds_bearer_info->timer_running;
}

/*===========================================================================
FUNCTION DS_BEARER_MGR_START_RAB_REEST_TIMER

DESCRIPTION
  This function gets the bearer info ptr

PARAMETERS
  timer_val   timer value

DEPENDENCIES
  None.

RETURN VALUE
  bearer info ptr

SIDE EFFECTS 
  None.
===========================================================================*/
void ds_bearer_mgr_start_rab_reest_timer(uint32 timer_val)
{
  ps_timer_start(ds_bearer_info->tmr_rab_reestab, timer_val);
  ds_bearer_info->timer_running = TRUE;	
}

/*===========================================================================
FUNCTION DS_BEARER_MGR_CANCEL_RAB_REEST_TIMER

DESCRIPTION
  This function gets the bearer info ptr

PARAMETERS
   NONE

DEPENDENCIES
  None.

RETURN VALUE
  bearer info ptr

SIDE EFFECTS 
  None.
===========================================================================*/
void ds_bearer_mgr_cancel_rab_reest_timer()
{
  ps_timer_cancel (ds_bearer_info->tmr_rab_reestab);
  ds_bearer_info->timer_running = FALSE;
}
/*===========================================================================
FUNCTION ds_bearer_mgr_get_mem_ctrl_ev_posted_flag

DESCRIPTION
  This function gets the Mem Ctrl Event posted Flag
 
PARAMETERS
  None
    
DEPENDENCIES
  None

RETURN VALUE
  TRUE - If Mem Cntrl Event was already posted
  FALSE- Otherwise

SIDE EFFECTS
  None

=============================================================================*/
static boolean ds_bearer_mgr_get_mem_ctrl_ev_posted_flag
(
  void
)
{
  return ds_bearer_info->mem_ctrl_ev_posted;
} /* ds_bearer_mgr_get_mem_ctrl_ev_posted_flag */

/*===========================================================================
FUNCTION ds_bearer_mgr_set_mem_ctrl_ev_posted_flag

DESCRIPTION
  This function sets the Mem Ctrl Event posted Flag
 
PARAMETERS
  flag: Mem Cntrl Event posted Flag
    
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void ds_bearer_mgr_set_mem_ctrl_ev_posted_flag
(
  boolean flag
)
{
  ds_bearer_info->mem_ctrl_ev_posted = flag;
} /* ds_bearer_cntxt_set_mem_ctrl_ev_posted_flag */

/*===========================================================================

FUNCTION DS_BEARER_MGR_MEM_CTRL_EV_CB

DESCRIPTION
  This function posts cmd to DS task to notify DSM Memory control event.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds_bearer_mgr_mem_ctrl_ev_cb
(
  dsm_mempool_id_enum_type pool_id,            /* memory pool ID */
  dsm_mem_level_enum_type  event,              /* type of memory event */
  dsm_mem_op_enum_type     op
)
{
  ds_command_type *cmd_ptr = NULL;
  boolean     post_cmd = TRUE;
  ds_bearer_mgr_event_type *ds_bearer_mgr_event_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (ds_bearer_mgr_get_mem_ctrl_ev_posted_flag() == FALSE)
  {
    post_cmd = TRUE;
  }
  
  if (post_cmd == TRUE)
  {
    cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_bearer_mgr_event_type));

    if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
    {                                                                      
      ASSERT(0);
      return;                                                              
    }                                                                      

    DS_MSG3(MSG_LEGACY_HIGH, "dsm mem ev cb: free [%d], event [%d], pool_id [%d]", 
                        DSM_POOL_FREE_CNT(pool_id), event, pool_id );                                                           
    cmd_ptr->hdr.cmd_id = DS_CMD_PDN_DSM_MEM_CTRL_EV;
    ds_bearer_mgr_event_ptr = (ds_bearer_mgr_event_type*)cmd_ptr->cmd_payload_ptr;
    ds_bearer_mgr_event_ptr->pool_id = pool_id;
    ds_bearer_mgr_event_ptr->op = op;
    ds_put_cmd( cmd_ptr ); 
    ds_bearer_mgr_set_mem_ctrl_ev_posted_flag(TRUE);
  }
  else
  {
    DS_MSG3(MSG_LEGACY_HIGH,"dsm mem ev cb: post cmd: %d, op: %d, pool_id: %d",
                       post_cmd,op,pool_id);
  }

  return;

} /*ds_3gpp_bearer_cntxt_mem_ctrl_ev_cb*/
/*===========================================================================

FUNCTION DS_BEARER_MGR_PROCESS_MEM_FC_EVENT

DESCRIPTION
  This function flushes PS-L2 watermarks

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds_bearer_mgr_process_mem_fc_event
(
  ds_bearer_mgr_event_type         *ds_bearer_mgr_event_ptr
)
{
  int index =0;
  ds_bearer_mgr_s   *bearer_mgr_p = NULL;
  uint8				  pool_id_mask = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/\
						
  if (ds_bearer_mgr_event_ptr == NULL) 
  {
    return;
  }

  DS_MSG3(MSG_LEGACY_HIGH, "ds_bearer_mgr_process_mem_fc_event: free [%d], op [%d], pool_id [%d]", 
					  DSM_POOL_FREE_CNT(ds_bearer_mgr_event_ptr->pool_id), ds_bearer_mgr_event_ptr->op,
					  ds_bearer_mgr_event_ptr->pool_id );

 	
  if (ds_bearer_mgr_event_ptr->pool_id == DSM_DS_SMALL_ITEM_POOL) 
  {
    pool_id_mask = DS_BEARER_SMALL_ITEM_POOL_MASK;
  }
  else if(ds_bearer_mgr_event_ptr->pool_id == DSM_DS_LARGE_ITEM_POOL)
  {
    pool_id_mask = DS_BEARER_LARGE_ITEM_POOL_MASK;
  }
  else if (ds_bearer_mgr_event_ptr->pool_id == DSM_DUP_ITEM_POOL)
  {
    pool_id_mask = DS_BEARER_DUP_ITEM_POOL_MASK;
  }

  if (ds_bearer_mgr_event_ptr->op == DSM_MEM_OP_NEW) 
  {	   
    ds_bearer_info->pool_id_mask |= pool_id_mask;
  }
  else if (ds_bearer_mgr_event_ptr->op == DSM_MEM_OP_FREE)
  {
    ds_bearer_info->pool_id_mask  &= (~(pool_id_mask)); 
  }
	
	
  for(index=0; index < DS_3GPP_MAX_BEARER_CONTEXT; index++)
  {
    bearer_mgr_p = ds_bearer_mgr_get_instance_ptr(index);
	
    if(!ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p))
    {
      continue;
    }
	
    if (ds_bearer_mgr_event_ptr->op == DSM_MEM_OP_NEW) 
    {
      ds_bearer_mgr_disable_tx_flow(bearer_mgr_p, DS_BEARER_MEM_FLOW_CTL_MASK);
    }
    else if (ds_bearer_mgr_event_ptr->op == DSM_MEM_OP_FREE)
    {
      if (ds_bearer_info->pool_id_mask == 0) 
      {
         ds_bearer_mgr_enable_tx_flow(bearer_mgr_p, DS_BEARER_MEM_FLOW_CTL_MASK);
      }
    }
  }
}

/*===========================================================================
FUNCTION DS_BEARER_MGR_IS_DATA_PENDING

DESCRIPTION
  This function returns TRUE if there is pending data in any of the bearers. 
  Else, it returns FALSE.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  Boolean.

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_bearer_mgr_is_data_pending
(
  void
)
{
  uint8                 bearer_index;
  ds_bearer_mgr_s*      bearer_mgr_p = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  for(bearer_index=0;bearer_index < DS_3GPP_MAX_BEARER_CONTEXT;bearer_index++)
  {  
    bearer_mgr_p = ds_bearer_manager[bearer_index]; 
    
    if(!ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p) ||
       (bearer_mgr_p->state == DS_BEARER_MGR_STATE_DOWN ||
        bearer_mgr_p->state == DS_BEARER_MGR_STATE_GOING_DOWN))
    {
      continue;
    }

    if(bearer_mgr_p->tx_wm.wm_item.current_cnt > 0)
    {
      DS_MSG1(MSG_LEGACY_HIGH, "Pending data in bearer %d", bearer_index);
      return TRUE;
    }
  }
  
  return FALSE;
} /* ds_bearer_mgr_is_data_pending */

/*===========================================================================
FUNCTION DS_BEARER_MGR_GET_BEARER_FLOW_MASK
 
DESCRIPTION
  This function returns the flow mask for the bearer.

PARAMETERS 
  Bearer ptr
 
DEPENDENCIES 
  None. 
  
RETURN VALUE 
  MASK
 
SIDE EFFECTS 
  None
===========================================================================*/

uint32 ds_bearer_mgr_get_bearer_flow_mask
(
  ds_bearer_mgr_s   *bearer_p
)
{
  if(!ds_bearer_mgr_validate_bearer_ptr(bearer_p))
  {
    return 0;
  }
  return bearer_p->tx_flow_mask;
}

/*===========================================================================
FUNCTION  DS_BEARER_MGR_RESET_CACHED_BEARER

DESCRIPTION
  This function resets cached bearer pending while call ended during tech suspension. 

PARAMETERS
  ds_bearer_mgr_s     - bearer_ptr.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_bearer_mgr_reset_cached_bearer
(
  ds_bearer_mgr_s   *bearer_mgr_p
)
{
  if(ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p))
  {
    DS_MSG0(MSG_LEGACY_HIGH, "Freeing the bearer !");
    
    // clean up tx and rx WM
    dsm_queue_destroy(&(bearer_mgr_p->tx_wm.wm_item));      
    dsm_queue_destroy(&(bearer_mgr_p->rx_wm.wm_item));
  
    // Reset the cookie
    bearer_mgr_p->cookie = 0;

    ds_bearer_manager_cache[bearer_mgr_p->index] = NULL;
  
    DS_SYSTEM_HEAP_MEM_FREE(bearer_mgr_p);
  }
}

#if defined FEATURE_APPSRV_TEST_TOOL || defined TEST_FRAMEWORK
/*===========================================================================
FUNCTION DS_BEARER_MGR_DIAG_TOOL_FLOW_CONTROL

DESCRIPTION
  This function does flow control based on commands from DIAG tool

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds_bearer_mgr_diag_tool_flow_control
(
  boolean is_bearer_flow_enabled,
  uint32  flow_val
)
{
  ds_bearer_mgr_s   *bearer_p = NULL;
  bearer_p = ds_bearer_mgr_get_instance_ptr(0);

  if (bearer_p == NULL)
  {
    DS_MSG0(MSG_LEGACY_ERROR,"ds_bearer_mgr_diag_tool_flow_control Bearer"
                               "is NULL");
    return;
  }

  DS_MSG3(MSG_LEGACY_HIGH,"ds_bearer_mgr_diag_tool_flow_control %d, flow_val %d"
          "bearer %p",is_bearer_flow_enabled,flow_val,bearer_p);
  
  if (is_bearer_flow_enabled)
  {
    if (flow_val == 1)
    {
      ds_bearer_mgr_enable_tx_flow(bearer_p ,DS_BEARER_CONTROL_MASK);
    }
    else if (flow_val == 2)
    {
      ds_bearer_mgr_enable_tx_flow(bearer_p ,DS_BEARER_TECH_SUSPENSION_MASK);
    }
    else
    {
      DS_MSG2(MSG_LEGACY_ERROR,"ds_bearer_mgr_diag_tool_flow_control Invalid"
                               " flow val from diag",
                              is_bearer_flow_enabled,flow_val);
    }
  }
  else
  {
    if (flow_val == 1)
    {
      ds_bearer_mgr_disable_tx_flow(bearer_p ,DS_BEARER_CONTROL_MASK);
    }
    else if (flow_val == 2)
    {
      ds_bearer_mgr_disable_tx_flow(bearer_p ,DS_BEARER_TECH_SUSPENSION_MASK);
    }
    else
    {
      DS_MSG2(MSG_LEGACY_ERROR,"ds_bearer_mgr_diag_tool_flow_control Invalid"
                               " flow val from diag",
                              is_bearer_flow_enabled,flow_val);
    }
  }
  return;
}

#endif

#ifdef FEATURE_NBIOT_NTN

/*===========================================================================
FUNCTION      DS_PDN_RETRY_DATA_AFTER_GNSS_RECOVERY

DESCRIPTION   This function will retyr the data after gnss recovery on NTN RAT

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/

void ds_bearer_mgr_retry_data_after_gnss_recovery()
{
  uint8                 bearer_index;
  ds_bearer_mgr_s*      bearer_mgr_p = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  for(bearer_index=0;bearer_index < DS_3GPP_MAX_BEARER_CONTEXT;bearer_index++)
  {  
    bearer_mgr_p = ds_bearer_manager[bearer_index]; 
    
    if(!ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p) ||
       (bearer_mgr_p->state == DS_BEARER_MGR_STATE_DOWN ||
        bearer_mgr_p->state == DS_BEARER_MGR_STATE_GOING_DOWN))
    {
      continue;
    }

    if(bearer_mgr_p->tx_wm.wm_item.current_cnt > 0)
    {
      DS_MSG1(MSG_LEGACY_HIGH, "Pending data in bearer %d", bearer_index);
       ds_bearer_mgr_reorig_dorm(bearer_mgr_p);
      return;
    }
  }
  
  return;
} /* ds_bearer_mgr_is_data_pending */

#endif /* FEATURE_NBIOT_NTN */
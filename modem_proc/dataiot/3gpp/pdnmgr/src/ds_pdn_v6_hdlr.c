/*===========================================================================
                      DS_PDN_V6_HDLR.C
DESCRIPTION
This file defines PDN v6 handler APIs.

Copyright (c) 2018 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/3gpp/pdnmgr/src/ds_pdn_v6_hdlr.c#1 $
  $DateTime: 2020/03/17 08:03:16 $$Author: pwbldsvc $

when           who    what, where, why
--------    ---    ----------------------------------------------------------
08/09/17    sd      First checkin
===========================================================================*/


/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#include "ds_pdn_defs.h"
#include "ds_pdn_v6_hdlr.h"
#include "ps_ip6_sm.h"
#include "ds_pdn_manager.h"
#include "ds_flow.h"
#include "ds_pdn_nv_manager.h"

/*===========================================================================

                    FORWARD  DECLARATIONS FOR MODULE

===========================================================================*/
/*===========================================================================
 FUNCTION DS_PDN_V6_HANDLE_IP6_EV_CBACK()
 
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
static void ds_pdn_v6_handle_ip6_ev_cback
(
  ip6_sm_type             *ip6_inst_ptr,
  ip6_sm_indidcation_type  ind,
  void                    *user_data
);

/*===========================================================================
FUNCTION DS_PDN_V6_TRANSITION_STATE()

DESCRIPTION
  This function is called whenever the state changes.
  The variable is set and housekeeping done for each state.
  
  Any change to state should be through this function only.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Changes the state.
===========================================================================*/
static void ds_pdn_v6_transition_state
(
  ds_pdn_v6_sm_cb_type     *instance_ptr,          /* DSSNET6 SM instance    */
  ds_pdn_v6_state_type      new_state               /* new state to change to */
);

/*===========================================================================

                      EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
FUNCTION DS_PDN_V6_SM_CREATE()

DESCRIPTION
  This function is is used to initialize the v6 state machine control 
  block. Called by the mode specific handler. The mode specific handler
  allocates memory for the control block as well as the ps iface.

DEPENDENCIES
  None

RETURN VALUE
 -1 on failure
  0 on success

SIDE EFFECTS
  None
===========================================================================*/
int ds_pdn_v6_sm_create
(
  ds_pdn_v6_sm_cb_type  *instance_ptr,       /* Ptr to DSSNET6 SM instance */
  ds_pdn_mgr_s          *pdn_mgr_p,
  ip6_sm_type           *ip6_inst_ptr        /* Ptr to IP6 SM instance     */
)
{

  struct ps_in6_addr *nv_primary_dns_ptr   = NULL;
  struct ps_in6_addr *nv_secondary_dns_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Make sure the DSSNET6 control block and the ps iface are not NULL.
  -------------------------------------------------------------------------*/
  if (instance_ptr == NULL)
  {
    return -1;
  }

  instance_ptr->state            = DS_PDN_V6_CLOSED_STATE;
  instance_ptr->pdn_mgr_p        = pdn_mgr_p;
  instance_ptr->ip6_instance     = ip6_inst_ptr;
  instance_ptr->call_end_reason  = DS_NET_DOWN_REASON_NOT_SPECIFIED;

  nv_primary_dns_ptr = (struct ps_in6_addr*)
                       ds_pdn_nv_manager_get_ipv6_primary_dns_addr();
  nv_secondary_dns_ptr = (struct ps_in6_addr*)
                       ds_pdn_nv_manager_get_ipv6_secondary_dns_addr();
  if ((NULL == nv_primary_dns_ptr) || (NULL == nv_secondary_dns_ptr))
  {
    return -1;
  }

  instance_ptr->nv_primary_dns.in6_u.u6_addr64[0] 
                                    = nv_primary_dns_ptr->in6_u.u6_addr64[0];
  instance_ptr->nv_primary_dns.in6_u.u6_addr64[1] 
                                    = nv_primary_dns_ptr->in6_u.u6_addr64[1];
  instance_ptr->nv_secondary_dns.in6_u.u6_addr64[0] 
                                    = nv_secondary_dns_ptr->in6_u.u6_addr64[0];
  instance_ptr->nv_secondary_dns.in6_u.u6_addr64[1] 
                                    = nv_secondary_dns_ptr->in6_u.u6_addr64[1];

  /*-------------------------------------------------------------------------
    Register for IP6 UP/DOWN Events
  -------------------------------------------------------------------------*/
  if (-1 == ip6_sm_ind_cback_reg( instance_ptr->ip6_instance,
                            (ip6_sm_cback_type)ds_pdn_v6_handle_ip6_ev_cback,
                            instance_ptr))
  {
    DS_ERROR_FATAL("ip6_sm_ind_cback_reg() returned -1");
  }

  DS_MSG1(MSG_LEGACY_HIGH,"DS PDN SM for PDN: %d initialized",
                          pdn_mgr_p->index);
  return 0;
} /* ds_pdn_v6_sm_create() */

/*===========================================================================
FUNCTION DS_PDN_V6_PROCESS_LINK_UP_EV()

DESCRIPTION
  This function processes the DS_PDN_V6_LINK_UP_EV. Changes state to 
  DS_PDN_V6_WAITING_FOR_NET_UP_STATE.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_pdn_v6_process_link_up_ev
(
  ds_pdn_v6_sm_cb_type *instance_ptr            /* dssnet6 instance ptr      */
)
{
  switch (instance_ptr->state)
  {
    case DS_PDN_V6_CLOSED_STATE:
      ds_pdn_v6_transition_state(instance_ptr,
                                 DS_PDN_V6_WAITING_FOR_NET_UP_STATE);      
      break;

    case DS_PDN_V6_WAITING_FOR_NET_UP_STATE:
    case DS_PDN_V6_NET_UP_STATE:
    case DS_PDN_V6_WAITING_FOR_NET_DOWN_STATE:
    default:
      break;
  } /* end switch */

} /* ds_pdn_v6_process_link_up_ev() */

/*===========================================================================
FUNCTION DSSNET6I_PROCESS_LINK_DOWN_EV()

DESCRIPTION
  This function processes the DS_PDN_V6_LINK_DOWN_EV. Changes state to 
  DS_PDN_V6_CLOSED_STATE.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_pdn_v6_process_link_down_ev
(
  ds_pdn_v6_sm_cb_type    *instance_ptr,            /* dssnet6 instance ptr      */
  ds_net_down_reason_type  down_reason
)
{
  switch (instance_ptr->state)
  {
    case DS_PDN_V6_CLOSED_STATE:
     /*---------------------------------------------------------------------
          Handle LINK_DOWN_EV.
        ---------------------------------------------------------------------*/
      ds_pdn_mgr_handle_v6_inst_down(instance_ptr->pdn_mgr_p, down_reason);     
      break;

    case DS_PDN_V6_NET_UP_STATE:
    case DS_PDN_V6_WAITING_FOR_NET_UP_STATE:
      /*---------------------------------------------------------------------
        Exit IPv6.
      ---------------------------------------------------------------------*/
      ds_pdn_v6_transition_state(instance_ptr,
                                 DS_PDN_V6_WAITING_FOR_NET_DOWN_STATE);
      if (ip6_sm_post_event(instance_ptr->ip6_instance, IP6_SM_ABORT_EV, NULL) 
          == -1)
      {
        DS_ASSERT(0);
      }      

      break;

    case DS_PDN_V6_WAITING_FOR_NET_DOWN_STATE:
      /*---------------------------------------------------------------------
          This is possibly a scenario, when Client has initiated a call
          end but, before IP6 SM can post NET_DOWN, the physlink
          was terminated by the network. In such cases, clean up
          the DSSNET6 SM
        ---------------------------------------------------------------------*/
      ds_pdn_v6_transition_state(instance_ptr, DS_PDN_V6_CLOSED_STATE);
      break;

    default:
      /*---------------------------------------------------------------------
        Sanity Check
      ---------------------------------------------------------------------*/
      DS_ASSERT(0);
  } /* end switch */

  return;
} /* ds_pdn_v6_process_link_down_ev() */


/*===========================================================================
FUNCTION DS_PDN_V6_PROCESS_NET_UP_EV()

DESCRIPTION
  This function processes the DS_PDN_V6_NET_UP_EV. Changes state to 
  DS_PDN_V6_NET_UP_STATE.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void ds_pdn_v6_process_net_up_ev
(
  ds_pdn_v6_sm_cb_type *instance_ptr            /* dssnet6 instance ptr      */
)
{
  switch (instance_ptr->state)
  {
    case DS_PDN_V6_CLOSED_STATE:
    case DS_PDN_V6_NET_UP_STATE:
    case DS_PDN_V6_WAITING_FOR_NET_DOWN_STATE:
      break;

    case DS_PDN_V6_WAITING_FOR_NET_UP_STATE:
      ds_pdn_mgr_handle_v6_ev_cback(instance_ptr,
                                    DS_PDN_V6_NET_UP_EV);
      ds_pdn_v6_transition_state(instance_ptr, DS_PDN_V6_NET_UP_STATE);      
      break;

    default:
      /*---------------------------------------------------------------------
        Sanity Check
      ---------------------------------------------------------------------*/
      DS_ASSERT(0);
  } /* end switch */

  return;
} /* ds_pdn_v6_process_net_up_ev() */



/*===========================================================================
FUNCTION DS_PDN_V6_PROCESS_NET_DOWN_EV()

DESCRIPTION
  This function processes the DS_PDN_V6_NET_DOWN_EV. Changes state to 
  DS_PDN_V6_WAITING_FOR_NET_DOWN_STATE.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void ds_pdn_v6_process_net_down_ev
(
  ds_pdn_v6_sm_cb_type *instance_ptr            /* dssnet6 instance ptr      */
)
{
  ds_pdn_mgr_s         *pdn_mgr_p = NULL;

  switch (instance_ptr->state)
  {
    case DS_PDN_V6_CLOSED_STATE:
      break;

    case DS_PDN_V6_WAITING_FOR_NET_UP_STATE:
    case DS_PDN_V6_NET_UP_STATE:     
      pdn_mgr_p = instance_ptr->pdn_mgr_p;
      ds_pdn_mgr_handle_v6_ev_cback(instance_ptr,
                                    DS_PDN_V6_NET_DOWN_EV);            

      /* This is to handle the case where v6 instance clean up happens
         as part of net down event posted above. */
      if(ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p) &&
         pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV6] != NULL)
      {           
        ds_pdn_v6_transition_state(instance_ptr,
                                   DS_PDN_V6_WAITING_FOR_NET_DOWN_STATE);
        ds_pdn_mgr_bring_down_bearer(pdn_mgr_p, DS_PDN_INSTANCE_IPV6);      
      }
      break;

    case DS_PDN_V6_WAITING_FOR_NET_DOWN_STATE: 
      ds_pdn_mgr_handle_v6_ev_cback(instance_ptr,
                                    DS_PDN_V6_NET_DOWN_EV);
      break;

    default:
      /*---------------------------------------------------------------------
        Sanity Check
      ---------------------------------------------------------------------*/
      DS_ASSERT(0);
  } /* end switch */

  return;
} /* ds_pdn_v6_process_net_down_ev() */
/*===========================================================================
FUNCTION DS_PDN_V6_PROCESS_NET_SOL_WAIT_EV()

DESCRIPTION
  This function processes the DS_PDN_V6_NET_SOL_WAIT_EV. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void ds_pdn_v6_process_net_sol_wait_ev
(
  ds_pdn_v6_sm_cb_type      *instance_ptr      /* dssnet6 instance ptr      */
)
{ 
  switch (instance_ptr->state)
  {
    case DS_PDN_V6_WAITING_FOR_NET_UP_STATE:      
      ds_pdn_mgr_handle_v6_ev_cback(instance_ptr,
                                    DS_PDN_V6_NET_SOL_WAIT_EV);
      break;
      
    case DS_PDN_V6_CLOSED_STATE:
    case DS_PDN_V6_NET_UP_STATE: 
    case DS_PDN_V6_WAITING_FOR_NET_DOWN_STATE:
      break;

    default:
      DS_ASSERT(0);
      break;
  } /* switch */

  return;
}/* ds_pdn_v6_process_net_sol_wait_ev */

/*===========================================================================
FUNCTION DS_PDN_V6_PROCESS_NET_ABORTED_EV()

DESCRIPTION
  This function processes the DS_PDN_V6_NET_ABORTED_EV. Changes state to 
  DS_PDN_V6_CLOSED_STATE.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void ds_pdn_v6_process_net_aborted_ev
(
  ds_pdn_v6_sm_cb_type      *instance_ptr      /* dssnet6 instance ptr      */
)
{
  switch (instance_ptr->state)
  {
    case DS_PDN_V6_CLOSED_STATE:
      break;

    case DS_PDN_V6_WAITING_FOR_NET_DOWN_STATE:      
      ds_pdn_v6_transition_state(instance_ptr, DS_PDN_V6_CLOSED_STATE);  
      break;
    /*----------------------------------------------------------------------- 
      NET_UP_STATE (or) WAITING FOR NET_UP STATE
      -----------------------------------------------------------------------*/
    default:
      DS_ASSERT(0);
      break;
  }

  return;
}/* ds_pdn_v6_process_net_aborted_ev() */

/*===========================================================================
FUNCTION DS_PDN_V6_PROCESS_IP6_STOP_EV()

DESCRIPTION
  This function processes the DS_PDN_V6_NET_ABORTED_EV. Changes state to 
  DS_PDN_V6_CLOSED_STATE.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_pdn_v6_process_ip6_stop_ev
(
  ds_pdn_v6_sm_cb_type      *instance_ptr      /* dssnet6 instance ptr      */
)
{
  ds_pdn_v6_transition_state(instance_ptr,
                             DS_PDN_V6_WAITING_FOR_NET_DOWN_STATE);

  /*---------------------------------------------------------------------
     Exit IPv6.
    ---------------------------------------------------------------------*/
  if (ip6_sm_post_event(instance_ptr->ip6_instance, IP6_SM_STOP_EV, NULL) 
      == -1)
  {
    DS_ASSERT(0);
  }
  
  return;
}/* ds_pdn_v6_process_ip6_stop_ev() */

/*===========================================================================
FUNCTION DS_PDN_V6_HANDLE_IP6_EV_CBACK()

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
static void ds_pdn_v6_handle_ip6_ev_cback
(
  ip6_sm_type             *ip6_inst_ptr,
  ip6_sm_indidcation_type  ind,
  void                    *user_data
)
{
  ds_pdn_v6_sm_cb_type    *instance_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  instance_ptr = (ds_pdn_v6_sm_cb_type*)(user_data); 
  if(instance_ptr == NULL)
  {
     return;
  }
  
  DS_MSG3(MSG_LEGACY_HIGH,
          "Recd IP6 event %d for DS PDN %d in state %d", 
          ind, instance_ptr->pdn_mgr_p->index, instance_ptr->state);

  switch (ind)
  {
    case IP6_SM_DOWN_IND:
      ds_pdn_v6_process_net_down_ev(instance_ptr);
      break;

    case IP6_SM_UP_IND:
      ds_pdn_v6_process_net_up_ev(instance_ptr);
      break;

    case IP6_SM_ABORTED_IND:
      ds_pdn_v6_process_net_aborted_ev(instance_ptr);
      break;  

    case IP6_SM_SOL_WAIT_IND:
       ds_pdn_v6_process_net_sol_wait_ev(instance_ptr);
      break;  

    default:
      break;
  }

  return;
} /* ds_pdn_v6_handle_ip6_ev_cback() */

/*===========================================================================
FUNCTION DS_PDN_V6_TRANSITION_STATE()

DESCRIPTION
  This function is called whenever the state changes.
  The variable is set and housekeeping done for each state.
  
  Any change to state should be through this function only.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Changes the state.
===========================================================================*/
static void ds_pdn_v6_transition_state
(
  ds_pdn_v6_sm_cb_type     *instance_ptr,          /* DSSNET6 SM instance    */
  ds_pdn_v6_state_type      new_state               /* new state to change to */
)
{
  ds_pdn_mgr_s             *pdn_mgr_p = NULL;
  ds_pdn_mgr_inst_type_s   *inst_p = NULL;
  ds_fwk_s_type            *fw_inst_p = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (NULL == instance_ptr) 
  {
    DS_ASSERT(0);
    return;
  }

  pdn_mgr_p = instance_ptr->pdn_mgr_p;
  if(pdn_mgr_p == NULL)
  {
    return;
  }
  
  inst_p = pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV6];
  if(inst_p == NULL)
  {
    return;
  }
  
  /*-------------------------------------------------------------------------
    Changing the state variable.
  -------------------------------------------------------------------------*/
  DS_MSG3(MSG_LEGACY_HIGH,"DS PDN V6 SM 0x%x transition state %d to %d",
                  instance_ptr,
                  instance_ptr->state,
                  new_state);
  instance_ptr->state = new_state;

  /*-------------------------------------------------------------------------
    Executing state based functionality.
  -------------------------------------------------------------------------*/
  switch (instance_ptr->state)
  {
    case DS_PDN_V6_CLOSED_STATE:
        /*-------------------------------------------------------------------
          Delete all prefixes associated with this interface.
        -------------------------------------------------------------------*/
        /*(void) ps_iface_delete_all_v6_prefixes(instance_ptr->ps_iface_ptr);*/

        ds_flow_go_null_ind(inst_p->flow->default_flow_ptr);        
        ds_pdn_mgr_handle_v6_inst_down(instance_ptr->pdn_mgr_p, 
        	                              instance_ptr->call_end_reason);     
        break;

    case DS_PDN_V6_WAITING_FOR_NET_UP_STATE:
      /*---------------------------------------------------------------------
        Start IPv6
      ---------------------------------------------------------------------*/
      if ((boolean)(instance_ptr->user_data_p) == TRUE)
      {
        if ((ip6_sm_set_passive(instance_ptr->ip6_instance)) == -1)
        {
          DS_ERROR_LOG_0("ip6_sm_set_passive() failed ");
        }
      }

      if ((ip6_sm_post_event( instance_ptr->ip6_instance,
                              IP6_SM_START_EV, NULL)) == -1)
      {
        DS_ERROR_FATAL("ip6_sm_post_event() failed ");
      }
      break;

    case DS_PDN_V6_NET_UP_STATE:             
      if (instance_ptr->pdsn_as_proxy_ipv6_dns_server)
      {
        struct ps_in6_addr v6;
        memset(&v6, 0, sizeof(struct ps_in6_addr));        
        
        fw_inst_p = ds_fwk_get_inst_ptr_from_index(pdn_mgr_p->fw_index);
        if(!fw_inst_p)
        {
          DS_ERROR_FATAL("NULL FWK ptr !");
          return;
        }       
      
        if(fw_inst_p->v6_net_info.gateway_iid)
        {
          v6.ps_s6_addr32[0] = ps_htonl(0xFE800000UL);
          v6.ps_s6_addr32[1] = 0;
          v6.ps_s6_addr64[1] = fw_inst_p->v6_net_info.gateway_iid;
        }
             
        fw_inst_p->v6_net_info.primary_dns = v6;
        fw_inst_p->v6_net_info.secondary_dns = v6;
      }
    /*---------------------------------------------------------------------
        Send instance up indication.
      ---------------------------------------------------------------------*/
      ds_pdn_mgr_instance_up_ind(pdn_mgr_p, inst_p); 
      break;

    case DS_PDN_V6_WAITING_FOR_NET_DOWN_STATE:
      break;

    default:
      DS_ASSERT(0);
  } /* switch(state) */

} /* ds_pdn_v6_transition_state() */
